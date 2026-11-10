#include "DSP/PeakChain.h"
#include <cstdio>
#include <cstdlib>
using namespace bm::dsp;
static int checks=0;
static void check(bool ok) { ++checks; if(!ok) { std::fprintf(stderr,"FAIL check %d\n",checks); std::exit(1); } }
int main() {
    for(double fs:{44100.,48000.,96000.,192000.}) {
        PeakSettings settings; settings.clipDb=3; settings.pushDb=2;
        for(int from:{2,4,8}) for(int to:{2,4,8}) {
            SwitchablePeakChain bank; bank.prepare(fs,from); bank.update(settings);
            PeakChain oldMode,newMode; oldMode.prepare(fs,from); newMode.prepare(fs,to);
            oldMode.update(settings); newMode.update(settings);
            const int warm=bank.latency()+Interpolator::span;
            const int fade=static_cast<int>(fs*.02), change=73;
            for(int i=0;i<change+warm+fade+40;++i) {
                const double input=1.7*std::sin(2*pi*997*i/fs);
                double l=input,r=input*.3,ol=l,orr=r,nl=l,nr=r;
                if(i==change) bank.select(to);
                bank.process(l,r); oldMode.process(ol,orr);
                double expectedL=ol,expectedR=orr;
                if(i>=change && from!=to) {
                    newMode.process(nl,nr);
                    const double mix=std::clamp(double(i-change-warm+1)/fade,0.,1.);
                    expectedL=ol+mix*(nl-ol); expectedR=orr+mix*(nr-orr);
                }
                check(std::abs(l-expectedL)<1.e-12 && std::abs(r-expectedR)<1.e-12);
                check(bank.latency()==192+static_cast<int>(std::ceil(fs*.003)));
                check(std::isfinite(l) && std::isfinite(r));
                check(std::abs(l)<=dbGain(-1)+1.e-12 && std::abs(r)<=dbGain(-1)+1.e-12);
            }
        }
        SwitchablePeakChain bank; bank.prepare(fs); bank.update(settings);
        // Rapid automation coalesces requests; silence and latency stay stable.
        for(int i=0;i<10000;++i) {
            if(i%37==0) bank.select(2 << ((i/37)%3));
            double l=0,r=0; bank.process(l,r); check(l==0 && r==0);
        }
    }
    std::printf("PASS %d oversampling switching checks\n",checks);
}
