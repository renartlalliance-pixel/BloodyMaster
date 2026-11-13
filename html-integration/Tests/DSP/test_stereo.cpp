#include "DSP/Engine.h"
#include <complex>
#include <cstdio>
#include <cstdlib>
using namespace bm::dsp;
void require(bool ok,const char* message){if(!ok){std::fprintf(stderr,"FAIL %s\n",message);std::exit(1);}}
double response(double fs,int profile,double hz,double amount) {
    MasteringCore core;core.prepare(fs);Settings s;s.lpf=fs;s.profile=profile;s.atmosphere=amount;core.update(s);
    std::complex<double> bin{};const int n=static_cast<int>(fs);
    for(int i=0;i<2*n;++i){float l=static_cast<float>(.2*std::sin(2*pi*hz*i/fs)),r=-l;
        float* frame[]{&l,&r};core.process(frame,2,1);
        require(l==-r,"Side does not generate Mid");
        if(i>=n)bin+=double(l)*std::polar(1.,-2*pi*hz*i/fs);
    }
    return 2*std::abs(bin)/n;
}
int main(){
    for(double fs:{44100.,48000.,96000.,192000.}) {
        for(int profile=0;profile<4;++profile){
            MasteringCore core;core.prepare(fs);Settings s;s.lpf=fs;s.atmosphere=100;s.profile=profile;core.update(s);
            for(int i=0;i<10000;++i){float l=static_cast<float>(.2*std::sin(i*.31)),r=l,original=l;
                float* f[]{&l,&r};core.process(f,2,1);require(l==original&&r==original,"centred audio unchanged");}
            for(double hz:{30.,60.,120.}){
                const double db=20*std::log10(response(fs,profile,hz,100)/response(fs,profile,hz,0));
                require(std::abs(db)<.05,"bass preserved within 0.05 dB through 120 Hz");
            }
            const double hz=profile==0?8000:profile==1?3500:profile==2?10000:1200;
            require(response(fs,profile,hz,100)>response(fs,profile,hz,0)*1.06,"profile audibly changes upper Side");
        }
        // Pure Side remains fixed for every Mid pan position.
        for(double pan:{-50.,-25.,0.,25.,50.}){
            MasteringCore core;core.prepare(fs);Settings s;s.lpf=fs;s.balance=pan;core.update(s);
            for(int i=0;i<1000;++i){float l=.25f,r=-l;float* f[]{&l,&r};core.process(f,2,1);require(l==.25f&&r==-.25f,"pan preserves Side");}
            core.prepare(fs);core.update(s);float l=.25f,r=l;float* f[]{&l,&r};core.process(f,2,1);
            require(std::abs(l*l+r*r-.125)<1e-7,"Mid pan constant power");
            if(pan==-50)require(std::abs(r)<1e-7,"Mid moves fully left");
            if(pan==50)require(std::abs(l)<1e-7,"Mid moves fully right");
        }
        MasteringCore core;core.prepare(fs);Settings s;
        for(int i=0;i<100000;++i){if(i%97==0){s.profile=(i/97)%4;s.atmosphere=(i/97)%2?100:0;s.balance=(i/97)%3*50-50;core.update(s);}
            float l=static_cast<float>(.3*std::sin(i*.71)),r=static_cast<float>(.3*std::cos(i*.13));float* f[]{&l,&r};core.process(f,2,1);
            require(std::isfinite(l)&&std::isfinite(r)&&std::abs(l)<2&&std::abs(r)<2,"automated profiles and pan bounded");}
        Engine e;e.prepare(fs);const int latency=e.latency();EngineSettings es;es.tone.atmosphere=100;e.update(es);
        require(e.latency()==latency,"no added declared latency");
    }
    // Amount zero must keep an anti-phase impulse at sample zero.
    MasteringCore core;core.prepare(48000);Settings s;core.update(s);float l=.25f,r=-l;float* f[]{&l,&r};core.process(f,2,1);
    require(l==.25f&&r==-.25f,"zero amount is immediate and neutral");
    std::puts("PASS Atmosphere and Mid pan: four rates, bass, centre, profiles, automation, latency");
}
