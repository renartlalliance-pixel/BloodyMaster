#include "DSP/Engine.h"
#include <cstdio>
#include <cstdlib>
using namespace bm::dsp;
static int checks=0;
static void check(bool ok,const char* name) { ++checks; if(!ok) { std::fprintf(stderr,"FAIL %s\n",name); std::exit(1); } }
static double steady(double fs,double hz,bool optical,double ratio=4) {
    BusCompressor c;c.prepare(fs);
    for(int i=0;i<static_cast<int>(fs*1.5);++i) {
        double l=.9*std::sin(2*pi*hz*i/fs),r=l;
        c.process(l,r,ratio,-12,100,1,1,optical,8);
    }
    return c.maxReduction();
}
static double tail(double fs,bool optical) {
    BusCompressor c;c.prepare(fs);
    for(int i=0;i<static_cast<int>(fs*1.5);++i) {
        double l=.9*std::sin(2*pi*1000*i/fs),r=l;
        c.process(l,r,4,-12,100,1,1,optical,8);
    }
    for(int i=0;i<static_cast<int>(fs*.2);++i) {
        double l=0,r=0;c.process(l,r,4,-12,100,1,1,optical,8);
    }
    return c.maxReduction();
}
int main() {
    // Shape invariants, not a comparison with measured hardware.
    for(double ratio:{1.,1.5,2.,4.,8.}) {
        double previous=0;
        for(int i=0;i<1000;++i) {
            const double reduction=compressionDb(-40+i*.06,-12,ratio,6);
            check(reduction>=previous-1.e-12,"monotonic soft knee");previous=reduction;
        }
        for(double edge:{-15.,-9.})
            check(std::abs(compressionDb(edge-1.e-7,-12,ratio,6)-compressionDb(edge+1.e-7,-12,ratio,6))<2.e-7,"knee continuity");
    }
    for(double fs:{44100.,48000.,96000.,192000.}) {
        const double bass=steady(fs,40,false),mid=steady(fs,1000,false);
        const double optical=tail(fs,true),pure=tail(fs,false);
        check(mid>1,"compression reacts to sustained signal");
        check(bass<mid-2,"80 Hz sidechain reduces bass-triggered compression");
        check(optical>pure+.1,"program-dependent optical recovery is slower after sustain");
        check(steady(fs,1000,false,1)<1.e-10,"ratio one has no reduction");
        BusCompressor linked;linked.prepare(fs);double error=0;
        for(int i=0;i<static_cast<int>(fs*.2);++i) {
            double l=.8*std::sin(2*pi*997*i/fs),r=l*.1;
            linked.process(l,r,4,-12,100,1,1,false,8);
            error=std::max(error,std::abs(r-.1*l));
        }
        check(error<1.e-12,"linked compressor preserves stereo ratio");
        std::printf("Fs %.0f: GR40Hz %.6f dB, GR1k %.6f dB, tail optical %.6f dB, pure %.6f dB\n",fs,bass,mid,optical,pure);
    }
    for(int i=0;i<=10000;++i) {
        const double x=i*.0003;
        check(std::abs(softClip(x)+softClip(-x))<1.e-12,"clip symmetry");
        check(std::abs(softClip(x))<=1,"clip saturation bound");
    }
    std::printf("PASS %d behavioral checks (not hardware equivalence)\n",checks);
}
