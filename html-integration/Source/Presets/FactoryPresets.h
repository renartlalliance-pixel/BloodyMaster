#pragma once
#include "DSP/Engine.h"
#include "Parameters/ParameterIDs.h"
#include <string_view>
#include <utility>

namespace bm {
struct FactoryPreset {
    const char* slug;
    const char* name;
    dsp::EngineSettings settings;
};
// Original starting points, not settings supplied or endorsed by named engineers.
inline const std::array<FactoryPreset,9>& factoryPresets() {
    static const auto bank=[] {
        std::array<FactoryPreset,9> p{};
        const auto base=[] {
            dsp::EngineSettings s;
            s.tone.hpf=15; s.dynamics.ratio=1.5; s.dynamics.releaseMs=150;
            s.peaks.ceilingDb=-1; s.peaks.releaseMs=100; s.oversampling=8;
            return s;
        };
        p[0]={"clean","Clean",base()};
        p[1]={"glue","Glue",base()};
        auto& g=p[1].settings; g.dynamics.drive=1;g.dynamics.ratio=2;g.dynamics.releaseMs=220;
        g.dynamics.mix=85;g.dynamics.glue=1.5;g.peaks.clipDb=.8;g.peaks.pushDb=1;
        p[2]={"punch","Punch",base()};
        auto& u=p[2].settings;u.dynamics.drive=2;u.dynamics.ratio=2.4;u.dynamics.mix=90;
        u.dynamics.glue=1;u.peaks.clipDb=2;u.peaks.pushDb=2;
        p[3]={"rap-sub-focus","Rap - Sub Focus",base()};
        auto& r=p[3].settings;r.tone.hpf=20;r.tone.bass=.8;r.tone.mid=-.3;
        r.tone.freq[0]=220;r.tone.gain[0]=-.7;r.tone.monoBass=100;
        r.dynamics.drive=1;r.dynamics.ratio=1.6;r.dynamics.releaseMs=160;
        r.dynamics.mix=85;r.dynamics.optical=false;r.peaks.clipDb=1;r.peaks.pushDb=.8;
        p[4]={"rap-forward","Rap - Forward",base()};
        auto& f=p[4].settings;f.tone.hpf=23;f.tone.bass=.4;f.tone.high=.3;
        f.tone.freq[1]=1800;f.tone.gain[1]=.4;f.tone.monoBass=110;f.tone.width=103;
        f.dynamics.drive=2;f.dynamics.ratio=2;f.dynamics.releaseMs=100;
        f.dynamics.mix=90;f.dynamics.optical=false;f.peaks.clipDb=1.8;
        f.peaks.pushDb=1.2;f.peaks.releaseMs=80;
        p[5]={"pop-open","Pop - Open",base()};
        auto& o=p[5].settings;o.tone.hpf=22;o.tone.bass=.3;o.tone.high=.4;
        o.tone.presence=.5;o.tone.monoBass=90;o.tone.width=104;
        o.dynamics.drive=.5;o.dynamics.ratio=1.5;o.dynamics.releaseMs=240;
        o.dynamics.mix=100;o.dynamics.glue=.3;o.peaks.clipDb=.4;o.peaks.pushDb=.4;
        p[6]={"pop-dense","Pop - Dense",base()};
        auto& d=p[6].settings;d.tone.hpf=25;d.tone.mid=-.3;d.tone.high=.5;
        d.tone.presence=.3;d.tone.monoBass=100;d.tone.width=105;
        d.dynamics.drive=1.5;d.dynamics.ratio=1.8;d.dynamics.releaseMs=180;
        d.dynamics.mix=100;d.dynamics.glue=.6;d.peaks.clipDb=1;d.peaks.pushDb=.8;
        p[7]={"electro-punch","Electro - Punch",base()};
        auto& e=p[7].settings;e.tone.hpf=20;e.tone.bass=.5;e.tone.high=.3;
        e.tone.monoBass=120;e.tone.width=106;e.dynamics.drive=1;
        e.dynamics.ratio=1.8;e.dynamics.releaseMs=100;e.dynamics.mix=80;
        e.dynamics.optical=false;e.peaks.clipDb=2;e.peaks.pushDb=1;e.peaks.releaseMs=70;
        p[8]={"electro-club","Electro - Club",base()};
        auto& c=p[8].settings;c.tone.hpf=25;c.tone.bass=.3;c.tone.mid=-.4;
        c.tone.high=.4;c.tone.monoBass=130;c.tone.width=108;c.dynamics.drive=2;
        c.dynamics.ratio=2;c.dynamics.releaseMs=80;c.dynamics.mix=85;
        c.dynamics.glue=.5;c.dynamics.optical=false;c.peaks.clipDb=3;
        c.peaks.pushDb=1.8;c.peaks.releaseMs=60;
        // Gentle starting points for the newly completed stereo/tape stages.
        r.peaks.color.tape=8;
        f.tone.atmosphere=8;f.tone.profile=1;f.peaks.color.tape=10;
        o.tone.atmosphere=15;o.tone.profile=0;o.peaks.color.tape=5;
        d.tone.atmosphere=10;d.tone.profile=3;d.peaks.color.tape=12;
        e.tone.atmosphere=8;e.tone.profile=2;e.peaks.color.tape=6;
        c.tone.atmosphere=10;c.tone.profile=0;c.peaks.color.tape=10;
        for(auto& preset:p)preset.settings.peaks.color.valve=preset.settings.dynamics.optical;
        return p;
    }();
    return bank;
}
inline const FactoryPreset* findPreset(std::string_view slug) {
    for(const auto& p:factoryPresets()) if(slug==p.slug) return &p;
    return nullptr;
}
// Physical APVTS values. This mapping also documents the CLI/plugin agreement.
inline std::array<std::pair<const char*,float>,49> presetParameters(const dsp::EngineSettings& s) {
    const auto& t=s.tone;const auto& d=s.dynamics;const auto& p=s.peaks;
    return {{{id::hpf,float(t.hpf)},{id::lpf,float(t.lpf/1000)},
      {id::bass,float(t.bass)},{id::mid,float(t.mid)},{id::high,float(t.high)},
      {id::presence,float(t.presence)},{id::foundation,float(t.body)},
      {id::p1f,float(t.freq[0])},{id::p2f,float(t.freq[1])},{id::p3f,float(t.freq[2])},
      {id::p1g,float(t.gain[0])},{id::p2g,float(t.gain[1])},{id::p3g,float(t.gain[2])},
      {id::monomaker,float(t.monoBass)},{id::width,float(t.width)},{id::pan,float(t.balance)},
      {id::eqActive,float(t.eq)},{id::bypass,float(s.bypass)},
      {id::drive,float(d.drive)},{id::squash,float(d.ratio)},{id::recovery,float(d.releaseMs)},
      {id::link,float(d.link)},{id::mix,float(d.mix)},{id::glue,float(d.glue)},
      {id::valve,float(d.optical)},{id::clipper,float(p.clipDb)},{id::turbo,float(p.pushDb)},
      {id::trim,float(p.ceilingDb)},{id::limrel,float(p.releaseMs)},{id::limlink,float(p.link)},
      {id::isp,float(p.isp)},{id::oversampling,float(s.oversampling==2?0:s.oversampling==4?1:2)},
      {id::defreq,float(s.cleanup.harshHz/1000)},{id::deamount,float(s.cleanup.amount)},
      {id::lowres,float(s.cleanup.mudHz)},{id::hires,float(s.cleanup.ringHz/1000)},
      {id::spikeListen,float(s.cleanup.listen)},{id::mudAmount,float(s.cleanup.mudAmount)},
      {id::mudActive,float(s.cleanup.mudEnabled)},{id::ringAmount,float(s.cleanup.ringAmount)},
      {id::ringActive,float(s.cleanup.ringEnabled)},
      {id::thd,float(p.color.thdDb)},{id::xl,float(p.color.xl)},
      {id::thdMode,float(p.color.thdMode)},{id::xlMode,float(p.color.xlMode)},
      {id::tapeColor,float(p.color.tape)},{id::tapeMode,float(p.color.tapeMode)},
      {id::enhance,float(t.atmosphere)},{id::profile,float(t.profile)}}};
}
}
