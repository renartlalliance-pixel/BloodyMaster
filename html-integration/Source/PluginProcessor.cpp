#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Parameters/ParameterIDs.h"
#include "Parameters/ParameterLayout.h"
#include "Presets/FactoryPresets.h"

void BloodyMasterProcessor::publishMeters() {
    const auto& m=mastering.meters;
    const std::array<double,12> values{m.inputPeak,finalOutput[0],m.compressorGr,m.glueGr,m.clipGr,m.limiterGr,
        m.inputL,m.inputR,finalOutput[1],finalOutput[2],finalOutput[3],finalOutput[4]};
    for(size_t i=0;i<values.size();++i) meterValues[i].store(static_cast<float>(values[i]),std::memory_order_relaxed);
}
bm::dsp::MeterValues BloodyMasterProcessor::getMeterValues() const {
    return {meterValues[0].load(std::memory_order_relaxed),meterValues[1].load(std::memory_order_relaxed),
            meterValues[2].load(std::memory_order_relaxed),meterValues[3].load(std::memory_order_relaxed),
            meterValues[4].load(std::memory_order_relaxed),meterValues[5].load(std::memory_order_relaxed),
            meterValues[6].load(std::memory_order_relaxed),meterValues[7].load(std::memory_order_relaxed),
            meterValues[8].load(std::memory_order_relaxed),meterValues[9].load(std::memory_order_relaxed),
            meterValues[10].load(std::memory_order_relaxed),meterValues[11].load(std::memory_order_relaxed)};
}
void BloodyMasterProcessor::captureScope(const juce::AudioBuffer<float>& buffer,bool input) {
    if(input) scopeSequence.fetch_add(1,std::memory_order_acq_rel);
    const int n=juce::jmin(256,buffer.getNumSamples());
    for(int channel=0;channel<2;++channel) {
        const float* data=buffer.getNumChannels()>0?buffer.getReadPointer(juce::jmin(channel,buffer.getNumChannels()-1)):nullptr;
        for(int i=0;i<n;++i) {
            float v=data?data[buffer.getNumSamples()-n+i]:0;
            if(!std::isfinite(v)) v=0;
            scope[static_cast<size_t>((input?0:2)+channel)][static_cast<size_t>(i)].store(v,std::memory_order_relaxed);
        }
    }
    if(!input) { scopeSamples.store(n,std::memory_order_relaxed); scopeSequence.fetch_add(1,std::memory_order_release); }
}
std::optional<BloodyMasterProcessor::ScopeFrame> BloodyMasterProcessor::getScopeFrame() const {
    for(int attempt=0;attempt<3;++attempt) {
        const auto before=scopeSequence.load(std::memory_order_acquire);
        if((before&1u)!=0) continue;
        ScopeFrame frame;frame.samples=scopeSamples.load(std::memory_order_relaxed);
        for(size_t c=0;c<4;++c) for(int i=0;i<frame.samples;++i)
            frame.channels[c][static_cast<size_t>(i)]=scope[c][static_cast<size_t>(i)].load(std::memory_order_relaxed);
        if(before==scopeSequence.load(std::memory_order_acquire)) return frame;
    }
    return {};
}

int BloodyMasterProcessor::getNumPrograms() { return static_cast<int>(bm::factoryPresets().size()); }
const juce::String BloodyMasterProcessor::getProgramName(int index) {
    if(index<0 || index>=getNumPrograms()) return {};
    return bm::factoryPresets()[static_cast<size_t>(index)].name;
}
void BloodyMasterProcessor::setCurrentProgram(int index) {
    if(index<0 || index>=getNumPrograms()) return;
    setDeltaEnabled(false);
    for(const auto& [id,value]:bm::presetParameters(bm::factoryPresets()[static_cast<size_t>(index)].settings))
        if(auto* parameter=apvts.getParameter(id)) parameter->setValueNotifyingHost(parameter->convertTo0to1(value));
    currentProgram.store(index,std::memory_order_relaxed);
}

