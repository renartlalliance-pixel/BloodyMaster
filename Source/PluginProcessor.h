#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "DSP/MasteringChain.h"

/** Processeur de mastering Bloody Master. */
class BloodyMasterProcessor final : public juce::AudioProcessor
{
public:
    BloodyMasterProcessor();
    ~BloodyMasterProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Bloody Master"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    juce::AudioProcessorParameter* getBypassParameter() const override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

private:
    bm::MasteringChain masteringChain;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BloodyMasterProcessor)
};
