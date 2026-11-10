#pragma once
#include "PeakChain.h"
namespace bm::dsp {
struct EngineSettings {
    Settings tone{};
    DynamicsSettings dynamics{};
    PeakSettings peaks{};
    bool bypass=false;
    int oversampling=0; // 0 keeps the mode selected by prepare.
};
struct MeterValues {
    double inputPeak=0,outputPeak=0,compressorGr=0,glueGr=0,clipGr=0,limiterGr=0;
    double inputL=0,inputR=0,outputL=0,outputR=0,rmsL=0,rmsR=0;
};
class Engine {
    MasteringCore tone;
    Dynamics dynamics;
    SwitchablePeakChain peaks;
    StereoDelay dry;
    Ramp wet;
    double fs=48000;
    bool initial=true;
public:
    MeterValues meters{};
    void prepare(double sampleRate,int oversampling=8) {
        fs=std::clamp(sampleRate,8000.,384000.);
        tone.prepare(fs); dynamics.prepare(fs); peaks.prepare(fs,oversampling);
        dry.prepare(peaks.latency()); initial=true; meters={};
    }
    int latency() const { return peaks.latency(); }
    void update(EngineSettings s) {
        s.tone.bypass=false; tone.update(s.tone); dynamics.update(s.dynamics); peaks.update(s.peaks);
        if(s.oversampling==2 || s.oversampling==4 || s.oversampling==8) peaks.select(s.oversampling);
        wet.set(s.bypass?0:1,static_cast<int>(fs*.005),initial); initial=false;
    }
    void process(float* const* data,int channels,int frames) {
        if(channels<1 || channels>2) return;
        meters={};
        for(int i=0;i<frames;++i) {
            double l=data[0][i],r=channels==2?data[1][i]:l;
            if(!std::isfinite(l)) l=0;
            if(!std::isfinite(r)) r=0;
            l=std::clamp(l,-1.e6,1.e6); r=std::clamp(r,-1.e6,1.e6);
            const auto bypass=dry.push(l,r);
            meters.inputPeak=std::max({meters.inputPeak,std::abs(l),std::abs(r)});
            meters.inputL=std::max(meters.inputL,std::abs(l));
            meters.inputR=std::max(meters.inputR,std::abs(r));
            const double drive=dynamics.inputGain();
            float left=static_cast<float>(l*drive),right=static_cast<float>(r*drive);
            float* frame[]{&left,&right}; tone.process(frame,channels,1);
            l=left; r=channels==2?right:left;
            dynamics.process(l,r); peaks.process(l,r);
            double w=wet.next(); l=bypass[0]+w*(l-bypass[0]); r=bypass[1]+w*(r-bypass[1]);
            data[0][i]=static_cast<float>(l); if(channels==2) data[1][i]=static_cast<float>(r);
            meters.outputPeak=std::max({meters.outputPeak,std::abs(l),std::abs(r)});
            meters.outputL=std::max(meters.outputL,std::abs(l));
            meters.outputR=std::max(meters.outputR,std::abs(r));
            meters.rmsL+=l*l; meters.rmsR+=r*r;
            meters.compressorGr=std::max(meters.compressorGr,dynamics.compressorGr);
            meters.glueGr=std::max(meters.glueGr,dynamics.glueGr);
            meters.clipGr=std::max(meters.clipGr,peaks.clipGr);
            meters.limiterGr=std::max(meters.limiterGr,peaks.limiterGr);
        }
        if(frames>0) { meters.rmsL=std::sqrt(meters.rmsL/frames); meters.rmsR=std::sqrt(meters.rmsR/frames); }
    }
};
}
