#pragma once
#include "PluginEditor.h"
#include <chrono>
#include <cstdio>
inline int runWebUiSmoke(const juce::String& reportPath) {
    using Clock=std::chrono::steady_clock;
    int checks=0;
    const auto pump=[](int ms){juce::MessageManager::getInstance()->runDispatchLoopUntil(ms);};
    const auto wait=[&](auto condition,int timeout=30000) {
        const auto end=Clock::now()+std::chrono::milliseconds(timeout);
        do {if(condition())return true;pump(25);}while(Clock::now()<end);
        return condition();
    };
    BloodyMasterProcessor p;p.setRateAndBufferSizeDetails(48000,512);p.prepareToPlay(48000,512);p.setCurrentProgram(3);
    const auto userDirectory=juce::File::getSpecialLocation(juce::File::tempDirectory).getNonexistentChildFile("BloodyMaster-ui-user-tests","",false);
    struct Cleanup {juce::File dir;~Cleanup(){dir.deleteRecursively();}} cleanup{userDirectory};
    auto editor=std::make_unique<BloodyMasterEditor>(p,userDirectory);
    editor->addToDesktop(juce::ComponentPeer::windowIsTemporary);editor->setVisible(true);
    const auto verify=[&](bool success,const char* name){++checks;std::printf("%s %s\n",success?"PASS":"FAIL",name);return success;};
    if(!verify(wait([&]{return editor->isUiReady();}),"WebView2 runtime page ready"))return 1;
    const auto evaluate=[&](const juce::String& script) -> juce::var {
        struct Capture { bool done=false;juce::var value; };
        auto capture=std::make_shared<Capture>();
        editor->evaluateForTesting(script,[capture](juce::WebBrowserComponent::EvaluationResult r){
            if(auto* result=r.getResult())capture->value=*result;capture->done=true;
        });
        if(!wait([&]{return capture->done;},10000))return {};
        return capture->value;
    };
    if(!verify(wait([&]{return static_cast<bool>(evaluate("!!window.bloodyMasterUi?.getState()"));}),"Native state reaches HTML"))return 2;
    if(!verify(static_cast<bool>(evaluate("document.querySelectorAll('.knob-base').length===38 && document.querySelector('#knob-drive').getBoundingClientRect().width===80 && getComputedStyle(document.querySelector('.wood-panel')).backgroundImage.includes('linear-gradient')")),"Approved layout and offline CSS loaded"))return 3;
    evaluate("document.querySelector('#knob-drive').dispatchEvent(new KeyboardEvent('keydown',{key:'ArrowUp',bubbles:true}));true");
    if(!verify(wait([&]{return p.apvts.getRawParameterValue(bm::id::drive)->load()>1.1f;}),"HTML knob updates APVTS"))return 4;
    auto* drive=p.apvts.getParameter(bm::id::drive);drive->setValueNotifyingHost(drive->convertTo0to1(6.3f));
    if(!verify(wait([&]{return static_cast<bool>(evaluate("Math.abs(Number(document.querySelector('#knob-drive').dataset.val)-6.3)<.001"));}),"Host automation updates knob"))return 5;
    evaluate("document.querySelector('#presetSelector').value='8';document.querySelector('#presetSelector').dispatchEvent(new Event('change'));true");
    if(!verify(wait([&]{return p.getCurrentProgram()==8;}),"HTML factory preset selection"))return 6;
    if(!verify(wait([&]{return static_cast<bool>(evaluate("window.bloodyMasterUi.getState().preset===8"));}),"Preset recall returns to HTML"))return 7;
    evaluate("document.querySelector('#bypassBtn').click();true");
    if(!verify(wait([&]{return p.apvts.getRawParameterValue(bm::id::bypass)->load()>.5f;}),"HTML bypass reaches audio parameter"))return 8;
    evaluate("document.querySelector('#abBtn').click();true");pump(100);
    drive->setValueNotifyingHost(drive->convertTo0to1(7.f));
    evaluate("document.querySelector('#abBtn').click();true");
    if(!verify(wait([&]{return std::abs(p.apvts.getRawParameterValue(bm::id::drive)->load()-2.f)<.001f;}),"A/B recalls parameter snapshot"))return 9;
    p.apvts.getParameter(bm::id::bypass)->setValueNotifyingHost(0);
    juce::AudioBuffer<float> audio(2,512);juce::MidiBuffer midi;
    for(int block=0;block<3;++block) {
        for(int i=0;i<512;++i){const auto v=.2f*std::sin(2*juce::MathConstants<float>::pi*997*(block*512+i)/48000);audio.setSample(0,i,v);audio.setSample(1,i,v*.5f);}
        p.processBlock(audio,midi);
    }
    if(!verify(wait([&]{return static_cast<bool>(evaluate("(()=>{const s=window.bloodyMasterUi.getState();return s.meters.outputL>0 && s.scope.out[0].some(v=>Math.abs(v)>.001) && s.latency===848 && s.sampleRate===48000;})()"));}),"Real audio samples, meters, Fs and latency reach HTML"))return 10;
    if(!verify(static_cast<bool>(evaluate("document.querySelector('#knob-de-amount').getAttribute('aria-disabled')==='false' && window.bloodyMasterUi.getState().parameters.mud_active.value===0")),"Cleanup controls active and traps off by default"))return 11;
    evaluate("document.querySelector('#mudActiveBtn').click();document.querySelector('#knob-mud-amount').dispatchEvent(new KeyboardEvent('keydown',{key:'ArrowUp',bubbles:true}));true");
    if(!verify(wait([&]{return p.apvts.getRawParameterValue(bm::id::mudActive)->load()>.5f && p.apvts.getRawParameterValue(bm::id::mudAmount)->load()>25.f && p.apvts.getRawParameterValue(bm::id::ringActive)->load()<.5f;}),"Independent Mud activation and amount reach APVTS"))return 12;
    evaluate("document.querySelector('#ringActiveBtn').click();document.querySelector('#deSoloBtn').click();true");
    if(!verify(wait([&]{return p.apvts.getRawParameterValue(bm::id::ringActive)->load()>.5f && p.apvts.getRawParameterValue(bm::id::spikeListen)->load()>.5f;}),"Ring activation and delta listen reach APVTS"))return 13;
    auto* ring=p.apvts.getParameter(bm::id::ringAmount);ring->setValueNotifyingHost(ring->convertTo0to1(61.f));
    if(!verify(wait([&]{return static_cast<bool>(evaluate("Math.abs(Number(document.querySelector('#knob-ring-amount').dataset.val)-61)<.001"));}),"Host automation updates new Ring amount"))return 14;
    evaluate("document.querySelector('#thdColorBtn').click();document.querySelector('#knob-xl-mode').dispatchEvent(new KeyboardEvent('keydown',{key:'ArrowUp',bubbles:true}));true");
    if(!verify(wait([&]{return p.apvts.getRawParameterValue(bm::id::thdMode)->load()>.5f && p.apvts.getRawParameterValue(bm::id::xlMode)->load()==1.f;}),"THD color and discrete XL mode reach APVTS"))return 15;
    auto* xlMode=p.apvts.getParameter(bm::id::xlMode);xlMode->setValueNotifyingHost(xlMode->convertTo0to1(2.f));
    if(!verify(wait([&]{return static_cast<bool>(evaluate("document.querySelector('#val-xl_mode').textContent==='Impact'"));}),"XL mode automation updates rotary label"))return 16;
    const auto originalGlue=p.apvts.getRawParameterValue(bm::id::glue)->load();
    evaluate("document.querySelector('#tapeModeBtn').click();document.querySelector('#knob-tape-color').dispatchEvent(new KeyboardEvent('keydown',{key:'ArrowUp',bubbles:true}));true");
    if(!verify(wait([&]{return p.apvts.getRawParameterValue(bm::id::tapeMode)->load()==1.f && p.apvts.getRawParameterValue(bm::id::tapeColor)->load()>0.f && p.apvts.getRawParameterValue(bm::id::glue)->load()==originalGlue;}),"Tape colour and character are independent of Glue compression"))return 17;
    auto* tape=p.apvts.getParameter(bm::id::tapeColor);tape->setValueNotifyingHost(tape->convertTo0to1(37.f));
    if(!verify(wait([&]{return static_cast<bool>(evaluate("Math.abs(Number(document.querySelector('#knob-tape-color').dataset.val)-37)<.001"));}),"Tape colour host automation returns to HTML"))return 18;
    evaluate("document.querySelector('.profile-btn[data-prof=\"4\"]').click();document.querySelector('#knob-enhance').dispatchEvent(new KeyboardEvent('keydown',{key:'ArrowUp',bubbles:true}));true");
    if(!verify(wait([&]{return p.apvts.getRawParameterValue(bm::id::profile)->load()==3.f && p.apvts.getRawParameterValue(bm::id::enhance)->load()>0.f;}),"Atmosphere and profile controls reach APVTS"))return 19;
    auto* profile=p.apvts.getParameter(bm::id::profile);profile->setValueNotifyingHost(profile->convertTo0to1(0.f));
    if(!verify(wait([&]{return static_cast<bool>(evaluate("document.querySelector('.profile-btn[data-prof=\"1\"]').getAttribute('aria-pressed')==='true'"));}),"Profile host automation updates selection"))return 20;
    p.apvts.getParameter(bm::id::spikeListen)->setValueNotifyingHost(0);
    for(int block=0;block<310;++block){
        for(int i=0;i<512;++i){const float v=.1f*std::sin(2*juce::MathConstants<float>::pi*1000*(block*512+i)/48000);audio.setSample(0,i,v);audio.setSample(1,i,v);}
        p.processBlock(audio,midi);
    }
    if(!verify(wait([&]{return static_cast<bool>(evaluate("(()=>{const m=window.bloodyMasterUi.getState().loudness;return m.integrated>-100 && m.shortTerm>-100 && m.truePeak>0 && !document.querySelector('#loudnessReadout').textContent.includes('—');})()"));}),"Real integrated, short term and true peak reach HTML"))return 21;
    evaluate("document.querySelector('#resetMetersBtn').click();true");pump(100);
    audio.clear();p.processBlock(audio,midi);
    if(!verify(wait([&]{return static_cast<bool>(evaluate("window.bloodyMasterUi.getState().loudness.integrated===-200 && window.bloodyMasterUi.getState().loudness.shortTerm===-200"));}),"Manual meter reset reaches audio thread"))return 22;
    evaluate("document.querySelector('#monitorMatchBtn').click();true");
    if(!verify(wait([&]{return p.isMonitorMatchEnabled();}),"Temporary monitor compensation can be enabled"))return 23;
    evaluate("document.querySelector('#monitorMatchBtn').click();true");
    if(!verify(wait([&]{return !p.isMonitorMatchEnabled();}),"Monitor compensation can be disabled before export"))return 24;
    drive->setValueNotifyingHost(drive->convertTo0to1(3.5f));
    evaluate(R"JS(document.querySelector('#saveUserPresetBtn').click();document.querySelector('#userPresetName').value='Test User é808';document.querySelector('#userPresetForm').requestSubmit();true)JS");
    if(!verify(wait([&]{return static_cast<bool>(evaluate("window.bloodyMasterUi.getState().userPresets.length===1 && !!window.bloodyMasterUi.getState().selectedUserPreset && !document.querySelector('#userPresetDialog').open"));}),"Save named User preset through HTML"))return 25;
    p.setCurrentProgram(0);
    evaluate("document.querySelector('#presetSelector').value='user:'+window.bloodyMasterUi.getState().userPresets[0].id;document.querySelector('#presetSelector').dispatchEvent(new Event('change'));true");
    if(!verify(wait([&]{return std::abs(p.apvts.getRawParameterValue(bm::id::drive)->load()-3.5f)<.001f;}),"User selection restores audio parameters"))return 26;
    evaluate(R"JS(document.querySelector('#saveUserPresetBtn').click();document.querySelector('#userPresetName').value='Test User é808';document.querySelector('#userPresetForm').requestSubmit();true)JS");
    if(!verify(wait([&]{return static_cast<bool>(evaluate("window.bloodyMasterUi.getState().userPresets.length===2 && window.bloodyMasterUi.getState().userPresets.some(p=>p.name.endsWith(' (2)'))"));}),"Duplicate names preserve originals"))return 27;
    editor->setVisible(false);editor->removeFromDesktop();editor.reset();
    editor=std::make_unique<BloodyMasterEditor>(p,userDirectory);editor->addToDesktop(juce::ComponentPeer::windowIsTemporary);editor->setVisible(true);
    if(!verify(wait([&]{return editor->isUiReady()&&static_cast<bool>(evaluate("window.bloodyMasterUi?.getState()?.userPresets.length===2 && [...document.querySelectorAll('#presetSelector optgroup')].some(g=>g.label==='User'&&g.children.length===2)"));}),"User presets survive editor reopening in their own section"))return 28;
    if(!verify(static_cast<bool>(evaluate("['Rap','Electro','New Rock','Trap'].every(label=>[...document.querySelectorAll('#presetSelector optgroup')].some(g=>g.label===label&&g.children.length===2))")),"Eight inspiration presets appear in four genre sections"))return 29;
    for(int block=0;block<64;++block){for(int i=0;i<512;++i){audio.setSample(0,i,10);audio.setSample(1,i,-10);}p.processBlockBypassed(audio,midi);}
    if(!verify(wait([&]{return static_cast<bool>(evaluate("window.bloodyMasterUi.getState().safetyReductionDb>0 && document.querySelector('#outputSafetyReadout').textContent.includes('Protection sortie') && window.bloodyMasterUi.getState().meters.outputL<0.751"));}),"Global protection and protected output meters reach HTML during host bypass"))return 30;
    const auto deltaSettings=p.apvts.copyState().toXmlString();
    evaluate("document.querySelector('#deltaBtn').click();true");
    const bool deltaActivated=wait([&]{return p.isDeltaEnabled()&&static_cast<bool>(evaluate("document.querySelector('#deltaBtn')?.getAttribute('aria-pressed')==='true'"));});
    if(!verify(deltaActivated,"DELTA button controls transient audition")){
        std::printf("Delta native=%d; browser=%s\n",int(p.isDeltaEnabled()),evaluate("JSON.stringify({delta:window.bloodyMasterUi?.getState()?.delta,button:document.querySelector('#deltaBtn')?.outerHTML})").toString().toRawUTF8());return 31;
    }
    if(!verify(static_cast<bool>(evaluate("document.querySelector('#deltaBtn').title==='\\u00c9couter la diff\\u00e9rence entre le signal original et le signal trait\\u00e9.'")),"DELTA tooltip matches exact French text")){
        std::printf("Delta tooltip: %s\n",evaluate("JSON.stringify(document.querySelector('#deltaBtn').title)").toString().toRawUTF8());return 35;
    }
    if(!verify(p.apvts.copyState().toXmlString()==deltaSettings,"DELTA does not modify processing parameters"))return 32;
    evaluate("document.querySelector('#presetSelector').value='0';document.querySelector('#presetSelector').dispatchEvent(new Event('change'));true");
    if(!verify(wait([&]{return !p.isDeltaEnabled()&&static_cast<bool>(evaluate("document.querySelector('#deltaBtn').getAttribute('aria-pressed')==='false'"));}),"Factory recall disables DELTA"))return 33;
    p.setDeltaEnabled(true);
    evaluate("document.querySelector('#presetSelector').value='user:'+window.bloodyMasterUi.getState().userPresets[0].id;document.querySelector('#presetSelector').dispatchEvent(new Event('change'));true");
    if(!verify(wait([&]{return !p.isDeltaEnabled();}),"User recall disables DELTA"))return 34;
    editor->setVisible(false);editor->removeFromDesktop();editor.reset();
    auto* result=new juce::DynamicObject;result->setProperty("passed",checks);result->setProperty("backend","WebView2 native Windows");
    if(reportPath.isNotEmpty())juce::File(reportPath).replaceWithText(juce::JSON::toString(juce::var(result)));
    else delete result;
    std::printf("PASS %d native WebView2 integration checks\n",checks);return 0;
}
