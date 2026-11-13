#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "DSP/Engine.h"
#include "DSP/AudioMeter.h"
#include "DSP/MonitorMatch.h"
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
    double getTailLengthSeconds() const override { return 0.1; }

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
    double getSafetyReductionDb() const { return safetyReduction.load(std::memory_order_relaxed); }
    bm::dsp::LoudnessValues getLoudnessValues() const;
    void setMonitorMatch(bool enabled) { monitorEnabled.store(enabled,std::memory_order_relaxed); }
    bool isMonitorMatchEnabled() const { return monitorEnabled.load(std::memory_order_relaxed); }
    void setComparisonIndex(int index) { comparisonIndex.store(index,std::memory_order_relaxed); }
    double getMonitorGainDb() const { return monitorGainDb.load(std::memory_order_relaxed); }
    bool isMonitorMatchReady() const { return monitorReady.load(std::memory_order_relaxed); }
    void requestMeterReset() { meterReset.store(true,std::memory_order_relaxed); }
    struct ScopeFrame { std::array<std::array<float,256>,4> channels{}; int samples=0; };
    std::optional<ScopeFrame> getScopeFrame() const;
    double getDspSampleRate() const { return preparedRate.load(std::memory_order_relaxed); }
    bm::dsp::Settings getToneSettingsForUi() const { return readDspSettings().tone; }

private:
    std::atomic<double> safetyReduction{0};
    std::atomic<int> currentProgram{0};
    std::array<std::atomic<float>,12> meterValues{};
    std::array<std::array<std::atomic<float>,256>,4> scope{};
    std::atomic<unsigned> scopeSequence{0};
    std::atomic<int> scopeSamples{0};
    std::atomic<double> preparedRate{0};
    void captureScope(const juce::AudioBuffer<float>&,bool input);
    void publishMeters();
    bm::dsp::AudioMeter outputMeter,matchMeter;
    bm::dsp::OutputSafety finalSafety;
    std::array<double,5> finalOutput{};
    std::atomic<bool> meterReset{false};
    std::array<std::atomic<double>,3> loudnessValues{{-200.,-200.,0.}};
    bm::dsp::MonitorMatch monitor;
    std::atomic<bool> monitorEnabled{false},monitorReady{false};
    std::atomic<int> comparisonIndex{0};
    std::atomic<double> monitorGainDb{0};
    void measureOutput(juce::AudioBuffer<float>&,bool bypass,double ceiling);
    bm::dsp::Engine mastering;
    std::array<std::atomic<float>*, 49> dspParameters{};
    bm::dsp::EngineSettings readDspSettings() const;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BloodyMasterProcessor)
};