BloodyMasterProcessor::BloodyMasterProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "BloodyMasterState", bm::createParameterLayout())
{
    (void) bm::factoryPresets();
    const std::array<const char*,49> ids {
        bm::id::hpf, bm::id::lpf, bm::id::bass, bm::id::mid, bm::id::high,
        bm::id::presence, bm::id::foundation, bm::id::p1f, bm::id::p2f, bm::id::p3f,
        bm::id::p1g, bm::id::p2g, bm::id::p3g, bm::id::monomaker, bm::id::width,
        bm::id::pan, bm::id::eqActive, bm::id::bypass,
        bm::id::drive, bm::id::squash, bm::id::recovery, bm::id::link, bm::id::mix,
        bm::id::glue, bm::id::valve, bm::id::clipper, bm::id::turbo, bm::id::trim,
        bm::id::limrel, bm::id::limlink, bm::id::isp, bm::id::oversampling,
        bm::id::defreq,bm::id::deamount,bm::id::lowres,bm::id::hires,bm::id::spikeListen,
        bm::id::mudAmount,bm::id::mudActive,bm::id::ringAmount,bm::id::ringActive,
        bm::id::thd,bm::id::xl,bm::id::thdMode,bm::id::xlMode,bm::id::tapeColor,bm::id::tapeMode,bm::id::enhance,bm::id::profile
    };
    for (size_t i=0; i<ids.size(); ++i)
        dspParameters[i]=apvts.getRawParameterValue(ids[i]);
}

bm::dsp::EngineSettings BloodyMasterProcessor::readDspSettings() const
{
    const auto v=[&](size_t i) { return static_cast<double>(dspParameters[i]->load(std::memory_order_relaxed)); };
    bm::dsp::EngineSettings settings;
    auto& s=settings.tone;
    s.hpf=v(0); s.lpf=v(1)*1000; s.bass=v(2); s.mid=v(3); s.high=v(4);
    s.presence=v(5); s.body=v(6);
    for(size_t i=0;i<3;++i) { s.freq[i]=v(7+i); s.gain[i]=v(10+i); }
    s.monoBass=v(13); s.width=v(14); s.balance=v(15); s.eq=v(16)>.5; s.bypass=v(17)>.5;
    settings.bypass=v(17)>.5;
    settings.oversampling=2 << juce::jlimit(0,2,static_cast<int>(v(31)));
    auto& d=settings.dynamics;
    d.drive=v(18); d.ratio=v(19); d.releaseMs=v(20); d.link=v(21); d.mix=v(22); d.glue=v(23); d.optical=v(24)>.5;
    auto& p=settings.peaks;
    p.clipDb=v(25); p.pushDb=v(26); p.ceilingDb=v(27); p.releaseMs=v(28); p.link=v(29); p.isp=v(30)>.5;
    auto& c=settings.cleanup;c.enabled=true;c.harshHz=v(32)*1000;c.amount=v(33);c.mudHz=v(34);c.ringHz=v(35)*1000;
    c.listen=v(36)>.5;c.mudAmount=v(37);c.mudEnabled=v(38)>.5;c.ringAmount=v(39);c.ringEnabled=v(40)>.5;
    p.color.thdDb=v(41);p.color.xl=v(42);p.color.thdMode=static_cast<int>(v(43));p.color.xlMode=static_cast<int>(v(44));p.color.valve=d.optical;
    p.color.tape=v(45);p.color.tapeMode=static_cast<int>(v(46));
    s.atmosphere=v(47);s.profile=static_cast<int>(v(48));
    return settings;
}

void BloodyMasterProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    setDeltaEnabled(false);
    preparedRate.store(sampleRate,std::memory_order_relaxed);
    const auto settings=readDspSettings();
    mastering.prepare(sampleRate,settings.oversampling);
    finalSafety.prepare(sampleRate);finalOutput={};safetyReduction.store(0);
    matchMeter.prepare(sampleRate);
    outputMeter.prepare(sampleRate);
    monitor.prepare(sampleRate);monitorGainDb.store(0);monitorReady.store(false);
    loudnessValues[0].store(-200);loudnessValues[1].store(-200);loudnessValues[2].store(0);
    meterReset.store(false);
    mastering.update(settings);
    setLatencySamples(mastering.latency()+finalSafety.latency());
    deltaMonitor.prepare(sampleRate,getLatencySamples(),samplesPerBlock);
}

bool BloodyMasterProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto& in  = layouts.getMainInputChannelSet();
    const auto& out = layouts.getMainOutputChannelSet();
    if (in != out) return false;
    return out == juce::AudioChannelSet::mono() || out == juce::AudioChannelSet::stereo();
}

void BloodyMasterProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) {processAudio(buffer,false);}
void BloodyMasterProcessor::processBlockBypassed(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) {processAudio(buffer,true);}
void BloodyMasterProcessor::processAudio(juce::AudioBuffer<float>& buffer,bool hostBypass) {
    juce::ScopedNoDenormals noDenormals;
    const int channels=buffer.getNumChannels();if(channels<1||channels>2)return;
    for(int ch=getTotalNumInputChannels();ch<getTotalNumOutputChannels();++ch)buffer.clear(ch,0,buffer.getNumSamples());
    const bool offline=isNonRealtime();if(offline)setDeltaEnabled(false);
    auto settings=readDspSettings();if(hostBypass)settings.bypass=true;
    // Global Delta supersedes cleanup-only solo without editing its parameter.
    if(!offline&&(isDeltaEnabled()||deltaMonitor.isAuditioning()))settings.cleanup.listen=false;
    mastering.update(settings);
    captureScope(buffer,true);
    bm::dsp::MeterValues sum{};std::array<double,5> outputSum{};double protectionPeak=0;
    for(int offset=0;offset<buffer.getNumSamples();){
        const int frames=std::min(deltaMonitor.maximumFrames(),buffer.getNumSamples()-offset);
        // JUCE uses its inline channel-pointer array for mono/stereo views.
        juce::AudioBuffer<float> chunk(buffer.getArrayOfWritePointers(),channels,offset,frames);
        deltaMonitor.capture(chunk.getArrayOfReadPointers(),channels,frames);
        deltaMonitor.update(isDeltaEnabled(),offline);
        mastering.process(chunk.getArrayOfWritePointers(),channels,frames);
        measureOutput(chunk,settings.bypass,settings.peaks.ceilingDb);
        const auto& m=mastering.meters;
        sum.inputPeak=std::max(sum.inputPeak,m.inputPeak);sum.inputL=std::max(sum.inputL,m.inputL);sum.inputR=std::max(sum.inputR,m.inputR);
        sum.compressorGr=std::max(sum.compressorGr,m.compressorGr);sum.glueGr=std::max(sum.glueGr,m.glueGr);
        sum.clipGr=std::max(sum.clipGr,m.clipGr);sum.limiterGr=std::max(sum.limiterGr,m.limiterGr);
        for(size_t i=0;i<3;++i)outputSum[i]=std::max(outputSum[i],finalOutput[i]);
        for(size_t i=3;i<5;++i)outputSum[i]+=finalOutput[i]*finalOutput[i]*frames;
        protectionPeak=std::max(protectionPeak,safetyReduction.load(std::memory_order_relaxed));
        offset+=frames;
    }
    mastering.meters=sum;finalOutput=outputSum;
    if(buffer.getNumSamples()>0)for(size_t i=3;i<5;++i)finalOutput[i]=std::sqrt(finalOutput[i]/buffer.getNumSamples());
    safetyReduction.store(protectionPeak,std::memory_order_relaxed);
    publishMeters();captureScope(buffer,false);
}

juce::AudioProcessorEditor* BloodyMasterProcessor::createEditor()
{
    return new BloodyMasterEditor(*this);
}

