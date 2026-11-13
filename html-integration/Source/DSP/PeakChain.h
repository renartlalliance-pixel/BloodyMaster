#pragma once
#include "Dynamics.h"
#include "Coloration.h"
#include <vector>
#include <cstdint>

namespace bm::dsp {
// Linear-phase, Blackman-Harris windowed sinc; cutoff 0.46 * native Fs.
// 128 native-sample span; each FIR has 64 native samples of group delay.
class Interpolator {
    int factor=8,pos=0;
    std::vector<double> taps;
    std::array<std::array<double,129>,2> history{};
public:
    static constexpr int span=128;
    void prepare(int f) {
        factor=f; pos=0; history={}; const int n=span*factor;
        taps.resize(n+1); double sum=0;
        for(int i=0;i<=n;++i) {
            const double x=double(i-n/2),w=2*pi*i/n,fc=.46/factor;
            const double win=.35875-.48829*std::cos(w)+.14128*std::cos(2*w)-.01168*std::cos(3*w);
            taps[i]=(x==0?2*fc:std::sin(2*pi*fc*x)/(pi*x))*win; sum+=taps[i];
        }
        for(auto& t:taps) t/=sum;
    }
    const std::vector<double>& coefficients() const { return taps; }
    void push(double l,double r) { pos=(pos+1)%129; history[0][pos]=l; history[1][pos]=r; }
    double phase(int c,int phase) const {
        double y=0; int j=pos;
        for(int k=phase;k<static_cast<int>(taps.size());k+=factor) { y+=taps[k]*history[c][j]; if(--j<0) j=128; }
        return y*factor;
    }
};
class Decimator {
    std::vector<double> h;
    std::array<std::vector<double>,2> ring;
    int pos=0;
public:
    void prepare(const std::vector<double>& coefficients) { h=coefficients; for(auto& r:ring) r.assign(h.size(),0); pos=0; }
    void push(double l,double r) { if(++pos==static_cast<int>(h.size())) pos=0; ring[0][pos]=l; ring[1][pos]=r; }
    double output(int c) const {
        double y=0; int j=pos;
        for(size_t k=0;k<h.size();++k) { y+=h[k]*ring[c][j]; if(--j<0) j=static_cast<int>(h.size())-1; }
        return y;
    }
};
class StereoDelay {
    std::vector<std::array<double,2>> ring;
    size_t pos=0;
public:
    void prepare(int samples) { ring.assign(static_cast<size_t>(std::max(1,samples)),{}); pos=0; }
    std::array<double,2> push(double l,double r) {
        auto old=ring[pos]; ring[pos]={l,r}; if(++pos==ring.size()) pos=0; return old;
    }
};
class SlidingMaximum {
    struct Entry { uint64_t time=0; double value=0; };
    std::vector<Entry> queue;
    size_t first=0,tail=0; uint64_t clock=0,window=1;
public:
    void prepare(int samples) { window=static_cast<uint64_t>(samples); queue.assign(samples+3,{}); first=tail=0; clock=0; }
    double push(double value) {
        while(first!=tail && queue[first].time+window<clock) first=(first+1)%queue.size();
        while(first!=tail) {
            size_t prev=(tail+queue.size()-1)%queue.size();
            if(queue[prev].value>value) break;
            tail=prev;
        }
        queue[tail]={clock++,value}; tail=(tail+1)%queue.size();
        return queue[first].value;
    }
};
inline double softClip(double x) {
    const double a=std::abs(x);
    if(a<=.75) return x;
    if(a>=1.25) return std::copysign(1.,x);
    const double u=(a-.75)/.5;
    return std::copysign(.75+.5*(u-.5*u*u),x);
}
struct PeakSettings {
    ColorSettings color{};
    double clipDb=0,pushDb=0,ceilingDb=-1,releaseMs=100,link=100,loudnessWarDb=0;
    bool isp=true,clipEnabled=true,limiterEnabled=true;
    bool operator==(const PeakSettings&) const = default;
};
class PeakChain {
    double fs=48000; int factor=8,lookahead=96; bool initial=true;
    PeakSettings last{};
    Interpolator up,analysis;
    Decimator down;
    Coloration color;
    StereoDelay align,delay;
    std::array<SlidingMaximum,2> max;
    std::array<double,2> gain{1,1},memory{};
    Ramp clipDrive,clipMix,push,ceiling,release,link,isp,limiterMix,loudnessWar;
public:
    double clipGr=0,limiterGr=0;
    void prepare(double rate,int oversampling=8) {
        fs=rate; factor=oversampling==2?2:oversampling==4?4:8;
        lookahead=static_cast<int>(std::ceil(fs*.003));
        up.prepare(factor); down.prepare(up.coefficients()); analysis.prepare(8);
        color.prepare(fs*factor);
        align.prepare(64); delay.prepare(lookahead);
        for(auto& m:max) m.prepare(lookahead);
        gain={1,1}; memory={}; initial=true; clipGr=limiterGr=0;
    }
    int latency() const { return 192+lookahead; }
    void update(const PeakSettings& s) {
        if(!initial && s==last) return;
        last=s;
        color.update(s.color);
        int n=static_cast<int>(fs*.02);
        clipDrive.set(dbGain(std::clamp(s.clipDb,0.,10.)),n,initial);
        clipMix.set(s.clipEnabled?std::clamp(s.clipDb/.1,0.,1.):0.,n,initial);
        loudnessWar.set(dbGain(std::clamp(s.loudnessWarDb,0.,18.)),n,initial);
        push.set(dbGain(std::clamp(s.pushDb,0.,6.)),n,initial);
        ceiling.set(dbGain(std::clamp(s.ceilingDb,-6.,0.)),n,initial);
        release.set(std::clamp(s.releaseMs,1.,500.),n,initial);
        link.set(std::clamp(s.link/100,0.,1.),n,initial);
        isp.set(s.isp?1:0,n,initial);
        limiterMix.set(s.limiterEnabled?1:0,n,initial); initial=false;
    }
    void process(double& l,double& r) {
        const double drive=clipDrive.next(), cm=clipMix.next(), p=push.next(), lift=loudnessWar.next();
        up.push(l,r); std::array<double,2> native{}; double reduction=0;
        for(int phase=0;phase<factor;++phase) {
            std::array<double,2> x{up.phase(0,phase),up.phase(1,phase)};
            color.process(x[0],x[1]);
            for(auto& v:x) {
                v*=lift; // post-colour gain into oversampled clipping / limiting
                const double clipped=softClip(v*drive)/drive;
                const double y=v+cm*(clipped-v);
                if(std::abs(v)>1.e-9) reduction=std::max(reduction,gainDb(std::abs(v)/std::max(std::abs(y),1.e-12)));
                v=y*p;
            }
            down.push(x[0],x[1]);
            if(phase==0) native={down.output(0),down.output(1)};
        }
        clipGr=reduction;
        // Causal 8x peak estimation after downsampling; align audio with its 64-sample delay.
        analysis.push(native[0],native[1]);
        auto audio=align.push(native[0],native[1]);
        const double protection=isp.next(), ceil=ceiling.next(), coupling=link.next(), wet=limiterMix.next();
        const double threshold=ceil*dbGain(-.35*protection); // measured headroom for reconstruction / gain modulation
        std::array<double,2> peaks{};
        for(int c=0;c<2;++c) {
            double tp=std::abs(audio[c]);
            for(int phase=0;phase<8;++phase) tp=std::max(tp,std::abs(analysis.phase(c,phase)));
            peaks[c]=max[c].push(std::abs(audio[c])+protection*(tp-std::abs(audio[c])));
        }
        const auto delayed=delay.push(audio[0],audio[1]);
        const double shared=std::max(peaks[0],peaks[1]);
        const double releaseSeconds=std::max(.008,release.next()*.001);
        const double attack=std::exp(-1/(fs*.0002));
        std::array<double,2> out{}; limiterGr=0;
        for(int c=0;c<2;++c) {
            const double detected=peaks[c]+coupling*(shared-peaks[c]);
            const double demand=std::min(1.,threshold/std::max(1.e-12,detected));
            memory[c]+=(1-demand-memory[c])*(1-std::exp(-1/(fs*.12)));
            const double recovery=std::exp(-1/(fs*releaseSeconds*(1+3*memory[c])));
            gain[c]=demand+(gain[c]-demand)*(demand<gain[c]?attack:recovery);
            // Exact sample-peak safety on the delayed sample (normally below the smoothed envelope).
            const double safe=std::min(gain[c],threshold/std::max(1.e-12,std::abs(delayed[c])));
            const double applied=1+wet*(safe-1);
            out[c]=delayed[c]*applied;
            limiterGr=std::max(limiterGr,-gainDb(applied));
        }
        l=out[0]; r=out[1];
    }
};

// Allocate every mode during prepare. Only the active mode runs in steady state.
// A requested mode first fills its causal filters/delays, then fades in at the
// same latency. Requests received during a transition are coalesced to the last.
class SwitchablePeakChain {
    std::array<PeakChain,3> modes;
    int active=2, incoming=-1, requested=2, warmRemaining=0;
    int warmSamples=0, fadeSamples=0, fadePosition=0;
    static int index(int factor) { return factor==2?0:factor==4?1:2; }
public:
    double clipGr=0, limiterGr=0;
    void prepare(double rate,int factor=8) {
        for(int i=0;i<3;++i) modes[i].prepare(rate,2<<i);
        active=requested=index(factor); incoming=-1;
        warmSamples=modes[active].latency()+Interpolator::span;
        fadeSamples=std::max(1,static_cast<int>(rate*.02));
        warmRemaining=fadePosition=0; clipGr=limiterGr=0;
    }
    int latency() const { return modes[active].latency(); }
    void select(int factor) { requested=index(factor); }
    void update(const PeakSettings& s) { for(auto& mode:modes) mode.update(s); }
    void process(double& l,double& r) {
        if(incoming<0 && requested!=active) {
            incoming=requested; warmRemaining=warmSamples; fadePosition=0;
        }
        double nextL=l,nextR=r;
        modes[active].process(l,r);
        clipGr=modes[active].clipGr; limiterGr=modes[active].limiterGr;
        if(incoming<0) return;
        modes[incoming].process(nextL,nextR);
        if(warmRemaining>0) { --warmRemaining; return; }
        const double mix=double(++fadePosition)/fadeSamples;
        l+=mix*(nextL-l); r+=mix*(nextR-r);
        clipGr=std::max(clipGr,modes[incoming].clipGr);
        limiterGr=std::max(limiterGr,modes[incoming].limiterGr);
        if(fadePosition==fadeSamples) { active=incoming; incoming=-1; }
    }
};
}
