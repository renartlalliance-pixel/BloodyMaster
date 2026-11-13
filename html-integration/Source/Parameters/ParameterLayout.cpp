#include "ParameterLayout.h"
#include "ParameterIDs.h"

namespace bm
{
// Plages et valeurs par defaut extraites de l'interface HTML d'origine (data-min / data-max / data-val).
// Noms = libelles de l'interface HTML. Mapping lineaire, identique au glisser du HTML. Les mappings non lineaires (frequences) viendront avec le DSP.
juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    using namespace juce;
    using PID = ParameterID;
    constexpr int v = id::kVersion;
    std::vector<std::unique_ptr<RangedAudioParameter>> p;

    auto addFloat = [&](const char* pid, const char* name, float lo, float hi, float def, const char* unit)
    {
        p.push_back(std::make_unique<AudioParameterFloat>(PID{ pid, v }, name, NormalisableRange<float>(lo, hi), def,
                                                          AudioParameterFloatAttributes().withLabel(unit)));
    };
    auto addBool = [&](const char* pid, const char* name, bool def)
    {
        p.push_back(std::make_unique<AudioParameterBool>(PID{ pid, v }, name, def));
    };

    addFloat(id::defreq, "Harsh Freq", 4.0f, 12.0f, 7.5f, "kHz");
    addFloat(id::deamount, "De-Harsh", 0.0f, 100.0f, 25.0f, "%");
    addFloat(id::lowres, "Mud Trap", 80.0f, 400.0f, 160.0f, "Hz");
    addFloat(id::hires, "Ring Trap", 2.0f, 10.0f, 4.2f, "kHz");
    addFloat(id::hpf, "Sub Barrier (HPF)", 15.0f, 100.0f, 28.0f, "Hz");
    addFloat(id::lpf, "Radio Ceil (LPF)", 18.0f, 45.0f, 38.0f, "kHz");
    addFloat(id::xl, "XL", 0.0f, 100.0f, 12.0f, "%");
    addFloat(id::thd, "THD", -60.0f, -20.0f, -42.0f, "dB");
    addFloat(id::bass, "Sub Weight", -6.0f, 6.0f, 0.8f, "dB");
    addFloat(id::mid, "Punch Body", -6.0f, 6.0f, -0.4f, "dB");
    addFloat(id::high, "Silk Shine", -6.0f, 6.0f, 1.2f, "dB");
    addFloat(id::presence, "Air Diffuse", -6.0f, 6.0f, 0.6f, "dB");
    addFloat(id::p1f, "Band I Low Freq", 30.0f, 400.0f, 120.0f, "Hz");
    addFloat(id::p1g, "Band I Low Gain", -6.0f, 6.0f, 0.0f, "dB");
    addFloat(id::p2f, "Band II Mid Freq", 400.0f, 4000.0f, 1800.0f, "Hz");
    addFloat(id::p2g, "Band II Mid Gain", -6.0f, 6.0f, -0.8f, "dB");
    addFloat(id::p3f, "Band III Air Freq", 4000.0f, 18000.0f, 12000.0f, "Hz");
    addFloat(id::p3g, "Band III Air Gain", -6.0f, 6.0f, 0.5f, "dB");
    addFloat(id::drive, "Blood Drive", 0.0f, 12.0f, 2.5f, "dB");
    addFloat(id::link, "Dual Link", 0.0f, 100.0f, 100.0f, "%");
    addFloat(id::mix, "Density Blend", 0.0f, 100.0f, 100.0f, "%");
    addFloat(id::squash, "Squash", 1.2f, 8.0f, 2.4f, ":1");
    addFloat(id::recovery, "Recovery", 10.0f, 1200.0f, 85.0f, "ms");
    addFloat(id::glue, "Tape Glue", 0.0f, 6.0f, 1.5f, "dB");
    addFloat(id::clipper, "Crush Clip", 0.0f, 10.0f, 2.0f, "");
    addFloat(id::monomaker, "Mono Base", 0.0f, 400.0f, 135.0f, "Hz");
    addFloat(id::width, "Horizon Width", 0.0f, 200.0f, 110.0f, "%");
    addFloat(id::enhance, "Atmosphere", 0.0f, 100.0f, 15.0f, "%");
    addFloat(id::pan, "Mid Bias", -50.0f, 50.0f, 0.0f, "");
    addFloat(id::foundation, "Gravity", -10.0f, 10.0f, 0.0f, "");
    addFloat(id::trim, "Ceiling Out", -6.0f, 0.0f, -0.2f, "dB");
    addFloat(id::turbo, "Hammer", 0.0f, 6.0f, 1.2f, "dB");
    addFloat(id::limrel, "Limiter Release", 1.0f, 500.0f, 12.0f, "ms");
    addFloat(id::limlink, "LR Unify", 0.0f, 100.0f, 100.0f, "%");

    addBool(id::bypass, "Bypass", false);
    addBool(id::valve, "Valve (Pure = off)", true);
    addBool(id::isp, "ISP Guard", true);
    addBool(id::eqActive, "EQ Active", true);
    addBool(id::spikeListen, "Spike Listen", false);
    p.push_back(std::make_unique<AudioParameterChoice>(PID{ id::profile, v }, "M/S Profile", StringArray{ "1", "2", "3", "4" }, 1));
    p.push_back(std::make_unique<AudioParameterChoice>(PID{ id::oversampling, v }, "Oversampling", StringArray{ "2x", "4x", "8x" }, 1));
    p.push_back(std::make_unique<AudioParameterChoice>(PID{ id::lufsTarget, v }, "LUFS Target", StringArray{ "-8", "-9", "-11", "-14", "-6" }, 0));

    addFloat(id::mudAmount, "Mud Trap Amount", 0.0f, 100.0f, 25.0f, "%");
    addBool(id::mudActive, "Mud Trap Active", false);
    addFloat(id::ringAmount, "Ring Trap Amount", 0.0f, 100.0f, 25.0f, "%");
    addBool(id::ringActive, "Ring Trap Active", false);
    p.push_back(std::make_unique<AudioParameterChoice>(PID{ id::thdMode, v }, "THD Color", StringArray{ "Warm", "Dense" }, 0));
    p.push_back(std::make_unique<AudioParameterChoice>(PID{ id::xlMode, v }, "XL Mode", StringArray{ "Density", "Presence", "Impact" }, 0));
    addFloat(id::tapeColor, "Tape Color", 0.0f, 100.0f, 0.0f, "%");
    p.push_back(std::make_unique<AudioParameterChoice>(PID{ id::tapeMode, v }, "Tape Character", StringArray{ "Discreet", "Marked" }, 0));
    addFloat(id::inputGain, "Input Gain", -18.0f, 18.0f, 0.0f, "dB");
    addFloat(id::loudnessWar, "LoudnessWar", 0.0f, 18.0f, 0.0f, "dB");
    return { p.begin(), p.end() };
}
} // namespace bm