juce::AudioProcessorParameter* BloodyMasterProcessor::getBypassParameter() const
{
    return apvts.getParameter(bm::id::bypass);
}

void BloodyMasterProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml()) {
        xml->setAttribute("factoryProgram",getCurrentProgram());
        copyXmlToBinary(*xml, destData);
    }
}

void BloodyMasterProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    setDeltaEnabled(false);
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
        if (xml->hasTagName(apvts.state.getType())) {
            currentProgram.store(juce::jlimit(0,getNumPrograms()-1,xml->getIntAttribute("factoryProgram",0)),std::memory_order_relaxed);
            apvts.replaceState(juce::ValueTree::fromXml(*xml));
        }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BloodyMasterProcessor();
}

void BloodyMasterProcessor::measureOutput(juce::AudioBuffer<float>& buffer,bool bypass,double ceiling) {
    if(meterReset.exchange(false,std::memory_order_relaxed)){outputMeter.reset();matchMeter.reset();monitor.reset();}
    const int channels=buffer.getNumChannels();
    if(channels<1 || channels>2)return;
    const auto* l=buffer.getReadPointer(0);const auto* r=buffer.getReadPointer(channels==2?1:0);
    for(int i=0;i<buffer.getNumSamples();++i)matchMeter.process(l[i],r[i],channels,false);
    std::array<float,49> signature{};
    for(size_t i=0;i<signature.size();++i)signature[i]=dspParameters[i]->load(std::memory_order_relaxed);
    monitor.select(comparisonIndex.load(std::memory_order_relaxed),signature);
    monitor.process(buffer.getArrayOfWritePointers(),channels,buffer.getNumSamples(),matchMeter.values.shortTerm,
                    monitorEnabled.load(std::memory_order_relaxed)&&!bypass,isNonRealtime()||bypass);
    monitorGainDb.store(monitor.gainDb,std::memory_order_relaxed);monitorReady.store(monitor.ready,std::memory_order_relaxed);
    finalSafety.setCeiling(ceiling);finalOutput={};double safetyGr=0;
    auto* left=buffer.getWritePointer(0);auto* right=buffer.getWritePointer(channels==2?1:0);
    for(int i=0;i<buffer.getNumSamples();++i){
        double a=left[i],b=right[i];finalSafety.process(a,b,channels);
        float heardL=static_cast<float>(a),heardR=static_cast<float>(b);
        deltaMonitor.process(heardL,heardR,i);
        left[i]=heardL;if(channels==2)right[i]=heardR;
        a=heardL;b=heardR;
        outputMeter.process(heardL,heardR,channels);
        safetyGr=std::max(safetyGr,finalSafety.reductionDb);
        finalOutput[1]=std::max(finalOutput[1],std::abs(a));finalOutput[2]=std::max(finalOutput[2],std::abs(b));
        finalOutput[3]+=a*a;finalOutput[4]+=b*b;
    }
    finalOutput[0]=std::max(finalOutput[1],finalOutput[2]);
    if(buffer.getNumSamples()>0){finalOutput[3]=std::sqrt(finalOutput[3]/buffer.getNumSamples());finalOutput[4]=std::sqrt(finalOutput[4]/buffer.getNumSamples());}
    safetyReduction.store(safetyGr,std::memory_order_relaxed);
    loudnessValues[0].store(outputMeter.values.integrated,std::memory_order_relaxed);
    loudnessValues[1].store(outputMeter.values.shortTerm,std::memory_order_relaxed);
    loudnessValues[2].store(outputMeter.values.truePeak,std::memory_order_relaxed);
}
bm::dsp::LoudnessValues BloodyMasterProcessor::getLoudnessValues() const {
    return {loudnessValues[0].load(std::memory_order_relaxed),loudnessValues[1].load(std::memory_order_relaxed),loudnessValues[2].load(std::memory_order_relaxed)};
}
