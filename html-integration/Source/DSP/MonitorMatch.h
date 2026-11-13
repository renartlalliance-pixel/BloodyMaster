#pragma once
#include "MasteringCore.h"
namespace bm::dsp {
// Transient A/B audition: learn each uncorrected 3 s short-term loudness,
// attenuate the louder snapshot. No upward gain or added latency.
class MonitorMatch {
    std::array<std::array<float,49>,2> signatures{};
    std::array<double,2> loudness{-200,-200};
    std::array<bool,2> known{};
    int selected=-1,age=0,learningSamples=144000,smoothing=4800;
    Ramp gain;
public:
    double gainDb=0;
    bool ready=false;
    void prepare(double fs){learningSamples=static_cast<int>(fs*3);smoothing=static_cast<int>(fs*.1);reset();}
    void reset(){loudness={-200,-200};known={};selected=-1;age=0;gain.set(1,1,true);gainDb=0;ready=false;}
    void select(int index,const std::array<float,49>& signature){
        index=std::clamp(index,0,1);
        if(selected!=index){selected=index;age=0;}
        const auto slot=static_cast<size_t>(index);
        if(!known[slot]||signatures[slot]!=signature){signatures[slot]=signature;known[slot]=true;loudness[slot]=-200;age=0;}
    }
    void process(float* const* audio,int channels,int frames,double shortTerm,bool enabled,bool offline){
        if(selected<0||channels<1||channels>2)return;
        age=std::min(learningSamples,age+frames);
        const auto slot=static_cast<size_t>(selected),other=1-slot;
        if(age>=learningSamples)loudness[slot]=shortTerm;
        ready=loudness[slot]>-70&&loudness[other]>-70;
        gainDb=enabled&&!offline&&ready?std::clamp(loudness[other]-loudness[slot],-12.,0.):0.;
        gain.set(std::pow(10.,gainDb/20),smoothing,offline);
        for(int i=0;i<frames;++i){const double g=gain.next();for(int c=0;c<channels;++c)audio[c][i]=static_cast<float>(audio[c][i]*g);}
    }
};
}
