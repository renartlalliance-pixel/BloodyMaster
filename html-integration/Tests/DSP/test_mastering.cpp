#include "DSP/MasteringCore.h"
#include <vector>
#include <iostream>
#include <random>
#include <cstring>
#include <cstdlib>
#include <complex>
using namespace bm::dsp;
int checks=0;
void expect(bool ok,const char* name) { ++checks; if(!ok) { std::cerr<<"FAIL: "<<name<<'\n'; std::exit(1); } }
double response(Type type,double fs,double corner,double gain,double hz) {
    Biquad f; f.set(design(type,fs,corner,gain,1),1,true);
    double in=0,out=0;
    for(int i=0;i<static_cast<int>(fs);++i) {
        const double x=.1*std::sin(2*pi*hz*i/fs); f.tick(); const double y=f.process(x,0);
        if(i>fs*.5) { in+=x*x; out+=y*y; }
    }
    return 10*std::log10(out/in);
}
double magnitude(Coefficients c,double fs,double hz) {
    const auto z=std::polar(1.0,-2*pi*hz/fs);
    return std::abs((c.b0+c.b1*z+c.b2*z*z)/(1.+c.a1*z+c.a2*z*z));
}
int main() {
    for(double fs:{44100.,48000.,96000.,192000.}) {
        expect(std::abs(response(Type::peak,fs,1000,6,1000)-6)<.01,"peak +6 dB at centre");
        expect(std::abs(response(Type::peak,fs,1000,-6,1000)+6)<.01,"peak -6 dB at centre");
        expect(response(Type::highpass,fs,100,0,10)<-39,"highpass low-frequency rejection");
        expect(response(Type::lowpass,fs,1000,0,10000)<-39,"lowpass high-frequency rejection");
        expect(std::abs(response(Type::lowShelf,fs,200,6,20)-6)<.02,"low shelf plateau");
        expect(std::abs(response(Type::highShelf,fs,1000,6,10000)-6)<.02,"high shelf plateau");
        const auto low=passiveLow(fs,6);
        const auto db=[&](double hz) { return 20*std::log10(magnitude(low[0],fs,hz)*magnitude(low[1],fs,hz)); };
        expect(db(20)>5.8 && db(20)<6.1,"passive bass boost level");
        expect(db(160)<-.5,"passive bass low-mid dip");
        expect(std::abs(db(2000))<.02,"passive bass leaves upper spectrum intact");
        expect(std::abs(response(Type::peak,fs,8000,6,8000)-6)<.01,"treble peak level");
        const int n=16000; std::vector<float> l(n),r(n),original(n);
        std::mt19937 random(23); std::uniform_real_distribution<float> noise(-.1f,.1f);
        for(int i=0;i<n;++i) l[i]=r[i]=original[i]=noise(random);
        float* data[]{l.data(),r.data()}; Settings s; s.lpf=fs;
        MasteringCore core; core.prepare(fs); core.update(s); core.process(data,2,n);
        double error=0; for(int i=0;i<n;++i) error=std::max(error,std::abs(double(l[i]-original[i])));
        expect(error<1.e-7,"neutral chain transparency");
        s.bass=6; s.hpf=100; s.bypass=true; core.prepare(fs); core.update(s);
        l=original; r=original; core.process(data,2,n);
        expect(std::memcmp(l.data(),original.data(),n*sizeof(float))==0,"initial bypass bit exact");
        // Settled bypass after processing and automation must return exact dry.
        s.bypass=false; core.update(s); core.process(data,2,n); s.bypass=true; core.update(s);
        l=original; r=original; core.process(data,2,n);
        const int settle=static_cast<int>(fs*.01);
        expect(std::memcmp(l.data()+settle,original.data()+settle,(n-settle)*sizeof(float))==0,"bypass fade settles exact");
        s=Settings{}; s.lpf=fs; s.width=0; core.prepare(fs); core.update(s);
        for(int i=0;i<n;++i) { l[i]=original[i]; r[i]=-original[i]; }
        core.process(data,2,n); double peak=0; for(float x:l) peak=std::max(peak,std::abs(double(x)));
        expect(peak<1.e-7,"zero width cancels side-only signal");
        s.width=200; s.monoBass=200; core.prepare(fs); core.update(s);
        for(int i=0;i<n;++i) l[i]=r[i]=original[i];
        core.process(data,2,n);
        error=0; for(int i=0;i<n;++i) error=std::max(error,std::abs(double(l[i]-original[i])));
        expect(error<1.e-7,"mono image preserved by width and mono bass");
        s=Settings{}; s.lpf=fs; s.monoBass=200; core.prepare(fs); core.update(s);
        for(int i=0;i<n;++i) { l[i]=float(.1*std::sin(2*pi*20*i/fs)); r[i]=-l[i]; }
        core.process(data,2,n); double energy=0; for(int i=n/2;i<n;++i) energy+=l[i]*l[i];
        expect(std::sqrt(energy/(n/2))<.001,"mono bass attenuates low-frequency side");
        // The same stream must not change with host block partitioning.
        s=Settings{}; s.hpf=30; s.bass=3; s.mid=-2; s.width=125;
        MasteringCore a,b; a.prepare(fs); b.prepare(fs); a.update(s); b.update(s);
        std::vector<float> x=original,y=original; float* pa[]{x.data()}; a.process(pa,1,n);
        for(int pos=0;pos<n;pos+=37) { float* pb[]{y.data()+pos}; b.update(s); b.process(pb,1,std::min(37,n-pos)); }
        expect(std::memcmp(x.data(),y.data(),n*sizeof(float))==0,"block-size independence");
        // Stress automation at legal extremes, including high EQ frequencies at 44.1 kHz.
        core.prepare(fs);
        for(int j=0;j<150;++j) {
            s.hpf=j%2?100:15; s.lpf=j%2?18000:45000; s.bass=j%2?6:-6;
            s.freq={30.+(j%2)*370.,400.+(j%2)*3600.,4000.+(j%2)*14000.};
            s.gain={s.bass,-s.bass,s.bass}; s.width=j%2?200:0;
            core.update(s); for(int i=0;i<127;++i) { l[i]=noise(random); r[i]=noise(random); }
            core.process(data,2,127);
            for(int i=0;i<127;++i) expect(std::isfinite(l[i])&&std::isfinite(r[i])&&std::abs(l[i])<10&&std::abs(r[i])<10,"automation bounded output");
        }
        std::fill(l.begin(),l.end(),0); std::fill(r.begin(),r.end(),0);
        core.prepare(fs); core.update(Settings{}); core.process(data,2,n);
        expect(l==r && l==std::vector<float>(n,0),"reset and silence");
    }
    std::cout<<"PASS: "<<checks<<" assertions, 44.1/48/96/192 kHz\n";
}
