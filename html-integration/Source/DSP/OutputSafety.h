#pragma once
#include "PeakChain.h"
#include <vector>
namespace bm::dsp {
// Last master stage (before diagnostic Delta). No upward gain.
// Independent 8x peak and 400 ms K-energy guards. Do not confuse transient
// peaks with loudness: peak recovery is fast; only the loudness guard is slow.
class OutputSafety {
    Biquad shelf,highpass;
    std::array<std::array<double,65>,8> taps{};
    std::array<std::array<double,65>,2> history{};
    int pos=0,lookahead=480;
    StereoDelay align,delay;
    SlidingMaximum peakMaximum,loudnessMaximum;
    std::vector<double> powers;
    size_t energyPosition=0,energyCount=0;
    double energySum=0,energyCompensation=0;
    double peakGain=1,loudnessGain=1,attack=0,peakRecovery=0,loudnessRecovery=0,ceiling=.75;
public:
    double reductionDb=0;
    void prepare(double fs) {
        fs=std::isfinite(fs)?std::clamp(fs,8000.,384000.):48000.;
        lookahead=static_cast<int>(std::ceil(fs*.010));align.prepare(32);delay.prepare(lookahead);peakMaximum.prepare(lookahead);loudnessMaximum.prepare(lookahead);
        powers.assign(static_cast<size_t>(std::max(1,static_cast<int>(std::round(fs*.4)))),0);energyPosition=energyCount=0;energySum=energyCompensation=0;
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
        attack=std::exp(-1/(fs*.0002));peakRecovery=std::exp(-1/(fs*.080));loudnessRecovery=std::exp(-1/(fs*2));
        history={};pos=0;peakGain=loudnessGain=1;reductionDb=0;
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
        const double change=power-powers[energyPosition];powers[energyPosition]=power;
        if(++energyPosition==powers.size())energyPosition=0;
        const double corrected=change-energyCompensation,next=energySum+corrected;
        energyCompensation=(next-energySum)-corrected;energySum=next;
        energyCount=std::min(powers.size(),energyCount+1);
        const double mean=std::max(0.,energySum/double(energyCount));
        const double loudnessDemand=std::sqrt(mean/std::pow(10.,(-5.5+.691)/10));
        const auto aligned=align.push(l,r);
        // Retain current unaligned peaks too: earlier gain reduction is safe.
        const double peakTarget=1/peakMaximum.push(std::max(1.,tp/ceiling));
        const double loudnessTarget=1/loudnessMaximum.push(std::max(1.,loudnessDemand));
        const auto audio=delay.push(aligned[0],aligned[1]);
        peakGain=peakTarget+(peakGain-peakTarget)*(peakTarget<peakGain?attack:peakRecovery);
        loudnessGain=loudnessTarget+(loudnessGain-loudnessTarget)*(loudnessTarget<loudnessGain?attack:loudnessRecovery);
        const double safe=std::min({peakGain,loudnessGain,ceiling/std::max({1.e-30,std::abs(audio[0]),std::abs(audio[1])})});
        l=audio[0]*safe;r=audio[1]*safe;reductionDb=-gainDb(std::max(safe,1.e-30));
    }
};
}
