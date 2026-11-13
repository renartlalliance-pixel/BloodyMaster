#include "DSP/DeltaMonitor.h"
#include <cstdio>
#include <cstdlib>
using namespace bm::dsp;
void require(bool ok,const char* text){if(!ok){std::fprintf(stderr,"FAIL %s\n",text);std::exit(1);}}
int main(){
 for(double fs:{44100.,48000.,96000.,192000.})for(int channels:{1,2})for(int os:{2,4,8}){
    SwitchablePeakChain peaks;peaks.prepare(fs,os);
    const int latency=peaks.latency()+32+static_cast<int>(std::ceil(fs*.010));
    DeltaMonitor delta;delta.prepare(fs,latency,257);delta.update(true);
    StereoDelay unity;unity.prepare(latency);
    // Time-aligned unity and known half gain, with independent delay reference.
    for(int block=0;block<static_cast<int>(fs*.08/257)+4;++block){
        float l[257],r[257];for(int i=0;i<257;++i){l[i]=static_cast<float>(.1*std::sin((block*257+i)*.217));r[i]=static_cast<float>(.07*std::cos((block*257+i)*.119));}
        const float* input[]{l,r};delta.capture(input,channels,257);
        for(int i=0;i<257;++i){const auto wet=unity.push(l[i],channels==2?r[i]:l[i]);float a=static_cast<float>(wet[0]),b=static_cast<float>(wet[1]);delta.process(a,b,i);
            if(block*257+i>static_cast<int>(fs*.021))require(a==0&&b==0,"unity null after full latency compensation");}
    }
    delta.prepare(fs,latency,257);delta.update(true);unity.prepare(latency);
    for(int block=0;block<static_cast<int>(fs*.08/257)+4;++block){
        float l[257],r[257];for(int i=0;i<257;++i){l[i]=static_cast<float>(.1*std::sin((block*257+i)*.217));r[i]=-l[i];}
        const float* input[]{l,r};delta.capture(input,channels,257);
        for(int i=0;i<257;++i){const auto raw=unity.push(l[i],channels==2?r[i]:l[i]);float a=static_cast<float>(raw[0]*.5),b=static_cast<float>(raw[1]*.5);delta.process(a,b,i);
            if(block*257+i>static_cast<int>(fs*.021))require(a==static_cast<float>(raw[0]*.5)&&b==static_cast<float>(raw[1]*.5),"sign and full output gain included");}
    }
    delta.prepare(fs,0,1);delta.update(true);
    for(int i=0;i<static_cast<int>(fs*.03);++i){float a=static_cast<float>(.03*std::sin(i*.123)),b=a;const float* in[]{&a,&b};delta.capture(in,2,1);delta.process(a,b,0);if(i>fs*.021)require(a==0&&b==0,"zero-latency identity null");}
    // Constant input/output isolate the transition from the programme waveform.
    delta.prepare(fs,0,1);float dry=.25f;const float* in[]{&dry,&dry};float previous=.125f;
    for(int i=0;i<static_cast<int>(fs*.1);++i){if(i==10)delta.update(true);if(i==static_cast<int>(fs*.05))delta.update(false);
        delta.capture(in,2,1);float a=.125f,b=a;delta.process(a,b,0);require(std::abs(a-previous)<1e-5,"smooth toggle");previous=a;}
    // Choose non-equal normal and delta signals for a real transition.
    delta.prepare(fs,0,1);previous=.1f;
    for(int i=0;i<static_cast<int>(fs*.1);++i){if(i==10)delta.update(true);if(i==static_cast<int>(fs*.05))delta.update(false);
        delta.capture(in,2,1);float a=.1f,b=a;delta.process(a,b,0);require(std::abs(a-previous)<.001,"bounded nontrivial fade increments");previous=a;}
    require(previous==.1f,"exact normal return after fade");
    delta.update(true);delta.update(true,true);delta.capture(in,2,1);float a=.1f,b=a;delta.process(a,b,0);require(a==.1f&&b==.1f,"offline render immediately normal");
 }
 std::puts("PASS Delta: four rates, mono/stereo, 2/4/8x latency, unity null, gain/sign, fades, normal return and offline");
}
