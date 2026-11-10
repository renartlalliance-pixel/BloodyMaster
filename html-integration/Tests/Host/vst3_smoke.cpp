// Independent minimal Windows VST3 host: load factory, instantiate, process stereo.
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <algorithm>
#include <cstring>
#include <cmath>
#include <cstdio>
#include "pluginterfaces/base/ipluginbase.h"
#include "pluginterfaces/vst/ivstcomponent.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"
#include "pluginterfaces/vst/vstspeaker.h"
using namespace Steinberg;
using namespace Steinberg::Vst;
int wmain(int argc,wchar_t** argv) {
    if(argc!=2) return 2;
    const auto dll=LoadLibraryW(argv[1]);if(!dll) {std::printf("LoadLibrary failed %lu\n",GetLastError());return 3;}
    using Entry=bool (*)();
    if(auto init=reinterpret_cast<Entry>(GetProcAddress(dll,"InitDll"))) if(!init()) return 4;
    using Factory=IPluginFactory* (*)();
    const auto entry=reinterpret_cast<Factory>(GetProcAddress(dll,"GetPluginFactory"));if(!entry) return 5;
    auto* factory=entry();if(!factory || factory->countClasses()<1) return 6;
    IComponent* component=nullptr;
    for(int i=0;i<factory->countClasses();++i) {
        PClassInfo info{};if(factory->getClassInfo(i,&info)!=kResultOk) return 7;
        std::printf("Class: %s / %s\n",info.name,info.category);
        if(std::strcmp(info.category,kVstAudioEffectClass)==0)
            if(factory->createInstance(info.cid,IComponent_iid,reinterpret_cast<void**>(&component))!=kResultOk) return 8;
    }
    if(!component || component->initialize(nullptr)!=kResultOk) return 9;
    IAudioProcessor* processor=nullptr;
    if(component->queryInterface(IAudioProcessor_iid,reinterpret_cast<void**>(&processor))!=kResultOk) return 10;
    SpeakerArrangement stereo=SpeakerArr::kStereo;
    if(processor->setBusArrangements(&stereo,1,&stereo,1)!=kResultOk) return 11;
    ProcessSetup setup{};setup.processMode=kRealtime;setup.symbolicSampleSize=kSample32;setup.maxSamplesPerBlock=512;setup.sampleRate=48000;
    if(processor->setupProcessing(setup)!=kResultOk) return 12;
    component->activateBus(kAudio,kInput,0,true);component->activateBus(kAudio,kOutput,0,true);
    if(component->setActive(true)!=kResultOk || processor->setProcessing(true)!=kResultOk) return 13;
    if(processor->getLatencySamples()!=336) return 14;
    float in[2][512]{},out[2][512]{};float* ip[]{in[0],in[1]};float* op[]{out[0],out[1]};
    AudioBusBuffers input{},output{};input.numChannels=output.numChannels=2;input.channelBuffers32=ip;output.channelBuffers32=op;
    double maximum=0;
    for(int block=0;block<16;++block) {
        for(int i=0;i<512;++i) {in[0][i]=float(.3*std::sin(6.283185307179586*997*(block*512+i)/48000));in[1][i]=in[0][i]*.7f;}
        ProcessData data{};data.processMode=kRealtime;data.symbolicSampleSize=kSample32;data.numSamples=512;
        data.numInputs=data.numOutputs=1;data.inputs=&input;data.outputs=&output;
        if(processor->process(data)!=kResultOk) return 15;
        for(auto& channel:out) for(float v:channel) {
            if(!std::isfinite(v) || std::abs(v)>1) return 16;
            maximum=std::max(maximum,double(std::abs(v)));
        }
    }
    if(maximum<.01) return 17;
    processor->setProcessing(false);component->setActive(false);component->terminate();processor->release();component->release();factory->release();
    if(auto exit=reinterpret_cast<Entry>(GetProcAddress(dll,"ExitDll"))) exit();
    FreeLibrary(dll);std::printf("PASS VST3 factory, instance, stereo processing, latency 336, peak %.6f\n",maximum);return 0;
}
