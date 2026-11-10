#include "DSP/Engine.h"
#include <complex>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
using namespace bm::dsp;
void require(bool ok,const char* message){if(!ok){std::fprintf(stderr,"FAIL %s\n",message);std::exit(1);}}
struct Spectrum {double fundamental=0,second=0,third=0,mean=0;};
Spectrum spectrum(ColorSettings s,double hz=1000,double amplitude=.5) {
    Coloration color;color.prepare(192000);color.update(s);
    std::array<std::complex<double>,3> bins{};double mean=0;
    constexpr int count=19200;
    for(int i=0;i<count*12;++i){double l=amplitude*std::sin(2*pi*hz*i/192000),r=l;color.process(l,r);
        require(std::isfinite(l)&&l==r,"finite, identical channels stay identical");
        if(i>=count*11){mean+=l;for(int h=1;h<=3;++h)bins[static_cast<size_t>(h-1)]+=l*std::polar(1.,-2*pi*hz*h*i/192000);}}
    return {2*std::abs(bins[0])/count,2*std::abs(bins[1])/count,2*std::abs(bins[2])/count,mean/count};
}
ColorSettings configuration(int mode){ColorSettings s;
    if(mode<2){s.thdDb=-20;s.thdMode=mode;}
    else if(mode<5){s.xl=100;s.xlMode=mode-2;}
    else s.valve=true;
    return s;
}
void exports(const std::filesystem::path& directory) {
    std::filesystem::create_directories(directory);
    // Coherent one-second windows after settling; raw doubles for independent
    // spectral and reconstruction analysis, no WAV codec or preset involved.
    for(int mode=0;mode<6;++mode)for(int factor:{1,2,4,8}) {
        const auto settings=configuration(mode);
        const double hz=mode==0||mode==5?14000.:7000.;
        Coloration native;native.prepare(48000);native.update(settings);
        PeakChain chain;chain.prepare(48000,factor==1?8:factor);
        PeakSettings peak;peak.color=settings;peak.clipEnabled=false;peak.limiterEnabled=false;chain.update(peak);
        std::ofstream file(directory/("color-"+std::to_string(mode)+"-"+std::to_string(factor)+".f64"),std::ios::binary);
        for(int i=0;i<96000;++i){double l=.8*std::sin(2*pi*hz*i/48000),r=l;
            if(factor==1)native.process(l,r);else chain.process(l,r);
            if(i>=48000)file.write(reinterpret_cast<const char*>(&l),sizeof(l));}
        require(bool(file),"export write");
    }
}
int main(int argc,char** argv){
    if(argc==3 && std::string_view(argv[1])=="--export"){exports(argv[2]);return 0;}
    ColorSettings neutral;Coloration color;color.prepare(384000);color.update(neutral);
    for(int i=0;i<5000;++i){double l=std::sin(i*.7)*2,r=-l*.3,x=l,y=r;color.process(l,r);require(l==x&&r==y,"neutral exact unity");}
    const auto warm=spectrum(configuration(0)),dense=spectrum(configuration(1)),valve=spectrum(configuration(5));
    require(warm.second>warm.third*20,"warm dominated by second harmonic");
    require(dense.third>dense.second*100,"dense dominated by odd harmonics");
    require(valve.second>valve.third*2 && valve.second<warm.second,"valve is mild and asymmetric");
    require(std::abs(warm.mean)<1.e-8 && std::abs(valve.mean)<1.e-8,"generated DC removed");
    auto mild=configuration(0);mild.thdDb=-40;
    require(spectrum(mild).second<warm.second*.2,"THD intensity monotonic");
    auto density=configuration(2),presence=configuration(3),impact=configuration(4);
    const auto bass=spectrum(density,40),bassImpact=spectrum(impact,40);
    require(std::abs(bass.fundamental/.5-1)<.01,"density preserves sub bass");
    require(bassImpact.third>bass.third*5,"impact acts on full band");
    require(spectrum(presence,4000).fundamental>spectrum(density,4000).fundamental,"presence has distinct upper-mid response");
    for(double fs:{44100.,48000.,96000.,192000.})for(int factor:{2,4,8}) {
        PeakChain chain;chain.prepare(fs,factor);PeakSettings s;s.color=configuration(0);chain.update(s);
        const int latency=chain.latency();
        double previous=0;
        for(int i=0;i<static_cast<int>(fs*.1);++i){
            if(i%257==0){s.color=configuration((i/257)%6);chain.update(s);}
            double l=2*std::sin(2*pi*997*i/fs),r=0;chain.process(l,r);
            require(std::isfinite(l)&&std::isfinite(r),"mode automation finite");
            require(std::abs(l)<=dbGain(s.ceilingDb)+1.e-8,"final ceiling with coloration");
            require(r==0,"no stereo crosstalk");
            require(std::abs(l-previous)<1.5,"bounded automation transition");previous=l;
        }
        require(chain.latency()==latency,"coloration adds no latency");
    }
    std::printf("PASS harmonic balance, dosage, DC, neutral, sub preservation, six modes, stereo, automation and ceiling at four rates / three OS factors\n");
}
