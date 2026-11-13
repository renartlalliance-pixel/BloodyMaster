#include "DSP/MonitorMatch.h"
#include <cstdio>
#include <cstdlib>
using namespace bm::dsp;
void require(bool ok,const char* message){if(!ok){std::fprintf(stderr,"FAIL %s\n",message);std::exit(1);}}
int main(){
 for(double fs:{44100.,48000.,96000.,192000.}){
    MonitorMatch match;match.prepare(fs);std::array<float,51> signature{};
    float l[512],r[512];float* data[]{l,r};
    auto feed=[&](double lu,bool enabled,bool offline,int blocks){for(int block=0;block<blocks;++block){std::fill_n(l,512,.5f);std::fill_n(r,512,-.25f);match.process(data,2,512,lu,enabled,offline);}};
    const int learn=static_cast<int>(std::ceil(fs*3/512))+1;
    match.select(0,signature);feed(-14,false,false,learn);require(!match.ready&&l[511]==.5f,"default transparent and needs both snapshots");
    match.select(1,signature);feed(-8,true,false,learn+static_cast<int>(fs/512));
    require(match.ready&&match.gainDb==-6,"learn both snapshots and attenuate louder");
    require(std::abs(l[511]-.5*std::pow(10.,-6./20))<1e-6&&r[511]==-.5f*l[511],"stereo gain linked and correct");
    feed(-8,true,true,1);require(l[0]==.5f&&l[511]==.5f&&match.gainDb==0,"offline render immediately unity");
    match.select(0,signature);feed(-14,true,false,learn);require(match.gainDb==0&&l[511]==.5f,"quieter snapshot never boosted");
    match.select(1,signature);feed(-8,true,false,learn+static_cast<int>(fs/512));feed(-8,false,false,static_cast<int>(fs/512)+1);
    require(l[511]==.5f,"disabled gain returns to unity");
    signature[0]=1;match.select(1,signature);feed(-8,true,false,1);require(!match.ready,"editing snapshot invalidates learned level");
    match.reset();require(!match.ready&&match.gainDb==0,"reset clears learning");
 }
 std::puts("PASS monitor A/B: learning, attenuation, stereo, offline unity, disable, edits and reset at four rates");
}
