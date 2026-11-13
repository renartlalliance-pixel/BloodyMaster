#pragma once
#include "MasteringCore.h"
#include <cstdint>
#include <vector>

namespace bm::dsp {
struct LoudnessValues {
    double integrated=-200, shortTerm=-200, truePeak=0;
};
// Passive output measurement. BS.1770 K weighting and 400 ms / 100 ms gating.
// Fixed 0.01 LU histogram bounds relative-gate quantisation to one bin.
// True peak is a 4x, 64-sample-span Blackman-windowed sinc estimate.
// Neither estimator changes the audio or adds transport latency.
class AudioMeter {
    static constexpr int bins=17001;
    std::vector<double> energy;
    std::vector<uint64_t> counts;
    std::array<double,30> blocks{};
    std::array<std::array<double,65>,4> taps{};
    std::array<std::array<double,65>,2> history{};
    Biquad shelf,rlb;
    int position=0,blockPosition=0,blockSamples=4800,samples=0,filled=0;
    double partial=0;
    static double lufs(double power) { return power>0?-.691+10*std::log10(power):-200; }
    void integrate(double power) {
        const double loudness=lufs(power);
        if(loudness < -70)return;
        const auto bin=static_cast<size_t>(std::clamp(static_cast<int>(std::floor((loudness+70)*100)),0,bins-1));
        energy[bin]+=power;++counts[bin];
        double sum=0;uint64_t count=0;
        for(size_t i=0;i<energy.size();++i){sum+=energy[i];count+=counts[i];}
        const double threshold=std::max(-70.,lufs(sum/static_cast<double>(count))-10);
        // Include bins whose lower bound passes the relative gate. At most
        // 0.01 LU of threshold uncertainty, no duration-dependent storage.
        const int start=std::clamp(static_cast<int>(std::ceil((threshold+70)*100)),0,bins-1);
        sum=0;count=0;
        for(int i=start;i<bins;++i){sum+=energy[static_cast<size_t>(i)];count+=counts[static_cast<size_t>(i)];}
        values.integrated=count?lufs(sum/static_cast<double>(count)):-200;
    }
public:
    LoudnessValues values{};
    void prepare(double fs) {
        fs=std::isfinite(fs)?std::clamp(fs,8000.,384000.):48000.;
        energy.assign(bins,0);counts.assign(bins,0);
        blockSamples=std::max(1,static_cast<int>(std::round(fs*.1)));
        const double k=std::tan(pi*1681.974450955533/fs),vh=std::pow(10.,3.999843853973347/20),vb=std::pow(vh,.4996667741545416),q=.7071752369554196;
        const double a=1+k/q+k*k;
        shelf.set({(vh+vb*k/q+k*k)/a,2*(k*k-vh)/a,(vh-vb*k/q+k*k)/a,2*(k*k-1)/a,(1-k/q+k*k)/a},1,true);
        const double h=std::tan(pi*38.13547087602444/fs),hq=.5003270373238773,ha=1+h/hq+h*h;
        rlb.set({1,-2,1,2*(h*h-1)/ha,(1-h/hq+h*h)/ha},1,true);
        for(size_t phase=0;phase<4;++phase){double sum=0;
            for(size_t i=0;i<65;++i){const double x=double(i)-32+double(phase)/4;
                const double window=std::abs(x)<=32?.42+.5*std::cos(pi*x/32)+.08*std::cos(2*pi*x/32):0;
                taps[phase][i]=(x==0?1:std::sin(pi*x)/(pi*x))*window;sum+=taps[phase][i];}
            for(auto& t:taps[phase])t/=sum;
        }
        reset();
    }
    void reset() {
        std::fill(energy.begin(),energy.end(),0);std::fill(counts.begin(),counts.end(),0);blocks.fill(0);history={};
        shelf.reset();rlb.reset();position=blockPosition=samples=filled=0;partial=0;values={};
    }
    void process(double l,double r,int channels,bool analysePeak=true) {
        if(channels<1 || channels>2)return;
        shelf.tick();rlb.tick();
        const std::array<double,2> input{l,r};
        double power=0;
        for(int c=0;c<channels;++c){const auto ch=static_cast<size_t>(c);const double x=std::isfinite(input[ch])?input[ch]:0;
            const double weighted=rlb.process(shelf.process(x,c),c);power+=weighted*weighted;
            history[ch][static_cast<size_t>(position)]=x;
            values.truePeak=std::max(values.truePeak,std::abs(x));
            if(analysePeak)for(const auto& phase:taps){double y=0;int index=position;
                for(size_t i=0;i<phase.size();++i){y+=phase[i]*history[ch][static_cast<size_t>(index)];if(--index<0)index=64;}
                values.truePeak=std::max(values.truePeak,std::abs(y));}
        }
        if(++position==65)position=0;
        partial+=power;
        if(++samples==blockSamples){
            blocks[static_cast<size_t>(blockPosition)]=partial/blockSamples;partial=0;samples=0;
            if(++blockPosition==30)blockPosition=0;
            filled=std::min(30,filled+1);
            if(filled>=4){double sum=0;for(int i=0;i<4;++i)sum+=blocks[static_cast<size_t>((blockPosition+29-i)%30)];integrate(sum/4);}
            if(filled==30){double sum=0;for(double v:blocks)sum+=v;values.shortTerm=lufs(sum/30);}
        }
    }
};
}
