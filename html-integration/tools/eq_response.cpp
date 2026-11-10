#include "DSP/MasteringCore.h"
#include <complex>
#include <iostream>
using namespace bm::dsp;
double mag(Coefficients c,double fs,double hz) {
    const auto z=std::polar(1.,-2*pi*hz/fs);
    return std::abs((c.b0+c.b1*z+c.b2*z*z)/(1.+c.a1*z+c.a2*z*z));
}
int main() {
    const double fs=48000;
    std::cout<<"frequency_hz,low_plus3_db,low_plus6_db,high_plus3_db,high_plus6_db,air_minus3_db\n";
    for(int i=0;i<=400;++i) {
        const double hz=20*std::pow(1000.,i/400.);
        std::cout<<hz;
        for(double g:{3.,6.}) { const auto c=passiveLow(fs,g); std::cout<<','<<20*std::log10(mag(c[0],fs,hz)*mag(c[1],fs,hz)); }
        for(double g:{3.,6.}) std::cout<<','<<20*std::log10(mag(design(Type::peak,fs,8000,g,.55),fs,hz));
        std::cout<<','<<20*std::log10(mag(design(Type::highShelf,fs,12000,-3),fs,hz))<<'\n';
    }
}
