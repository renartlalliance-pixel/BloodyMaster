#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <array>
#include <atomic>

namespace bm
{
class MasteringChain
{
public:
    explicit MasteringChain(juce::AudioProcessorValueTreeState&);

    void prepare(double sampleRate) noexcept;
    void reset() noexcept;
    void process(juce::AudioBuffer<float>&) noexcept;

private:
    struct Biquad
    {
        float b0 = 1.0f, b1 = 0.0f, b2 = 0.0f, a1 = 0.0f, a2 = 0.0f;
        float z1 = 0.0f, z2 = 0.0f;

        void set(float, float, float, float, float, float) noexcept;
        float process(float) noexcept;
    };

    enum Parameter : size_t
    {
        defreq, deamount, lowres, hires, hpf, lpf, xl, thd, bass, mid, high,
        presence, p1f, p1g, p2f, p2g, p3f, p3g, drive, link, mix, squash,
        recovery, glue, clipper, monomaker, width, enhance, pan, foundation,
        trim, turbo, limrel, limlink, bypass, valve, isp, eqActive, spikeListen,
        profile, oversampling, lufsTarget, parameterCount
    };

    struct ChannelState
    {
        std::array<Biquad, 15> filters;
        float compressorEnvelope = 0.0f;
        float limiterGain = 1.0f;
        float previousNonlinearSample = 0.0f;
    };

    std::array<std::atomic<float>*, parameterCount> parameters {};
    std::array<ChannelState, 2> channels;
    double sampleRate = 44100.0;
    float monoSideLowState = 0.0f;
};
}
