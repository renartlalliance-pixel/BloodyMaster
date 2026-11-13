#pragma once
#include "MasteringCore.h"

namespace bm::dsp {
// Original tape-inspired colour, not a magnetic/hysteresis circuit model.
// Soft saturation plus level-dependent HF rounding; no noise or modulation.
// Run at the existing oversampled rate before the clipper and limiter.
class TapeColor {
    double fs=384000,attack=0,release=0,hfCoefficient=0,envelope=0;
    std::array<double,2> softened{};
    Ramp mix,character;
    bool initial=true;
public:
    void prepare(double rate) {
        fs=rate;envelope=0;softened={};initial=true;
        attack=std::exp(-1/(fs*.0007));release=std::exp(-1/(fs*.08));
        hfCoefficient=std::exp(-2*pi*6000/fs);
    }
    void update(double amount,int mode) {
        const int n=std::max(1,static_cast<int>(fs*.02));
        mix.set(std::clamp(amount/100,0.,1.),n,initial);
        character.set(mode==1?1.:0.,n,initial);initial=false;
    }
    void process(double& l,double& r) {
        const double amount=mix.next(),mode=character.next();
        const double input=std::max(std::abs(l),std::abs(r));
        envelope=input+(envelope-input)*(input>envelope?attack:release);
        const double drive=1.1+1.9*mode;
        const double rounding=(.04+.14*mode)*envelope/(envelope+.5);
        const double blend=amount*(.5+.35*mode);
        std::array<double,2> samples{l,r};
        for(size_t c=0;c<2;++c) {
            const double x=samples[c];
            const double saturated=amount>0?std::tanh(drive*x)/drive:x;
            softened[c]=saturated+(softened[c]-saturated)*hfCoefficient;
            if(std::abs(softened[c])<1.e-30)softened[c]=0;
            const double coloured=saturated+rounding*(softened[c]-saturated);
            samples[c]=x+blend*(coloured-x);
        }
        l=samples[0];r=samples[1];
    }
};
}
