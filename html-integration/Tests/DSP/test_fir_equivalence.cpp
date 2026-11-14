#include "DSP/PeakChain.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <vector>
void check(double actual,double reference){if(std::abs(actual-reference)>1e-12*(1+std::abs(reference))){std::fprintf(stderr,"FIR mismatch: %.17g vs %.17g\n",actual,reference);std::exit(1);}}
int main(){
    std::mt19937 random(410);std::uniform_real_distribution<double> distribution(-2,2);
    for(int factor:{2,4,8}){
        bm::dsp::Interpolator up;up.prepare(factor);bm::dsp::Decimator down;down.prepare(up.coefficients());
        std::array<std::vector<double>,2> interpolation,decimation;
        for(auto& h:interpolation)h.assign(129,0);
        for(auto& h:decimation)h.assign(up.coefficients().size(),0);
        for(int sample=0;sample<5000;++sample){
            // Independent shift-register oracle; exercise many wraps and tails.
            const double l=sample<3000?distribution(random):0,r=sample==1?5:sample<3000?distribution(random):0;
            up.push(l,r);
            for(int channel=0;channel<2;++channel){auto& h=interpolation[static_cast<size_t>(channel)];std::move_backward(h.begin(),h.end()-1,h.end());h[0]=channel?r:l;
                for(int phase=0;phase<factor;++phase){double reference=0;size_t lag=0;
                    for(size_t k=static_cast<size_t>(phase);k<up.coefficients().size();k+=static_cast<size_t>(factor))reference+=up.coefficients()[k]*h[lag++];
                    check(up.phase(channel,phase),reference*factor);
                }
            }
            down.push(l,r);
            for(int channel=0;channel<2;++channel){auto& h=decimation[static_cast<size_t>(channel)];std::move_backward(h.begin(),h.end()-1,h.end());h[0]=channel?r:l;double reference=0;
                for(size_t k=0;k<h.size();++k)reference+=up.coefficients()[k]*h[k];
                check(down.output(channel),reference);
            }
        }
        // Reprepare must discard all history, including mirrored storage.
        up.prepare(factor);down.prepare(up.coefficients());up.push(0,0);down.push(0,0);
        for(int channel=0;channel<2;++channel){check(down.output(channel),0);for(int phase=0;phase<factor;++phase)check(up.phase(channel,phase),0);}
    }
    std::puts("PASS contiguous FIR vs independent shift-register oracle: 2/4/8x, stereo, wraps, silence and reprepare");
}
