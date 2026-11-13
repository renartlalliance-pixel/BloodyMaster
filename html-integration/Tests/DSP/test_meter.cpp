#include "DSP/AudioMeter.h"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string_view>
using namespace bm::dsp;
void require(bool ok,const char* message){if(!ok){std::fprintf(stderr,"FAIL %s\n",message);std::exit(1);}}
int main(int argc,char** argv){
    if(argc==5 && std::string_view(argv[1])=="--raw") {
        const double fs=std::strtod(argv[2],nullptr);const int channels=std::atoi(argv[3]);require(channels==1||channels==2,"raw channels");
        AudioMeter meter;meter.prepare(fs);std::ifstream file(argv[4],std::ios::binary);double l,r;
        while(file.read(reinterpret_cast<char*>(&l),sizeof(l))){r=l;if(channels==2)require(bool(file.read(reinterpret_cast<char*>(&r),sizeof(r))),"complete stereo frame");meter.process(l,r,channels);}
        require(file.eof(),"raw file read");
        std::printf("%.9f %.9f %.9f\n",meter.values.integrated,meter.values.shortTerm,meter.values.truePeak);return 0;
    }
    for(double fs:{44100.,48000.,96000.,192000.}){
        AudioMeter mono,stereo;mono.prepare(fs);stereo.prepare(fs);
        for(int i=0;i<static_cast<int>(fs*3.2);++i){const double x=.1*std::sin(2*pi*1000*i/fs);mono.process(x,x,1);stereo.process(x,x,2);}
        require(std::abs((stereo.values.integrated-mono.values.integrated)-3.0102999566)<.001,"mono counted once and stereo sum");
        require(std::abs(stereo.values.integrated+20)<.12,"1 kHz stereo reference at -20 LUFS");
        require(std::abs(stereo.values.integrated-stereo.values.shortTerm)<.01,"3 second steady short term");
        const auto old=stereo.values.integrated;
        for(int i=0;i<static_cast<int>(fs*3.2);++i)stereo.process(0,0,2);
        require(std::abs(stereo.values.integrated-old)<.5,"relative gate rejects trailing silence");
        stereo.reset();require(stereo.values.integrated==-200&&stereo.values.shortTerm==-200&&stereo.values.truePeak==0,"reset clears all measurements");
        for(int i=0;i<static_cast<int>(fs*.3);++i)stereo.process(0,0,2);
        require(stereo.values.integrated==-200&&stereo.values.shortTerm==-200,"no partial window or silence loudness");
        AudioMeter peak;peak.prepare(fs);
        for(int i=0;i<10000;++i){const double x=.9*std::sin(pi*.5*i+pi/4);peak.process(x,-x,2);}
        require(peak.values.truePeak>.89&&peak.values.truePeak<1.,"4x detects intersample peak above 0.6364 sample peak");
    }
    std::puts("PASS LUFS/true peak: mono/stereo, reference level, windows, gate, silence, reset, ISP at four rates");
}
