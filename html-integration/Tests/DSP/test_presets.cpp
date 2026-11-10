#include "Presets/FactoryPresets.h"
#include <cstdio>
#include <cstdlib>
#include <set>
using namespace bm::dsp;
int main() {
    std::set<std::string_view> slugs;
    for(const auto& p:bm::factoryPresets()) {
        if(!slugs.insert(p.slug).second || bm::findPreset(p.slug)!=&p) return 1;
        std::set<std::string_view> ids;
        for(const auto& [id,value]:bm::presetParameters(p.settings))
            if(!ids.insert(id).second || !std::isfinite(value)) return 2;
        for(double fs:{44100.,48000.,96000.,192000.}) for(int os:{2,4,8}) {
            Engine e;e.prepare(fs,os);auto s=p.settings;s.oversampling=os;e.update(s);
            float l[257],r[257];float* data[]{l,r};
            for(int block=0;block<12;++block) {
                for(int i=0;i<257;++i) {
                    const double t=(block*257+i)/fs;
                    l[i]=float(.8*std::sin(2*pi*60*t)+.5*std::sin(2*pi*1900*t));
                    r[i]=float(.7*std::sin(2*pi*60*t)+.4*std::sin(2*pi*7500*t));
                }
                e.process(data,2,257);
                for(int i=0;i<257;++i)
                    if(!std::isfinite(l[i]) || !std::isfinite(r[i]) || std::max(std::abs(l[i]),std::abs(r[i]))>dbGain(s.peaks.ceilingDb)+1.e-6) return 3;
            }
        }
    }
    std::puts("PASS 9 presets x 4 rates x 3 oversampling modes (108 stereo renders)");
}
