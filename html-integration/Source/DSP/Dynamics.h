#pragma once
#include "MasteringCore.h"

namespace bm::dsp {
inline double dbGain(double db) { return std::pow(10.0, db/20.0); }
inline double gainDb(double v) { return 20.0*std::log10(std::max(v,1.e-12)); }
struct DynamicsSettings {
    double drive=0, ratio=2, releaseMs=100, link=100, mix=100, glue=0;
    bool optical=true, enabled=true;
    bool operator==(const DynamicsSettings&) const = default;
};
// Soft-knee downward transfer, in decibels. Gain reduction is nonnegative.
inline double compressionDb(double level,double threshold,double ratio,double knee) {
    const double x=level-threshold, slope=1-1/std::max(1.0,ratio);
    if(x<=-knee*.5) return 0;
    if(x>=knee*.5) return slope*x;
    return slope*(x+knee*.5)*(x+knee*.5)/(2*knee);
}
class BusCompressor {
    double fs=48000, detectorAttack=0, detectorRelease=0;
    std::array<double,2> energy{}, reduction{}, memory{}, previous{}, hp{};
public:
    void prepare(double rate) {
        fs=rate; energy={}; reduction={}; memory={}; previous={}; hp={};
        detectorAttack=std::exp(-1/(fs*.003)); detectorRelease=std::exp(-1/(fs*.025));
    }
    double maxReduction() const { return std::max(reduction[0],reduction[1]); }
    void process(double& l,double& r,double ratio,double threshold,double release,
                 double link,double mix,bool optical,double attackMs) {
        std::array<double,2> in{l,r}, level{};
        const double h=std::exp(-2*pi*80/fs);
        for(int c=0;c<2;++c) {
            hp[c]=h*(hp[c]+in[c]-previous[c]); previous[c]=in[c];
            const double power=hp[c]*hp[c];
            const double coeff=power>energy[c]?detectorAttack:detectorRelease;
            energy[c]=power+(energy[c]-power)*coeff;
            level[c]=10*std::log10(std::max(energy[c],1.e-24));
        }
        const double combined=std::max(level[0],level[1]);
        for(int c=0;c<2;++c) {
            const double linked=level[c]+link*(combined-level[c]);
            const double desired=compressionDb(linked,threshold,ratio,6);
            // Program-dependent release: accumulated reduction lengthens recovery.
            memory[c]+= (desired-memory[c])*(1-std::exp(-1/(fs*.35)));
            const double time=desired>reduction[c]?attackMs*.001:
                release*.001*(optical?1+std::min(memory[c]/6,3.0):1);
            const double coeff=std::exp(-1/(fs*std::max(.001,time)));
            reduction[c]=desired+(reduction[c]-desired)*coeff;
            in[c]*=1+mix*(dbGain(-reduction[c])-1);
        }
        l=in[0]; r=in[1];
    }
};
class Dynamics {
    double fs=48000; bool initial=true; DynamicsSettings last{};
    Ramp drive,ratio,release,link,mix,glue,optical;
    BusCompressor main,cohesion;
public:
    double compressorGr=0,glueGr=0;
    void prepare(double rate) { fs=rate; initial=true; main.prepare(fs); cohesion.prepare(fs); }
    void update(const DynamicsSettings& s) {
        if(!initial && s==last) return;
        last=s;
        const int n=static_cast<int>(fs*.02);
        drive.set(dbGain(std::clamp(s.drive,0.,12.)),n,initial);
        ratio.set(s.enabled?std::clamp(s.ratio,1.,8.):1.,n,initial);
        release.set(std::clamp(s.releaseMs,10.,1200.),n,initial);
        link.set(std::clamp(s.link/100,0.,1.),n,initial);
        mix.set(s.enabled?std::clamp(s.mix/100,0.,1.):0.,n,initial);
        glue.set(s.enabled?std::clamp(s.glue,0.,6.):0.,n,initial);
        optical.set(s.optical?1:0,n,initial); initial=false;
    }
    double inputGain() { return drive.next(); }
    void process(double& l,double& r) {
        const double g=glue.next(), opt=optical.next();
        main.process(l,r,ratio.next(),-12,release.next(),link.next(),mix.next(),opt>.5,8);
        cohesion.process(l,r,1.5,-9-g*1.5,250,1,g/6,true,30);
        compressorGr=main.maxReduction(); glueGr=cohesion.maxReduction()*g/6;
    }
};
}
