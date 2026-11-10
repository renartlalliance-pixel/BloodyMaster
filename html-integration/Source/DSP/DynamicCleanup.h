#pragma once
#include "MasteringCore.h"

namespace bm::dsp {
struct CleanupSettings {
    double harshHz=7500,mudHz=160,ringHz=4200,amount=0,mudAmount=25,ringAmount=25;
    bool enabled=false,mudEnabled=false,ringEnabled=false,listen=false;
    bool operator==(const CleanupSettings&) const = default;
};
// TPT state-variable bandpass, normalised to unity at its centre. Frequency
// changes move g smoothly; no interpolated direct-form poles or heap storage.
class CleanupBand {
    std::array<double,2> ic1{},ic2{};
    Ramp g;
    double k=1,fs=48000,power=0,reduction=0;
    double detectAttack=0,detectRelease=0,gainAttack=0,gainRelease=0;
public:
    void prepare(double rate,double q,double attackMs,double releaseMs) {
        fs=rate;k=1/q;ic1={};ic2={};power=reduction=0;
        detectAttack=std::exp(-1/(fs*.003));detectRelease=std::exp(-1/(fs*.05));
        gainAttack=std::exp(-1/(fs*attackMs*.001));gainRelease=std::exp(-1/(fs*releaseMs*.001));
    }
    void tune(double hz,bool snap) {
        g.set(std::tan(pi*std::clamp(hz,20.,fs*.45)/fs),static_cast<int>(fs*.02),snap);
    }
    std::array<double,2> filter(double l,double r) {
        const double coefficient=g.next(),a=1/(1+coefficient*(coefficient+k));
        std::array<double,2> band{};const std::array<double,2> in{l,r};
        for(size_t c=0;c<2;++c) {
            const double v3=in[c]-ic2[c],v1=a*(ic1[c]+coefficient*v3),v2=ic2[c]+coefficient*v1;
            ic1[c]=2*v1-ic1[c];ic2[c]=2*v2-ic2[c];
            if(std::abs(ic1[c])<1.e-30)ic1[c]=0;
            if(std::abs(ic2[c])<1.e-30)ic2[c]=0;
            band[c]=k*v1;
        }
        return band;
    }
    double detect(const std::array<double,2>& band,double broadband,double thresholdDb,double strength) {
        const double energy=std::max(band[0]*band[0],band[1]*band[1]);
        const double coefficient=energy>power?detectAttack:detectRelease;
        power=energy+(power-energy)*coefficient;
        if(strength<=0){reduction=0;return 0;}
        // Relative spectral prominence, with a -70 dBFS RMS gate to avoid
        // chasing quiet tails. This is not an automatic resonance finder.
        const double prominence=10*std::log10((power+1.e-20)/(broadband+1.e-20));
        const double desired=broadband>1.e-7?std::clamp(prominence-thresholdDb,0.,12.)*strength:0.;
        const double smoothing=desired>reduction?gainAttack:gainRelease;
        reduction=desired+(reduction-desired)*smoothing;
        // Suppress off-centre responses: a neighbouring band must not consume
        // most of the shared budget merely because its detector has a wide skirt.
        const double confidence=std::clamp(power/(broadband+1.e-20),0.,1.);
        return (1-std::pow(10.,-reduction/20))*confidence;
    }
};
class DynamicCleanup {
    std::array<CleanupBand,3> bands;
    CleanupSettings last{};
    std::array<Ramp,3> amount;
    double fs=48000,broadband=0,broadAttack=0,broadRelease=0;
    bool initial=true;
public:
    std::array<double,2> removed{};
    std::array<double,3> bandReductionDb{};
    void prepare(double rate) {
        fs=std::clamp(rate,8000.,384000.);initial=true;broadband=0;removed={};bandReductionDb={};
        broadAttack=std::exp(-1/(fs*.003));broadRelease=std::exp(-1/(fs*.05));
        bands[0].prepare(fs,1.2,1,80);bands[1].prepare(fs,3.,12,180);bands[2].prepare(fs,3.,3,120);
    }
    void update(const CleanupSettings& s) {
        if(!initial && s==last)return;
        last=s;
        bands[0].tune(s.harshHz,initial);bands[1].tune(s.mudHz,initial);bands[2].tune(s.ringHz,initial);
        const std::array<double,3> strength{s.amount,s.mudEnabled?s.mudAmount:0,s.ringEnabled?s.ringAmount:0};
        for(size_t i=0;i<3;++i)amount[i].set(s.enabled?std::clamp(strength[i]/100,0.,1.):0.,static_cast<int>(fs*.02),initial);
        initial=false;
    }
    void process(double& l,double& r) {
        std::array<double,3> strength{};
        for(size_t i=0;i<3;++i)strength[i]=amount[i].next();
        const double energy=std::max(l*l,r*r);
        const double smoothing=energy>broadband?broadAttack:broadRelease;
        broadband=energy+(broadband-energy)*smoothing;
        std::array<std::array<double,2>,3> filtered{};
        std::array<double,3> depth{};double totalDepth=0;
        for(size_t i=0;i<3;++i) {
            filtered[i]=bands[i].filter(l,r);
            depth[i]=bands[i].detect(filtered[i],broadband,i==0?-3-15*strength[i]:-12,strength[i]);
            totalDepth+=depth[i];
        }
        // Parallel complementary cuts share a 12 dB depth budget. At fixed
        // settings each normalised bandpass has magnitude <=1, so overlap
        // cannot turn two coincident 12 dB cuts into a 24 dB notch.
        const double budget=1-std::pow(10.,-12*std::max({strength[0],strength[1],strength[2]})/20);
        const double scale=totalDepth>budget && totalDepth>0?budget/totalDepth:1;
        removed={};
        for(size_t i=0;i<3;++i) {
            depth[i]*=scale;
            bandReductionDb[i]=-20*std::log10(std::max(1.e-12,1-depth[i]));
            for(size_t c=0;c<2;++c)removed[c]+=depth[i]*filtered[i][c];
        }
        l-=removed[0];r-=removed[1];
    }
};
}
