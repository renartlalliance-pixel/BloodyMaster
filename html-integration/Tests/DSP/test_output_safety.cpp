#include "DSP/Engine.h"
#include <cstdio>
#include <cstdlib>
#include <limits>
using namespace bm::dsp;
void require(bool b,const char* s){if(!b){std::fprintf(stderr,"FAIL %s\n",s);std::exit(1);}}
int main(){
 OutputSafety invalidRate;invalidRate.prepare(std::numeric_limits<double>::quiet_NaN());require(invalidRate.latency()==512,"invalid rate falls back safely");
 for(double fs:{44100.,48000.,96000.,192000.}){
    OutputSafety guard;guard.prepare(fs);guard.setCeiling(0);
    for(int i=0;i<static_cast<int>(fs*2);++i){double l=2*std::sin(2*pi*1000*i/fs),r=l;guard.process(l,r,2);
        require(std::isfinite(l)&&l==r&&std::abs(l)<=dbGain(-2.5)+1e-12,"hot stereo finite linked bounded");}
    require(guard.reductionDb>6,"dense hot input attenuated");
    for(double invalid:{std::numeric_limits<double>::infinity(),std::numeric_limits<double>::quiet_NaN(),1e300}){double l=invalid,r=-invalid;guard.process(l,r,2);require(std::isfinite(l)&&std::isfinite(r),"invalid input contained");}
    Engine e;e.prepare(fs,8,true);EngineSettings s;s.bypass=true;s.peaks.ceilingDb=0;e.update(s);
    for(int i=0;i<static_cast<int>(fs);++i){float l=10,r=-10;float* f[]{&l,&r};e.process(f,2,1);require(std::abs(l)<=dbGain(-2.5)+1e-6&&r==-l,"bypass cannot defeat protection");}
    e.prepare(fs,8,true);s.peaks.ceilingDb=-6;e.update(s);
    float first=.01f;std::vector<float> input(static_cast<size_t>(e.latency()+1),0);input[0]=first;
    for(size_t i=0;i<input.size();++i){float* f[]{&input[i]};e.process(f,1,1);}
    require(input.back()==first,"safe bypass impulse aligned and unchanged");
 }
 std::puts("PASS output safety: four rates, hot input, stereo, nonfinite, bypass and latency");
}
