#pragma once
#include "PeakChain.h"
namespace bm::dsp {
// Transient audition only. Dry input is captured before any processing and
// continuously delayed, even while audition is off. Buffers allocated in prepare.
class DeltaMonitor {
    StereoDelay reference;
    std::array<std::vector<float>,2> aligned;
    Ramp blend;
    int fadeSamples=960,capacity=1,delaySamples=0;
    double currentBlend=0;
public:
    void prepare(double fs,int latency,int maxFrames) {
        fs=std::isfinite(fs)?std::clamp(fs,8000.,384000.):48000.;
        currentBlend=0;
        capacity=std::clamp(maxFrames,1,65536);
        fadeSamples=std::max(1,static_cast<int>(fs*.020));
        delaySamples=std::max(0,latency);reference.prepare(delaySamples);
        for(auto& channel:aligned)channel.assign(static_cast<size_t>(capacity),0);
        blend.set(0,1,true);
    }
    bool isAuditioning() const{return currentBlend>0;}
    int maximumFrames() const{return capacity;}
    void capture(const float* const* input,int channels,int frames) {
        for(int i=0;i<frames;++i){
            double l=input[0][i],r=channels==2?input[1][i]:l;
            if(!std::isfinite(l))l=0;
            if(!std::isfinite(r))r=0;
            const auto dry=delaySamples?reference.push(l,r):std::array<double,2>{l,r};
            aligned[0][static_cast<size_t>(i)]=static_cast<float>(dry[0]);
            aligned[1][static_cast<size_t>(i)]=static_cast<float>(dry[1]);
        }
    }
    void update(bool enabled,bool offline=false){blend.set(enabled&&!offline?1.:0.,fadeSamples,offline);}
    void process(float& l,float& r,int frame) {
        const double amount=blend.next();currentBlend=amount;
        if(amount==0)return; // exact normal output after fade / in offline renders
        const double deltaL=double(aligned[0][static_cast<size_t>(frame)])-l;
        const double deltaR=double(aligned[1][static_cast<size_t>(frame)])-r;
        if(amount==1){l=static_cast<float>(deltaL);r=static_cast<float>(deltaR);}
        else {l=static_cast<float>(l+amount*(deltaL-l));r=static_cast<float>(r+amount*(deltaR-r));}
    }
};
}
