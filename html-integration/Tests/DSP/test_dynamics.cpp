#include "DSP/Engine.h"
#include <iostream>
#include <vector>
#include <random>
#include <cstdlib>
#include <cstring>
using namespace bm::dsp;
int checks=0;
void require(bool ok,const char* name) { ++checks; if(!ok) { std::cerr<<"FAIL "<<name<<'\n'; std::exit(1); } }
std::vector<float> run(double fs,int os,EngineSettings s,const std::vector<float>& x,int block=113) {
    Engine e; e.prepare(fs,os); e.update(s);
    auto y=x; y.resize(x.size()+e.latency(),0);
    for(size_t i=0;i<y.size();i+=block) { float* p[]{y.data()+i}; e.update(s); e.process(p,1,static_cast<int>(std::min(size_t(block),y.size()-i))); }
    y.erase(y.begin(),y.begin()+e.latency()); return y;
}
int main() {
    require(std::abs(compressionDb(-6,-12,4,6)-4.5)<1.e-12,"compressor ratio law");
    require(compressionDb(-30,-12,4,6)==0,"below knee unity");
    require(std::abs(softClip(1.25)-1)<1.e-12,"soft clip maximum");
    require(softClip(.5)==.5,"soft clip linear zone");
    for(int os:{2,4,8}) for(double fs:{44100.,48000.,96000.,192000.}) {
        EngineSettings s; s.dynamics.enabled=false; s.peaks.limiterEnabled=false; s.tone.lpf=fs;
        const int n=static_cast<int>(fs*.08); std::vector<float> x(n);
        x[0]=.1f;
        auto y=run(fs,os,s,x);
        auto peak=std::max_element(y.begin(),y.end(),[](float a,float b){return std::abs(a)<std::abs(b);});
        require(peak==y.begin(),"latency impulse alignment");
        s.bypass=true; auto bypass=run(fs,os,s,x);
        require(bypass==x,"delayed bypass exact");
        s.bypass=false; s.peaks.limiterEnabled=true; s.peaks.ceilingDb=-1; s.peaks.clipDb=4;
        std::mt19937 gen(42); std::uniform_real_distribution<float> rand(-1.5f,1.5f);
        for(auto& v:x) v=rand(gen);
        x[10]=10;
        y=run(fs,os,s,x);
        bool finite=true,bounded=true; for(float v:y) { finite&=std::isfinite(v); bounded&=std::abs(v)<=dbGain(-1)+1.e-6; }
        require(finite && bounded,"sample ceiling under overload");
        auto z=run(fs,os,s,x,257); require(y==z,"block-size invariance");
        std::fill(x.begin(),x.end(),0); y=run(fs,os,s,x);
        require(y==x,"no noise on silence");
    }
    EngineSettings s; s.tone.lpf=48000; s.dynamics.enabled=false; s.peaks.ceilingDb=-1;
    Engine e; e.prepare(48000,8); e.update(s);
    const int n=12000; std::vector<float> l(n),r(n);
    for(int i=0;i<n;++i) { l[i]=float(1.5*std::sin(2*pi*1000*i/48000)); r[i]=l[i]*.1f; }
    float* p[]{l.data(),r.data()}; e.process(p,2,n);
    double maxError=0; for(int i=e.latency()+100;i<n;++i) maxError=std::max(maxError,std::abs(double(r[i]-.1*l[i])));
    require(maxError<1.e-6,"fully linked limiting preserves stereo ratio");
    // Rapid automation, including bypass and ceiling changes, must stay finite.
    for(int b=0;b<150;++b) {
        s.peaks.clipDb=b%2?10:0; s.peaks.ceilingDb=b%2?-6:0; s.peaks.releaseMs=b%2?1:500;
        s.peaks.link=b%2?0:100; s.bypass=b%7==0; s.dynamics.enabled=true;
        s.dynamics.ratio=b%2?8:1.2; s.dynamics.drive=b%2?12:0; e.update(s);
        for(int i=0;i<64;++i) { l[i]=float(std::sin(i*.7)); r[i]=float(std::cos(i*.3)); }
        e.process(p,2,64); for(int i=0;i<64;++i) require(std::isfinite(l[i])&&std::isfinite(r[i])&&std::abs(l[i])<8&&std::abs(r[i])<8,"automation finite and bounded");
    }
    std::cout<<"PASS "<<checks<<" dynamics/peak assertions\n";
}
