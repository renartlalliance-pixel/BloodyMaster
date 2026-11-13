#if !BM_HTML_UI
#include "PluginEditor.h"
#include "Presets/FactoryPresets.h"

namespace {
class ParameterRow final : public juce::Component {
    juce::Label name;
    juce::Slider slider;
    juce::ToggleButton toggle;
    juce::ComboBox choice;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> buttonAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> choiceAttachment;
public:
    ParameterRow(juce::AudioProcessorValueTreeState& state,const char* id) {
        auto* parameter=state.getParameter(id);
        jassert(parameter!=nullptr);
        name.setText(parameter->getName(64),juce::dontSendNotification);
        addAndMakeVisible(name);
        if(dynamic_cast<juce::AudioParameterBool*>(parameter)!=nullptr) {
            addAndMakeVisible(toggle);
            buttonAttachment=std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(state,id,toggle);
        } else if(auto* selected=dynamic_cast<juce::AudioParameterChoice*>(parameter)) {
            choice.addItemList(selected->choices,1);addAndMakeVisible(choice);
            choiceAttachment=std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(state,id,choice);
        } else {
            slider.setSliderStyle(juce::Slider::LinearHorizontal);
            slider.setTextBoxStyle(juce::Slider::TextBoxRight,false,85,24);
            slider.setTextValueSuffix(" "+parameter->getLabel());addAndMakeVisible(slider);
            sliderAttachment=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state,id,slider);
            const int decimals=parameter->getLabel()=="Hz"?0:2;
            slider.textFromValueFunction=[decimals](double value) { return juce::String(value,decimals); };
            slider.updateText();
        }
    }
    void resized() override {
        auto area=getLocalBounds().reduced(4,2);
        name.setBounds(area.removeFromLeft(175));
        slider.setBounds(area);toggle.setBounds(area);choice.setBounds(area);
    }
};
}
BloodyMasterEditor::BloodyMasterEditor(BloodyMasterProcessor& p)
    : AudioProcessorEditor(&p),processor(p)
{
    title.setText("BloodyMaster - test VST3",juce::dontSendNotification);
    status.setText("DSP actifs uniquement. Interface HTML finale en preparation.",juce::dontSendNotification);
    for(auto* label:{&title,&status,&meters}) addAndMakeVisible(*label);
    const auto& bank=bm::factoryPresets();
    for(size_t i=0;i<bank.size();++i) presets.addItem(bank[i].name,static_cast<int>(i)+1);
    presets.setTextWhenNothingSelected("Choisir un preset");
    presets.onChange=[this] { processor.setCurrentProgram(presets.getSelectedId()-1); };
    addAndMakeVisible(presets);addAndMakeVisible(deltaButton);
    deltaButton.setTooltip(juce::String::fromUTF8("Écouter la différence entre le signal original et le signal traité."));
    deltaButton.onClick=[this]{processor.setDeltaEnabled(!processor.isDeltaEnabled());};
    viewport.setViewedComponent(&parameterList,false);addAndMakeVisible(viewport);
    for(const auto& [id,value]:bm::presetParameters(bank[0].settings)) {
        juce::ignoreUnused(value);
        auto row=std::make_unique<ParameterRow>(processor.apvts,id);
        parameterList.addAndMakeVisible(*row);rows.push_back(std::move(row));
    }
    setSize(640,740);startTimerHz(15);
}
void BloodyMasterEditor::resized() {
    auto area=getLocalBounds().reduced(12);
    title.setBounds(area.removeFromTop(28));
    auto presetRow=area.removeFromTop(32);deltaButton.setBounds(presetRow.removeFromRight(80));presets.setBounds(presetRow);
    status.setBounds(area.removeFromTop(28));
    meters.setBounds(area.removeFromTop(30));
    viewport.setBounds(area);
    const int width=juce::jmax(100,area.getWidth()-viewport.getScrollBarThickness());
    parameterList.setSize(width,static_cast<int>(rows.size())*38);
    for(size_t i=0;i<rows.size();++i) rows[i]->setBounds(0,static_cast<int>(i)*38,width,38);
}
void BloodyMasterEditor::timerCallback() {
    deltaButton.setToggleState(processor.isDeltaEnabled(),juce::dontSendNotification);
    const auto m=processor.getMeterValues();
    const auto db=[](double gain){return juce::String(juce::Decibels::gainToDecibels(gain,-120.),1);};
    meters.setText("IN "+db(m.inputPeak)+" / OUT "+db(m.outputPeak)+" dBFS   GR comp "+juce::String(m.compressorGr,1)+" / lim "+juce::String(m.limiterGr,1)+" dB",juce::dontSendNotification);
}

#endif
