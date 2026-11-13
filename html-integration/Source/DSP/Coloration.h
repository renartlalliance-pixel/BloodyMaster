#pragma once
#include "MasteringCore.h"
#include "TapeColor.h"

namespace bm::dsp {
struct ColorSettings {
    double thdDb=-60,xl=0;
    int thdMode=0,xlMode=0; // warm/dense; density/presence/impact
    bool valve=false;
    double tape=0;
    int tapeMode=0;
    bool operator==(const ColorSettings&) const = default;
};
// Original harmonic shapers. Run inside the existing 2/4/8x FIR path, before
// clipping and limiting. THD dB is an intensity scale, not measured output THD.
class Coloration {
    double fs=384000,lowCoefficient=0,presenceCoefficient=0,dcCoefficient=0;
    std::array<double,2> low{},presence{},dcInput{},dcOutput{};
    Ramp thd,asymmetric,valve,xl;
    std::array<Ramp,3> modes;
    ColorSettings last{};
    TapeColor tape;
    bool initial=true;
public:
    void prepare(double rate) {
        fs=rate;initial=true;low={};presence={};dcInput={};dcOutput={};
        tape.prepare(rate);
        lowCoefficient=std::exp(-2*pi*180/fs);
        presenceCoefficient=std::exp(-2*pi*1300/fs);
        dcCoefficient=std::exp(-2*pi*8/fs);
    }
    void update(const ColorSettings& s) {
        if(!initial && s==last)return;
        last=s;const int n=std::max(1,static_cast<int>(fs*.02));
        const double intensity=(std::pow(10.,(std::clamp(s.thdDb,-60.,-20.)+20)/20)-.01)/.99;
        tape.update(s.tape,s.tapeMode);
        thd.set(intensity,n,initial);asymmetric.set(s.thdMode==0?1.:0.,n,initial);
        valve.set(s.valve?1.:0.,n,initial);xl.set(std::clamp(s.xl/100,0.,1.),n,initial);
        for(int i=0;i<3;++i)modes[static_cast<size_t>(i)].set(i==std::clamp(s.xlMode,0,2)?1.:0.,n,initial);
        initial=false;
    }
    void process(double& l,double& r) {
        tape.process(l,r);
        const double amount=thd.next(),warm=asymmetric.next(),tube=valve.next(),bloom=xl.next();
        std::array<double,3> weights{};for(size_t i=0;i<3;++i)weights[i]=modes[i].next();
        std::array<double,2> samples{l,r};
        for(size_t c=0;c<2;++c) {
            const double x=samples[c];
            low[c]=x+(low[c]-x)*lowCoefficient;
            presence[c]=x+(presence[c]-x)*presenceCoefficient;
            double delta=0;
            if(amount>0 || tube>0) {
                const double t=x/std::sqrt(1+x*x),even=t*t,odd=even*t;
                delta=amount*(warm*.16*even-(1-warm)*.4*odd)+tube*(.025*even-.01*odd);
            }
            if(bloom>0) {
                const double upper=x-low[c],detail=x-presence[c];
                if(weights[0]>0)delta+=bloom*weights[0]*.35*(std::tanh(1.8*upper)/1.8-upper);
                if(weights[1]>0)delta+=bloom*weights[1]*.5*(1.4*std::tanh(2.5*detail)/2.5-detail);
                if(weights[2]>0)delta+=bloom*weights[2]*.8*(std::tanh(3*x)/3-x);
            }
            // Block DC only on the generated residual; neutral settings retain
            // the existing signal path exactly, without an extra audio filter.
            const double filtered=delta-dcInput[c]+dcCoefficient*dcOutput[c];
            dcInput[c]=delta;dcOutput[c]=std::abs(filtered)<1.e-30?0:filtered;
            samples[c]=x+dcOutput[c];
            if(std::abs(low[c])<1.e-30)low[c]=0;
            if(std::abs(presence[c])<1.e-30)presence[c]=0;
        }
        l=samples[0];r=samples[1];
    }
};
}
