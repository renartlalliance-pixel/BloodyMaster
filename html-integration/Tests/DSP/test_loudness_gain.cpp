#include "DSP/Engine.h"
#include <cstdio>
#include <cstdlib>
#include <vector>
using namespace bm::dsp;
void require(bool ok,const char* s){if(!ok){std::fprintf(stderr,"FAIL %s\n",s);std::exit(1);}}
std::vector<float> render(double fs,int os,EngineSettings s){
    Engine engine;engine.prepare(fs,os);engine.update(s);
    const int length=static_cast<int>(fs*.10);
    std::vector<float> audio(static_cast<size_t>(length+engine.latency()),0);
    for(int i=0;i<length;++i)audio[static_cast<size_t>(i)]=float(.001*std::sin(2*pi*1000*i/fs));
    for(size_t pos=0;pos<audio.size();pos+=113){float* p[]{audio.data()+pos};engine.process(p,1,int(std::min(size_t(113),audio.size()-pos)));}
    audio.erase(audio.begin(),audio.begin()+engine.latency());return audio;
}
int main(){
    for(double fs:{44100.,48000.,96000.,192000.})for(int os:{2,4,8}){
        EngineSettings s;s.dynamics.enabled=false;s.peaks.limiterEnabled=false;s.peaks.clipEnabled=false;s.tone.lpf=fs;
        const auto base=render(fs,os,s);
        s.inputGainDb=20*std::log10(2.);const auto input=render(fs,os,s);
        s.inputGainDb=0;s.peaks.loudnessWarDb=20*std::log10(2.);const auto output=render(fs,os,s);
        for(size_t i=0;i<base.size();++i){
            require(std::abs(input[i]-2*base[i])<2e-7,"independent input gain before DSP");
            require(std::abs(output[i]-2*base[i])<2e-7,"post-colour LoudnessWar gain includes limiter input");
        }
        s.bypass=true;s.inputGainDb=18;s.peaks.loudnessWarDb=18;const auto bypass=render(fs,os,s);
        for(size_t i=0;i<bypass.size();++i)require(std::abs(bypass[i]-float(.001*std::sin(2*pi*1000*double(i)/fs)))<1e-7,"bypass ignores both new processing gains");
    }
    std::puts("PASS independent input / LoudnessWar gains, unity defaults, bypass at four rates and 2/4/8x");
}
