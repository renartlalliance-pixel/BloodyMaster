#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "DSP/Engine.h"
#include <array>
#include <atomic>
#include <optional>

/** M3 alpha: tonal DSP, stereo dynamics, oversampled clipping and lookahead limiting. */
class BloodyMasterProcessor final : public juce::AudioProcessor
{
public:
    BloodyMasterProcessor();
    ~BloodyMasterProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void processBlockBypassed(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Bloody Master"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.02; }

    int getNumPrograms() override;
    int getCurrentProgram() override { return currentProgram.load(std::memory_order_relaxed); }
    void setCurrentProgram(int) override;
    const juce::String getProgramName(int) override;
    void changeProgramName(int, const juce::String&) override {}

    juce::AudioProcessorParameter* getBypassParameter() const override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;
    bm::dsp::MeterValues getMeterValues() const;
    struct ScopeFrame { std::array<std::array<float,256>,4> channels{}; int samples=0; };
    std::optional<ScopeFrame> getScopeFrame() const;
    double getDspSampleRate() const { return preparedRate.load(std::memory_order_relaxed); }
    bm::dsp::Settings getToneSettingsForUi() const { return readDspSettings().tone; }

private:
    std::atomic<int> currentProgram{0};
    std::array<std::atomic<float>,12> meterValues{};
    std::array<std::array<std::atomic<float>,256>,4> scope{};
    std::atomic<unsigned> scopeSequence{0};
    std::atomic<int> scopeSamples{0};
    std::atomic<double> preparedRate{0};
    void captureScope(const juce::AudioBuffer<float>&,bool input);
    void publishMeters();
    bm::dsp::Engine mastering;
    std::array<std::atomic<float>*, 32> dspParameters{};
    bm::dsp::EngineSettings readDspSettings() const;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BloodyMasterProcessor)
};
