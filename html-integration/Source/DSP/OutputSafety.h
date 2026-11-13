#pragma once
#include "PeakChain.h"
#include <vector>
namespace bm::dsp {
// Last stage, including internal/host bypass and delta audition. No upward gain.
// Full-band 8x peak estimate + 10 ms lookahead, K-weighted fast energy guard.
// -5 LUFS is a programme constraint, not an instantaneous sample measurement;
// a conservative -6 LUFS energy threshold leaves transient/measurement margin.
class OutputSafety {
    Biquad shelf,highpass;
    std::array<std::array<double,65>,8> taps{};
    std::array<std::array<double,65>,2> history{};
    int pos=0,lookahead=480;
    StereoDelay align,delay;
    SlidingMaximum demandMaximum;
    double energy=0,gain=1,attack=0,recovery=0,energyAttack=0,energyRelease=0,ceiling=.75;
public:
    double reductionDb=0;
    void prepare(double fs) {
        fs=std::isfinite(fs)?std::clamp(fs,8000.,384000.):48000.;
        lookahead=static_cast<int>(std::ceil(fs*.010));align.prepare(32);delay.prepare(lookahead);demandMaximum.prepare(lookahead);
        const double k=std::tan(pi*1681.974450955533/fs),vh=std::pow(10.,3.999843853973347/20),vb=std::pow(vh,.4996667741545416),q=.7071752369554196,a=1+k/q+k*k;
        shelf.reset();shelf.set({(vh+vb*k/q+k*k)/a,2*(k*k-vh)/a,(vh-vb*k/q+k*k)/a,2*(k*k-1)/a,(1-k/q+k*k)/a},1,true);
        const double h=std::tan(pi*38.13547087602444/fs),hq=.5003270373238773,ha=1+h/hq+h*h;
        highpass.reset();highpass.set({1,-2,1,2*(h*h-1)/ha,(1-h/hq+h*h)/ha},1,true);
        for(size_t phase=0;phase<8;++phase){double sum=0;
            for(size_t i=0;i<65;++i){const double x=double(i)-32+double(phase)/8;
                const double w=std::abs(x)<=32?.42+.5*std::cos(pi*x/32)+.08*std::cos(2*pi*x/32):0;
                taps[phase][i]=(x==0?1:std::sin(pi*x)/(pi*x))*w;sum+=taps[phase][i];}
            for(auto& t:taps[phase])t/=sum;
        }
        attack=std::exp(-1/(fs*.0002));recovery=std::exp(-1/(fs*2));
        energyAttack=std::exp(-1/(fs*.001));energyRelease=std::exp(-1/(fs*.4));
        history={};pos=0;energy=0;gain=1;reductionDb=0;
    }
    int latency() const{return 32+lookahead;}
    void setCeiling(double db){ceiling=dbGain(std::min(-2.,std::isfinite(db)?db:-2.)-.5);}
    void process(double& l,double& r,int channels) {
        if(!std::isfinite(l))l=0;
        if(!std::isfinite(r))r=0;
        l=std::clamp(l,-1.e6,1.e6);r=std::clamp(r,-1.e6,1.e6);
        shelf.tick();highpass.tick();double power=0,tp=std::max(std::abs(l),std::abs(r));
        const std::array<double,2> input{l,r};
        for(int c=0;c<channels;++c){const auto ch=static_cast<size_t>(c);const double x=highpass.process(shelf.process(input[ch],c),c);power+=x*x;
            history[ch][static_cast<size_t>(pos)]=input[ch];
            for(const auto& phase:taps){double y=0;int index=pos;for(size_t i=0;i<65;++i){y+=phase[i]*history[ch][static_cast<size_t>(index)];if(--index<0)index=64;}tp=std::max(tp,std::abs(y));}
        }
        if(++pos==65)pos=0;
        energy=power+(energy-power)*(power>energy?energyAttack:energyRelease);
        const double loudnessDemand=std::sqrt(energy/std::pow(10.,(-6.+.691)/10));
        const auto aligned=align.push(l,r);
        // Retain current unaligned peaks too: earlier gain reduction is safe.
        const double demand=demandMaximum.push(std::max({1.,tp/ceiling,loudnessDemand}));
        const auto audio=delay.push(aligned[0],aligned[1]);
        const double target=1/demand;
        gain=target+(gain-target)*(target<gain?attack:recovery);
        const double safe=std::min(gain,ceiling/std::max({1.e-30,std::abs(audio[0]),std::abs(audio[1])}));
        l=audio[0]*safe;r=audio[1]*safe;reductionDb=-gainDb(std::max(safe,1.e-30));
    }
};
}
