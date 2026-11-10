#include "MasteringChain.h"
#include "Parameters/ParameterIDs.h"
#include "Utilities/DbUtils.h"

#include <algorithm>
#include <cmath>

namespace bm
{
namespace
{
constexpr float pi = 3.14159265358979323846f;

enum FilterIndex
{
    highPass, lowPass, lowResonance, highResonance, xlShelf, bassShelf,
    midBell, highShelf, presenceShelf, parametric1, parametric2, parametric3,
    deHarsh, listenHighPass, listenLowPass
};

float finiteOr(float value, float fallback) noexcept
{
    return std::isfinite(value) ? value : fallback;
}

}

MasteringChain::MasteringChain(juce::AudioProcessorValueTreeState& state)
{
    constexpr std::array<const char*, parameterCount> ids
    {{
        bm::id::defreq, bm::id::deamount, bm::id::lowres, bm::id::hires, bm::id::hpf, bm::id::lpf,
        bm::id::xl, bm::id::thd, bm::id::bass, bm::id::mid, bm::id::high, bm::id::presence,
        bm::id::p1f, bm::id::p1g, bm::id::p2f, bm::id::p2g, bm::id::p3f, bm::id::p3g,
        bm::id::drive, bm::id::link, bm::id::mix, bm::id::squash, bm::id::recovery, bm::id::glue,
        bm::id::clipper, bm::id::monomaker, bm::id::width, bm::id::enhance, bm::id::pan, bm::id::foundation,
        bm::id::trim, bm::id::turbo, bm::id::limrel, bm::id::limlink, bm::id::bypass, bm::id::valve,
        bm::id::isp, bm::id::eqActive, bm::id::spikeListen, bm::id::profile, bm::id::oversampling, bm::id::lufsTarget
    }};

    for (size_t i = 0; i < ids.size(); ++i)
        parameters[i] = state.getRawParameterValue(ids[i]);
}

void MasteringChain::Biquad::set(float nb0, float nb1, float nb2, float na0, float na1, float na2) noexcept
{
    if (!std::isfinite(na0) || std::abs(na0) < 1.0e-12f)
        return;
    const auto inverseA0 = 1.0f / na0;
    b0 = nb0 * inverseA0;
    b1 = nb1 * inverseA0;
    b2 = nb2 * inverseA0;
    a1 = na1 * inverseA0;
    a2 = na2 * inverseA0;
}

float MasteringChain::Biquad::process(float x) noexcept
{
    const auto y = b0 * x + z1;
    z1 = b1 * x - a1 * y + z2;
    z2 = b2 * x - a2 * y;
    if (!std::isfinite(y) || !std::isfinite(z1) || !std::isfinite(z2))
    {
        z1 = z2 = 0.0f;
        return 0.0f;
    }
    return y;
}

void MasteringChain::prepare(double newSampleRate) noexcept
{
    sampleRate = std::isfinite(newSampleRate) && newSampleRate > 1000.0 ? newSampleRate : 44100.0;
    reset();
}

void MasteringChain::reset() noexcept
{
    monoSideLowState = 0.0f;
    for (auto& channel : channels)
    {
        channel = {};
    }
}

void MasteringChain::process(juce::AudioBuffer<float>& buffer) noexcept
{
    const auto samples = buffer.getNumSamples();
    const auto numChannels = juce::jmin(buffer.getNumChannels(), 2);
    if (samples == 0 || numChannels == 0)
        return;

    std::array<float, parameterCount> p {};
    for (size_t i = 0; i < p.size(); ++i)
        p[i] = parameters[i] != nullptr ? parameters[i]->load(std::memory_order_relaxed) : 0.0f;

    const auto sr = static_cast<float>(sampleRate);
    const auto nyquistSafe = sr * 0.45f;
    const auto clampFrequency = [nyquistSafe](float frequency)
    {
        return juce::jlimit(5.0f, nyquistSafe, finiteOr(frequency, 1000.0f));
    };

    const auto setFilter = [sr, &clampFrequency](Biquad& filter, int type, float frequency, float q, float gainDb)
    {
        const auto f = clampFrequency(frequency);
        const auto w = 2.0f * pi * f / sr;
        const auto c = std::cos(w);
        const auto s = std::sin(w);
        const auto alpha = s / (2.0f * juce::jmax(0.1f, q));
        float b0 = 1.0f, b1 = 0.0f, b2 = 0.0f, a0 = 1.0f, a1 = 0.0f, a2 = 0.0f;

        if (type == 0) // high-pass
        {
            b0 = (1.0f + c) * 0.5f; b1 = -(1.0f + c); b2 = b0;
            a0 = 1.0f + alpha; a1 = -2.0f * c; a2 = 1.0f - alpha;
        }
        else if (type == 1) // low-pass
        {
            b0 = (1.0f - c) * 0.5f; b1 = 1.0f - c; b2 = b0;
            a0 = 1.0f + alpha; a1 = -2.0f * c; a2 = 1.0f - alpha;
        }
        else if (type == 2) // peaking EQ
        {
            const auto a = std::pow(10.0f, gainDb / 40.0f);
            b0 = 1.0f + alpha * a; b1 = -2.0f * c; b2 = 1.0f - alpha * a;
            a0 = 1.0f + alpha / a; a1 = -2.0f * c; a2 = 1.0f - alpha / a;
        }
        else // low/high shelf
        {
            const auto a = std::pow(10.0f, gainDb / 40.0f);
            const auto beta = 2.0f * std::sqrt(a) * alpha;
            if (type == 3)
            {
                b0 = a * ((a + 1.0f) - (a - 1.0f) * c + beta);
                b1 = 2.0f * a * ((a - 1.0f) - (a + 1.0f) * c);
                b2 = a * ((a + 1.0f) - (a - 1.0f) * c - beta);
                a0 = (a + 1.0f) + (a - 1.0f) * c + beta;
                a1 = -2.0f * ((a - 1.0f) + (a + 1.0f) * c);
                a2 = (a + 1.0f) + (a - 1.0f) * c - beta;
            }
            else
            {
                b0 = a * ((a + 1.0f) + (a - 1.0f) * c + beta);
                b1 = -2.0f * a * ((a - 1.0f) + (a + 1.0f) * c);
                b2 = a * ((a + 1.0f) + (a - 1.0f) * c - beta);
                a0 = (a + 1.0f) - (a - 1.0f) * c + beta;
                a1 = 2.0f * ((a - 1.0f) - (a + 1.0f) * c);
                a2 = (a + 1.0f) - (a - 1.0f) * c - beta;
            }
        }
        filter.set(b0, b1, b2, a0, a1, a2);
    };

    const auto deFreq = p[defreq] * 1000.0f;
    const auto xlGain = p[xl] * 0.025f;
    for (auto& channel : channels)
    {
        auto& f = channel.filters;
        setFilter(f[highPass], 0, p[hpf], 0.707f, 0.0f);
        setFilter(f[lowPass], 1, p[lpf] * 1000.0f, 0.707f, 0.0f);
        setFilter(f[lowResonance], 2, p[lowres], 2.0f, -2.5f);
        setFilter(f[highResonance], 2, p[hires] * 1000.0f, 2.5f, -2.5f);
        setFilter(f[xlShelf], 3, 70.0f, 0.707f, xlGain);
        setFilter(f[bassShelf], 3, 120.0f, 0.707f, p[bass] + p[foundation] * 0.5f);
        setFilter(f[midBell], 2, 1000.0f, 0.7f, p[mid]);
        setFilter(f[highShelf], 4, 6000.0f, 0.707f, p[high]);
        setFilter(f[presenceShelf], 4, 14000.0f, 0.707f, p[presence]);
        setFilter(f[parametric1], 2, p[p1f], 1.0f, p[p1g]);
        setFilter(f[parametric2], 2, p[p2f], 1.0f, p[p2g]);
        setFilter(f[parametric3], 2, p[p3f], 1.0f, p[p3g]);
        setFilter(f[deHarsh], 2, deFreq, 2.0f, -p[deamount] * 0.06f);
        setFilter(f[listenHighPass], 0, deFreq * 0.72f, 0.707f, 0.0f);
        setFilter(f[listenLowPass], 1, deFreq * 1.38f, 0.707f, 0.0f);
    }

    const auto eqEnabled = p[eqActive] > 0.5f;
    const auto listen = eqEnabled && p[spikeListen] > 0.5f;
    const auto mixAmount = juce::jlimit(0.0f, 1.0f, p[mix] * 0.01f);
    const auto linkAmount = juce::jlimit(0.0f, 1.0f, p[link] * 0.01f);
    const auto limiterLink = juce::jlimit(0.0f, 1.0f, p[limlink] * 0.01f);
    const auto ratio = juce::jmax(1.0f, p[squash]);
    constexpr std::array<float, 5> targetLufs { -8.0f, -9.0f, -11.0f, -14.0f, -6.0f };
    const auto targetIndex = juce::jlimit(0, 4, juce::roundToInt(p[lufsTarget]));
    const auto threshold = targetLufs[static_cast<size_t>(targetIndex)] - 10.0f - p[glue];
    const auto compressorRelease = std::exp(-1.0f / (sr * juce::jmax(0.01f, p[recovery] * 0.001f)));
    const auto limiterRelease = std::exp(-1.0f / (sr * juce::jmax(0.001f, p[limrel] * 0.001f)));
    const auto attack = std::exp(-1.0f / (sr * 0.003f));
    const auto driveGain = bm::util::dbToGain(p[drive]);
    const auto turboGain = bm::util::dbToGain(p[turbo]);
    const auto outputCeiling = bm::util::dbToGain(p[trim]) * (p[isp] > 0.5f ? 0.96f : 1.0f);
    const auto clipThreshold = juce::jlimit(0.5f, 1.0f, 1.0f - p[clipper] * 0.045f);
    const auto thdAmount = juce::jlimit(0.0f, 1.0f, (p[thd] + 60.0f) / 40.0f);
    const auto valveDrive = 1.0f + 3.0f * thdAmount;
    const auto oversample = 1 << (juce::jlimit(0, 2, juce::roundToInt(p[oversampling])) + 1);
    const auto valveEnabled = p[valve] > 0.5f;
    const auto clipEnabled = p[clipper] > 0.001f;
    const auto widthProfile = p[profile] < 0.5f ? 1.0f
                            : p[profile] < 1.5f ? 1.1f
                            : p[profile] < 2.5f ? 0.9f : 0.0f;
    const auto sideGain = juce::jlimit(0.0f, 2.0f,
        p[width] * 0.01f * widthProfile * (1.0f + p[enhance] * 0.003f));
    const auto monoCutoff = juce::jlimit(0.0f, 400.0f, p[monomaker]);
    const auto monoCoefficient = monoCutoff > 0.0f
        ? 1.0f - std::exp(-2.0f * pi * monoCutoff / sr) : 0.0f;
    const auto panAmount = juce::jlimit(-1.0f, 1.0f, p[pan] * 0.02f);

    auto* left = buffer.getWritePointer(0);
    auto* right = numChannels > 1 ? buffer.getWritePointer(1) : nullptr;

    for (int i = 0; i < samples; ++i)
    {
        const auto dryL = left[i];
        const auto dryR = right != nullptr ? right[i] : dryL;
        const std::array<float, 2> dry { dryL, dryR };
        std::array<float, 2> wet = dry;

        for (int ch = 0; ch < numChannels; ++ch)
        {
            auto& channel = channels[static_cast<size_t>(ch)];
            auto& f = channel.filters;
            auto x = finiteOr(wet[static_cast<size_t>(ch)], 0.0f);

            if (listen)
            {
                x = f[listenLowPass].process(f[listenHighPass].process(x));
            }
            else if (eqEnabled)
            {
                x = f[highPass].process(x);
                x = f[lowPass].process(x);
                x = f[lowResonance].process(x);
                x = f[highResonance].process(x);
                x = f[xlShelf].process(x);
                x = f[bassShelf].process(x);
                x = f[midBell].process(x);
                x = f[highShelf].process(x);
                x = f[presenceShelf].process(x);
                x = f[parametric1].process(x);
                x = f[parametric2].process(x);
                x = f[parametric3].process(x);
                x = f[deHarsh].process(x);
            }

            x *= driveGain;
            const auto envelope = std::abs(x);
            const auto coefficient = envelope > channel.compressorEnvelope ? attack : compressorRelease;
            channel.compressorEnvelope = coefficient * channel.compressorEnvelope + (1.0f - coefficient) * envelope;
            wet[static_cast<size_t>(ch)] = x;
        }

        if (numChannels > 1)
        {
            const auto linkedEnvelope = juce::jmax(channels[0].compressorEnvelope, channels[1].compressorEnvelope);
            for (int ch = 0; ch < 2; ++ch)
            {
                auto& state = channels[static_cast<size_t>(ch)];
                const auto detector = state.compressorEnvelope * (1.0f - linkAmount) + linkedEnvelope * linkAmount;
                const auto levelDb = bm::util::gainToDb(detector);
                if (levelDb > threshold)
                {
                    const auto compressedDb = threshold + (levelDb - threshold) / ratio - levelDb;
                    wet[static_cast<size_t>(ch)] *= bm::util::dbToGain(compressedDb);
                }
            }
        }
        else
        {
            const auto levelDb = bm::util::gainToDb(channels[0].compressorEnvelope);
            if (levelDb > threshold)
                wet[0] *= bm::util::dbToGain(threshold + (levelDb - threshold) / ratio - levelDb);
        }

        if (listen)
        {
            wet[0] *= 8.0f;
            if (numChannels > 1)
                wet[1] *= 8.0f;
        }

        for (int ch = 0; ch < numChannels; ++ch)
        {
            auto& state = channels[static_cast<size_t>(ch)];
            const auto current = wet[static_cast<size_t>(ch)];
            float shaped = 0.0f;
            for (int step = 1; step <= oversample; ++step)
            {
                auto sample = state.previousNonlinearSample
                            + (current - state.previousNonlinearSample) * (static_cast<float>(step) / oversample);
                if (valveEnabled)
                    sample = std::tanh(sample * valveDrive) / std::tanh(valveDrive);
                if (clipEnabled)
                    sample = std::tanh(sample / clipThreshold) * clipThreshold / std::tanh(1.0f / clipThreshold);
                shaped += sample;
            }
            state.previousNonlinearSample = current;
            wet[static_cast<size_t>(ch)] = shaped / static_cast<float>(oversample);
        }

        if (numChannels > 1)
        {
            const auto midSignal = (wet[0] + wet[1]) * 0.5f;
            auto sideSignal = (wet[0] - wet[1]) * 0.5f;
            monoSideLowState += monoCoefficient * (sideSignal - monoSideLowState);
            sideSignal -= monoSideLowState * 0.85f;
            sideSignal *= sideGain;
            wet[0] = midSignal + sideSignal;
            wet[1] = midSignal - sideSignal;
            if (panAmount > 0.0f)
                wet[0] *= 1.0f - panAmount;
            else if (panAmount < 0.0f)
                wet[1] *= 1.0f + panAmount;
        }

        for (int ch = 0; ch < numChannels; ++ch)
            wet[static_cast<size_t>(ch)] = dry[static_cast<size_t>(ch)]
                + (wet[static_cast<size_t>(ch)] - dry[static_cast<size_t>(ch)]) * mixAmount;

        wet[0] *= turboGain;
        if (numChannels > 1)
            wet[1] *= turboGain;

        const auto peakL = std::abs(wet[0]);
        const auto peakR = numChannels > 1 ? std::abs(wet[1]) : peakL;
        const auto commonPeak = juce::jmax(peakL, peakR);
        for (int ch = 0; ch < numChannels; ++ch)
        {
            auto& state = channels[static_cast<size_t>(ch)];
            const auto ownPeak = ch == 0 ? peakL : peakR;
            const auto detectedPeak = ownPeak * (1.0f - limiterLink) + commonPeak * limiterLink;
            const auto targetGain = detectedPeak > outputCeiling ? outputCeiling / detectedPeak : 1.0f;
            if (targetGain < state.limiterGain)
                state.limiterGain = targetGain;
            else
                state.limiterGain = limiterRelease * state.limiterGain + (1.0f - limiterRelease) * targetGain;

            wet[static_cast<size_t>(ch)] *= state.limiterGain;
            wet[static_cast<size_t>(ch)] = juce::jlimit(-outputCeiling, outputCeiling,
                                                         finiteOr(wet[static_cast<size_t>(ch)], 0.0f));
        }

        left[i] = wet[0];
        if (right != nullptr)
            right[i] = wet[1];
    }
}
}
