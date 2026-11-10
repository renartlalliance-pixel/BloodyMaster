#pragma once
#include <algorithm>
#include <array>
#include <cmath>

namespace bm::dsp {
constexpr double pi = 3.14159265358979323846;
struct Coefficients {
    double b0=1, b1=0, b2=0, a1=0, a2=0;
    bool operator==(const Coefficients&) const = default;
};
enum class Type { highpass, lowpass, peak, lowShelf, highShelf };
// RBJ biquads, normalized a0. Reference: https://www.w3.org/TR/audio-eq-cookbook/
inline Coefficients design(Type type, double fs, double hz, double gain=0, double q=0.7071067811865476) {
    if ((type == Type::highpass && hz <= 0) || (type == Type::lowpass && hz >= fs*.499)) return {};
    hz=std::clamp(hz, 5.0, fs*.45); q=std::clamp(q,.1,10.0);
    const double w=2*pi*hz/fs, c=std::cos(w), s=std::sin(w), alpha=s/(2*q);
    const double A=std::pow(10.0,gain/40), r=s*std::sqrt(2*A);
    double b0,b1,b2,a0,a1,a2;
    switch(type) {
    case Type::highpass: b0=(1+c)/2; b1=-(1+c); b2=b0; a0=1+alpha; a1=-2*c; a2=1-alpha; break;
    case Type::lowpass: b0=(1-c)/2; b1=1-c; b2=b0; a0=1+alpha; a1=-2*c; a2=1-alpha; break;
    case Type::peak: b0=1+alpha*A; b1=-2*c; b2=1-alpha*A; a0=1+alpha/A; a1=-2*c; a2=1-alpha/A; break;
    case Type::lowShelf:
        b0=A*((A+1)-(A-1)*c+r); b1=2*A*((A-1)-(A+1)*c); b2=A*((A+1)-(A-1)*c-r);
        a0=(A+1)+(A-1)*c+r; a1=-2*((A-1)+(A+1)*c); a2=(A+1)+(A-1)*c-r; break;
    default:
        b0=A*((A+1)+(A-1)*c+r); b1=-2*A*((A-1)+(A+1)*c); b2=A*((A+1)+(A-1)*c-r);
        a0=(A+1)-(A-1)*c+r; a1=2*((A-1)-(A+1)*c); a2=(A+1)-(A-1)*c-r; break;
    }
    return {b0/a0,b1/a0,b2/a0,a1/a0,a2/a0};
}
// Original broad passive-style low curve. Coupled boost/cut, not a circuit model.
// Positive knob: +1.45*g shelf at 60 Hz and -0.45*g shelf at 240 Hz.
// DC gain remains g; the offset corners produce a gentle low-mid dip.
// Negative knob: broad low-frequency attenuation only.
inline std::array<Coefficients,2> passiveLow(double fs,double gain) {
    return {design(Type::lowShelf,fs,60,gain>0?1.45*gain:gain),
            design(Type::lowShelf,fs,240,gain>0?-.45*gain:0)};
}
class Ramp {
    double value=0, target=0, step=0; int remaining=0;
public:
    void set(double v, int samples, bool snap=false) {
        if(snap) { value=target=v; remaining=0; return; }
        if(v==target) return;
        target=v; remaining=std::max(1,samples); step=(target-value)/remaining;
    }
    double next() { if(remaining>0) { value+=step; if(--remaining==0) value=target; } return value; }
};
class Biquad {
    struct State { double x1=0,x2=0,y1=0,y2=0; };
    std::array<State,2> state{};
    std::array<Ramp,5> ramps{};
    Coefficients current{}, target{};
public:
    void reset() { state={}; }
    void set(Coefficients c, int samples, bool snap) {
        if(!snap && c==target) return;
        target=c;
        const std::array<double,5> a{c.b0,c.b1,c.b2,c.a1,c.a2};
        for(int i=0;i<5;++i) ramps[i].set(a[i],samples,snap);
    }
    void tick() { current={ramps[0].next(),ramps[1].next(),ramps[2].next(),ramps[3].next(),ramps[4].next()}; }
    double process(double x, int channel) {
        auto& z=state[channel];
        const double y=current.b0*x+current.b1*z.x1+current.b2*z.x2-current.a1*z.y1-current.a2*z.y2;
        z.x2=z.x1; z.x1=x; z.y2=z.y1; z.y1=std::abs(y)<1.e-30 ? 0.0 : y;
        return y;
    }
};
struct Settings {
    double hpf=0, lpf=45000;
    double bass=0, mid=0, high=0, presence=0, body=0;
    std::array<double,3> freq{120,1800,12000}, gain{0,0,0};
    double monoBass=0, width=100, balance=0;
    bool eq=true, bypass=false;
    bool operator==(const Settings&) const = default;
};
class MasteringCore {
    double sampleRate=44100;
    int smoothing=882;
    bool initial=true;
    Settings lastSettings{};
    std::array<Biquad,11> filters{};
    Biquad sideFilter;
    Ramp width, balance, wet;
public:
    void prepare(double fs) {
        sampleRate=std::isfinite(fs) && fs>=8000 ? fs : 44100;
        smoothing=static_cast<int>(sampleRate*.02);
        for(auto& f:filters) f.reset();
        sideFilter.reset(); initial=true;
    }
    void update(const Settings& s) {
        if(!initial && s==lastSettings) return;
        lastSettings=s;
        const auto set=[&](int i,Type t,double hz,double gain=0,double q=.7071067811865476) {
            filters[i].set(design(t,sampleRate,hz,gain,q),smoothing,initial);
        };
        set(0,Type::highpass,s.hpf); set(1,Type::lowpass,s.lpf);
        const auto low=passiveLow(sampleRate,s.bass);
        filters[2].set(low[0],smoothing,initial); filters[10].set(low[1],smoothing,initial);
        set(3,Type::peak,650,s.mid,.7);
        set(4,Type::peak,8000,s.high,.55); set(5,Type::highShelf,12000,s.presence);
        set(6,Type::lowShelf,160,s.body*.3); // +/-10 UI units -> +/-3 dB broad shelf
        for(int i=0;i<3;++i) set(7+i,Type::peak,s.freq[i],s.eq?s.gain[i]:0,1.0);
        sideFilter.set(design(Type::highpass,sampleRate,s.monoBass),smoothing,initial);
        width.set(std::clamp(s.width/100,0.0,2.0),smoothing,initial);
        balance.set(std::clamp(s.balance/50,-1.0,1.0),smoothing,initial);
        wet.set(s.bypass?0:1,static_cast<int>(sampleRate*.005),initial);
        initial=false;
    }
    // No allocation, locks, UI calls or parameter lookups. Exactly 1 or 2 channels.
    void process(float* const* channels,int count,int samples) {
        if(count<1 || count>2) return;
        for(int n=0;n<samples;++n) {
            const double dryL=channels[0][n], dryR=count==2?channels[1][n]:dryL;
            double l=dryL,r=dryR;
            for(auto& f:filters) { f.tick(); l=f.process(l,0); if(count==2) r=f.process(r,1); }
            sideFilter.tick();
            const double w=width.next(), p=balance.next(), mix=wet.next();
            if(count==2) {
                const double m=(l+r)*.5, side=sideFilter.process((l-r)*.5,0)*w;
                l=(m+side)*(p>0?1-p:1); r=(m-side)*(p<0?1+p:1);
            }
            // Exact dry after the bypass fade; filter states keep following the input.
            channels[0][n]=mix==0?static_cast<float>(dryL):static_cast<float>(dryL+(l-dryL)*mix);
            if(count==2) channels[1][n]=mix==0?static_cast<float>(dryR):static_cast<float>(dryR+(r-dryR)*mix);
        }
    }
};
} // namespace bm::dsp
