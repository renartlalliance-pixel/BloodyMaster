// Audio-only performance probe, not a FL Studio CPU measurement.
// Compile with the same compiler/options for before/after comparison:
// c++ -std=c++20 -O3 -DNDEBUG -I Source tools/benchmark_dsp.cpp -o benchmark_dsp
// Usage: benchmark_dsp [oversampling: 2/4/8] [duration seconds]
// Includes mastering, final protection, output metering and A/B learning meter.
// Excludes JUCE/UI/driver/Delta; reported percentage is one core's elapsed budget.
#include "DSP/Engine.h"
#include "DSP/AudioMeter.h"
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <vector>
int main(int argc,char** argv){
 const int os=argc>1?std::atoi(argv[1]):4,seconds=argc>2?std::atoi(argv[2]):3;const double fs=48000;constexpr int block=512;
 if((os!=2&&os!=4&&os!=8)||seconds<1||seconds>600){std::fprintf(stderr,"Expected OS 2/4/8 and duration 1..600 seconds\n");return 2;}
 bm::dsp::Engine engine;bm::dsp::OutputSafety guard;bm::dsp::AudioMeter inputMeter,outputMeter;
 engine.prepare(fs,os);guard.prepare(fs);inputMeter.prepare(fs);outputMeter.prepare(fs);
 bm::dsp::EngineSettings s;s.dynamics.drive=6;s.peaks.clipDb=2;s.peaks.loudnessWarDb=6;s.peaks.color.xl=12;s.peaks.color.thdDb=-42;s.peaks.color.tape=8;s.cleanup.enabled=true;s.cleanup.amount=20;engine.update(s);guard.setCeiling(-3);
 std::vector<float> l(block),r(block);float* p[]{l.data(),r.data()};double checksum=0;auto start=std::chrono::steady_clock::now();
 for(int n=0;n<int(fs*seconds);n+=block){for(int i=0;i<block;++i){double t=(n+i)/fs;l[i]=float(.3*std::sin(t*2*bm::dsp::pi*61)+.07*std::sin(t*2*bm::dsp::pi*7001));r[i]=float(.28*std::sin(t*2*bm::dsp::pi*63)+.08*std::sin(t*2*bm::dsp::pi*6701));}engine.process(p,2,block);
 for(int i=0;i<block;++i){inputMeter.process(l[i],r[i],2,false);double a=l[i],b=r[i];guard.process(a,b,2);outputMeter.process(float(a),float(b),2);checksum+=a+b;}}
 double wall=std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count();std::printf("os=%d audioSeconds=%d wall=%.6f oneCorePercent=%.2f checksum=%.12f\n",os,seconds,wall,100*wall/seconds,checksum);
}
