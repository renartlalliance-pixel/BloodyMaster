#include "DSP/Engine.h"
#include <cstdio>
#include <cstdlib>
using namespace bm::dsp;
static int checks=0;
void check(bool ok,const char* message){++checks;if(!ok){std::fprintf(stderr,"FAIL %s\n",message);std::exit(1);}}
double render(double fs,double hz,CleanupSettings s) {
    DynamicCleanup clean;clean.prepare(fs);clean.update(s);double input=0,output=0;
    const int n=static_cast<int>(fs*.5);
    for(int i=0;i<n;++i){const double x=.5*std::sin(2*pi*hz*i/fs);double l=x,r=x*.2;clean.process(l,r);
        check(std::isfinite(l)&&std::isfinite(r),"finite");check(std::abs(r-.2*l)<1.e-12,"linked stereo ratio");
        check(std::abs(x-l-clean.removed[0])<1.e-14,"delta reconstructs input");
        if(i>n/2){input+=x*x;output+=l*l;}
    }
    return 10*std::log10(output/input);
}
int main(){
    CleanupSettings s;s.enabled=true;s.amount=100;s.mudEnabled=s.ringEnabled=true;s.mudAmount=s.ringAmount=100;s.harshHz=7500;s.ringHz=4200;s.mudHz=160;
    for(double fs:{44100.,48000.,96000.,192000.}){
        const double harsh=render(fs,7500,s),mud=render(fs,160,s),ring=render(fs,4200,s),pass=render(fs,1000,s);
        check(harsh<-8 && harsh>-12.15,"harsh attenuation, shared 12 dB budget");
        check(mud<-8 && mud>-12.15,"mud attenuation");check(ring<-8 && ring>-12.15,"ring attenuation");
        check(pass>-.8,"unrelated midrange preserved");
        s.amount=s.mudAmount=s.ringAmount=0;check(std::abs(render(fs,7500,s))<1.e-12,"amount zero exact unity");s.amount=s.mudAmount=s.ringAmount=100;
        auto overlap=s;overlap.harshHz=overlap.ringHz=6000;
        check(render(fs,6000,overlap)>-12.15,"overlap shares depth budget");
        DynamicCleanup clean;clean.prepare(fs);clean.update(s);
        for(int i=0;i<3000;++i){double l=0,r=0;clean.process(l,r);check(l==0&&r==0,"silence");}
        for(int b=0;b<100;++b){auto automated=s;automated.harshHz=b%2?4000:12000;automated.mudHz=b%2?80:400;automated.ringHz=b%2?2000:10000;clean.update(automated);
            for(int i=0;i<64;++i){double l=.3*std::sin((b*64+i)*.7),r=.2*std::cos((b*64+i)*.2);clean.process(l,r);check(std::isfinite(l)&&std::isfinite(r)&&std::abs(l)<2&&std::abs(r)<2,"frequency automation stable");}}
        std::printf("Fs %.0f: harsh %.3f dB, mud %.3f dB, ring %.3f dB, pass %.3f dB\n",fs,harsh,mud,ring,pass);
    }
    auto isolated=s;isolated.amount=0;isolated.mudEnabled=isolated.ringEnabled=false;
    check(std::abs(render(48000,160,isolated))<1.e-12,"both traps off exact unity");
    isolated.mudEnabled=true;
    check(render(48000,160,isolated)<-10,"mud independent activation");
    check(render(48000,4200,isolated)>-.1,"mud does not cut ring region");
    isolated.mudEnabled=false;isolated.ringEnabled=true;
    check(render(48000,4200,isolated)<-10,"ring independent activation");
    check(render(48000,160,isolated)>-.1,"ring does not cut mud region");
    isolated.ringAmount=0;
    check(std::abs(render(48000,4200,isolated))<1.e-12,"ring zero amount exact unity");
    isolated=s;isolated.enabled=false;
    check(std::abs(render(48000,7500,isolated))<1.e-12,"global cleanup disabled exact unity");
    DynamicCleanup quiet;quiet.prepare(48000);quiet.update(s);
    for(int i=0;i<48000;++i){double l=1.e-5*std::sin(2*pi*7500*i/48000),r=l,x=l;quiet.process(l,r);check(l==x&&r==x,"quiet signal below gate unchanged");}
    // Audition must be the pre-EQ signal removed, delayed to the declared latency.
    EngineSettings engineSettings;engineSettings.cleanup=s;engineSettings.cleanup.listen=true;
    Engine engine;engine.prepare(48000,8);engine.update(engineSettings);
    DynamicCleanup reference;reference.prepare(48000);reference.update(s);
    const int latency=engine.latency(),length=4096;
    std::vector<float> audio(length+latency),expected(length+latency);
    for(int i=0;i<length;++i)audio[static_cast<size_t>(i)]=float(.5*std::sin(2*pi*7500*i/48000));
    for(int i=0;i<length;++i){double l=audio[static_cast<size_t>(i)],r=l;reference.process(l,r);expected[static_cast<size_t>(i+latency)]=static_cast<float>(reference.removed[0]);}
    float* data[]{audio.data()};engine.process(data,1,length+latency);
    for(int i=0;i<length+latency;++i)check(std::abs(audio[static_cast<size_t>(i)]-expected[static_cast<size_t>(i)])<1.e-7,"audition delta and latency");
    engineSettings.bypass=true;engine.prepare(48000,8);engine.update(engineSettings);
    std::fill(audio.begin(),audio.end(),0);std::fill(expected.begin(),expected.end(),0);
    for(int i=0;i<length;++i){audio[static_cast<size_t>(i)]=float(.5*std::sin(2*pi*7500*i/48000));expected[static_cast<size_t>(i+latency)]=audio[static_cast<size_t>(i)];}
    engine.process(data,1,length+latency);
    check(audio==expected,"global bypass overrides listen and remains latency aligned");
    std::printf("PASS %d dynamic cleanup checks\n",checks);
}
