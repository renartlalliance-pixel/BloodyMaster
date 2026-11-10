#include "PluginEditor.h"
#if BM_HTML_UI
#include "BloodyMasterWebResources.h"
#include "Presets/FactoryPresets.h"
#include <complex>

class BloodyMasterEditor::Browser final : public juce::WebBrowserComponent {
public:
    explicit Browser(const Options& options):WebBrowserComponent(options) {}
    bool pageAboutToLoad(const juce::String& url) override {
        return url=="about:blank" || url.startsWith(getResourceProviderRoot());
    }
    void newWindowAttemptingToLoad(const juce::String&) override {}
};
namespace {
juce::var object() { return juce::var(new juce::DynamicObject); }
void property(juce::var& v,const juce::Identifier& id,const juce::var& value) {v.getDynamicObject()->setProperty(id,value);}
std::optional<juce::WebBrowserComponent::Resource> resource(const juce::String& path) {
    const char* key=nullptr;const char* mime=nullptr;
    if(path=="/" || path=="/index.html") {key="index_html";mime="text/html; charset=utf-8";}
    else if(path=="/tailwind.css") {key="tailwind_css";mime="text/css; charset=utf-8";}
    else if(path=="/bridge.js") {key="bridge_js";mime="application/javascript; charset=utf-8";}
    else return {};
    int size=0;const auto* data=BloodyMasterWebResources::getNamedResource(key,size);
    if(data==nullptr || size<=0) return {};
    const auto* begin=reinterpret_cast<const std::byte*>(data);
    return juce::WebBrowserComponent::Resource{{begin,begin+size},mime};
}
}
BloodyMasterEditor::BloodyMasterEditor(BloodyMasterProcessor& p):AudioProcessorEditor(&p),processor(p) {
    for(const auto& [id,value]:bm::presetParameters(bm::factoryPresets()[0].settings)) {
        juce::ignoreUnused(value);activeIds.insert(id);
    }
    const auto cache=juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
        .getChildFile("BRI Audio Systems/BloodyMaster/WebView2");
    cache.createDirectory();
    using Web=juce::WebBrowserComponent;
    const auto options=Web::Options{}.withBackend(Web::Options::Backend::webview2)
        .withWinWebView2Options(Web::Options::WinWebView2{}.withUserDataFolder(cache).withStatusBarDisabled().withBuiltInErrorPageDisabled())
        .withNativeIntegrationEnabled().withKeepPageLoadedWhenBrowserIsHidden()
        .withResourceProvider(resource)
        .withEventListener("bmReady",[this](const juce::var&){uiReady=true;timerCallback();})
        .withEventListener("bmParam",[this](const juce::var& v){parameterEvent(v);})
        .withEventListener("bmAction",[this](const juce::var& v){actionEvent(v);});
    if(Web::areOptionsSupported(options)) {
        browser=std::make_unique<Browser>(options);addAndMakeVisible(*browser);
        browser->goToURL(Web::getResourceProviderRoot());
    } else {
        error.setText("L'interface HTML demande Microsoft Edge WebView2. Relancez l'installateur BloodyMaster pour installer ce composant.",juce::dontSendNotification);
        error.setJustificationType(juce::Justification::centred);addAndMakeVisible(error);
    }
    setResizable(true,true);setResizeLimits(1100,700,1800,1400);setSize(1240,900);startTimerHz(15);
}
BloodyMasterEditor::~BloodyMasterEditor() {
    stopTimer();browser.reset();
    for(const auto& id:gestures) if(auto* p=processor.apvts.getParameter(id)) p->endChangeGesture();
}
void BloodyMasterEditor::resized() {if(browser)browser->setBounds(getLocalBounds());error.setBounds(getLocalBounds().reduced(40));}
void BloodyMasterEditor::parameterEvent(const juce::var& event) {
    if(!event.isObject()) return;
    const auto id=event["id"].toString(),phase=event["phase"].toString();
    if(activeIds.count(id)==0) return;
    auto* p=processor.apvts.getParameter(id);if(p==nullptr) return;
    if(phase=="begin") {if(gestures.insert(id).second)p->beginChangeGesture();return;}
    if(phase=="end") {if(gestures.erase(id)>0)p->endChangeGesture();return;}
    if(phase!="value" || !(event["value"].isDouble() || event["value"].isInt() || event["value"].isInt64()))return;
    const double v=static_cast<double>(event["value"]);if(!std::isfinite(v))return;
    const auto range=p->getNormalisableRange();
    const float physical=range.snapToLegalValue(juce::jlimit(range.start,range.end,static_cast<float>(v)));
    p->setValueNotifyingHost(p->convertTo0to1(physical));
}
void BloodyMasterEditor::actionEvent(const juce::var& event) {
    if(!event.isObject())return;
    const auto action=event["action"].toString();
    if(action=="preset" && event["index"].isInt())processor.setCurrentProgram(static_cast<int>(event["index"]));
    else if(action=="ab") {
        comparison[comparisonIndex]=processor.apvts.copyState();
        const int next=1-comparisonIndex;
        if(!comparison[next].isValid())comparison[next]=comparison[comparisonIndex].createCopy();
        processor.apvts.replaceState(comparison[next].createCopy());comparisonIndex=next;
    }
    timerCallback();
}
void BloodyMasterEditor::updateCurve(double rate) {
    const auto s=processor.getToneSettingsForUi();
    if(rate==lastCurveRate && s==lastTone)return;
    lastCurveRate=rate;lastTone=s;
    using namespace bm::dsp;
    const double fs=rate>0?rate:48000;
    const auto low=passiveLow(fs,s.bass);
    std::array<Coefficients,11> filters{design(Type::highpass,fs,s.hpf),design(Type::lowpass,fs,s.lpf),low[0],
        design(Type::peak,fs,650,s.mid,.7),design(Type::peak,fs,8000,s.high,.55),design(Type::highShelf,fs,12000,s.presence),
        design(Type::lowShelf,fs,160,s.body*.3),design(Type::peak,fs,s.freq[0],s.eq?s.gain[0]:0,1),
        design(Type::peak,fs,s.freq[1],s.eq?s.gain[1]:0,1),design(Type::peak,fs,s.freq[2],s.eq?s.gain[2]:0,1),low[1]};
    juce::Array<juce::var> points;
    for(int i=0;i<128;++i) {
        const double hz=20*std::pow(std::min(20000.,fs*.45)/20,double(i)/127);
        const auto z=std::polar(1.,-2*pi*hz/fs);std::complex<double> response{1,0};
        for(const auto& c:filters)response*=(c.b0+c.b1*z+c.b2*z*z)/(1.+c.a1*z+c.a2*z*z);
        juce::Array<juce::var> point;point.add(hz);point.add(20*std::log10(std::max(std::abs(response),1.e-12)));points.add(juce::var(point));
    }
    curve=juce::var(points);
}
juce::var BloodyMasterEditor::makeState() {
    auto result=object(),parameters=object();
    for(auto* base:processor.getParameters())if(auto* p=dynamic_cast<juce::RangedAudioParameter*>(base)) {
        auto item=object();const auto id=p->getParameterID();
        property(item,"value",static_cast<double>(processor.apvts.getRawParameterValue(id)->load(std::memory_order_relaxed)));
        property(item,"defaultValue",static_cast<double>(p->convertFrom0to1(p->getDefaultValue())));
        property(item,"active",activeIds.count(id)!=0);property(parameters,id.substring(3),item);
    }
    property(result,"parameters",parameters);
    juce::Array<juce::var> presets;int selected=-1;
    const auto& bank=bm::factoryPresets();
    for(size_t i=0;i<bank.size();++i) {
        presets.add(bank[i].name);bool matches=true;
        for(const auto& [id,value]:bm::presetParameters(bank[i].settings))
            if(std::abs(processor.apvts.getRawParameterValue(id)->load(std::memory_order_relaxed)-value)>1.e-3f){matches=false;break;}
        if(matches)selected=static_cast<int>(i);
    }
    property(result,"presets",juce::var(presets));property(result,"preset",selected);property(result,"ab",comparisonIndex==0?"A":"B");
    const double rate=processor.getDspSampleRate();property(result,"sampleRate",rate);property(result,"latency",processor.getLatencySamples());
    const auto tone=processor.getToneSettingsForUi();
    property(result,"lpfEffective",tone.lpf>=(rate>0?rate:48000)*.499?(rate>0?rate:48000)*.5:std::min(tone.lpf,(rate>0?rate:48000)*.45));
    updateCurve(rate);property(result,"eq",curve);
    const auto m=processor.getMeterValues();auto meters=object();
    property(meters,"inputL",m.inputL);property(meters,"inputR",m.inputR);property(meters,"outputL",m.outputL);property(meters,"outputR",m.outputR);
    property(meters,"compressorGr",m.compressorGr);property(meters,"glueGr",m.glueGr);property(meters,"clipGr",m.clipGr);property(meters,"limiterGr",m.limiterGr);
    property(result,"meters",meters);
    if(auto frame=processor.getScopeFrame()) {
        auto traces=object();
        for(int direction=0;direction<2;++direction) {
            juce::Array<juce::var> stereo;
            for(int channel=0;channel<2;++channel) {
                juce::Array<juce::var> values;
                for(int i=0;i<frame->samples;++i)values.add(static_cast<double>(frame->channels[static_cast<size_t>(2*direction+channel)][static_cast<size_t>(i)]));
                stereo.add(juce::var(values));
            }
            property(traces,direction==0?"in":"out",juce::var(stereo));
        }
        property(result,"scope",traces);
    }
    return result;
}
void BloodyMasterEditor::timerCallback() {if(browser && uiReady)browser->emitEventIfBrowserIsVisible("bmState",makeState());}
#if BM_UI_TESTING
void BloodyMasterEditor::evaluateForTesting(const juce::String& script,juce::WebBrowserComponent::EvaluationCallback callback) {
    if(browser)browser->evaluateJavascript(script,std::move(callback));
}
#endif
#endif
