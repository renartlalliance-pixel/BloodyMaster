#pragma once
#include "PluginProcessor.h"
#if BM_HTML_UI
#include <juce_gui_extra/juce_gui_extra.h>
#include <set>
class BloodyMasterEditor final : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit BloodyMasterEditor(BloodyMasterProcessor&);
    ~BloodyMasterEditor() override;
    void resized() override;
#if BM_UI_TESTING
    bool isUiReady() const { return uiReady; }
    void evaluateForTesting(const juce::String&,juce::WebBrowserComponent::EvaluationCallback);
#endif
private:
    class Browser;
    BloodyMasterProcessor& processor;
    std::unique_ptr<Browser> browser;
    juce::Label error;
    std::set<juce::String> activeIds,gestures;
    juce::ValueTree comparison[2];
    int comparisonIndex=0;
    bool uiReady=false;
    bm::dsp::Settings lastTone{};
    double lastCurveRate=-1;
    juce::var curve;
    void timerCallback() override;
    juce::var makeState();
    void parameterEvent(const juce::var&);
    void actionEvent(const juce::var&);
    void updateCurve(double);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BloodyMasterEditor)
};
#else
#pragma once
#include "PluginProcessor.h"

// Functional test editor. The approved HTML resources remain untouched.
class BloodyMasterEditor final : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit BloodyMasterEditor(BloodyMasterProcessor&);
    void resized() override;
private:
    BloodyMasterProcessor& processor;
    juce::Label title, status, meters;
    juce::ComboBox presets;
    juce::Viewport viewport;
    juce::Component parameterList;
    std::vector<std::unique_ptr<juce::Component>> rows;
    void timerCallback() override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BloodyMasterEditor)
};

#endif
