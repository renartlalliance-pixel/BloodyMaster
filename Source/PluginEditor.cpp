#include "PluginEditor.h"
#include "Parameters/ParameterIDs.h"

#include <memory>
#include <vector>

namespace
{
class SliderControl final : public juce::Component
{
public:
    SliderControl(BloodyMasterProcessor& processor, const juce::String& parameterId)
        : attachment(processor.apvts, parameterId, slider)
    {
        auto* parameter = processor.apvts.getParameter(parameterId);
        title.setText(parameter->getName(64), juce::dontSendNotification);
        title.setJustificationType(juce::Justification::centred);
        title.setColour(juce::Label::textColourId, juce::Colour(0xffd5c9ad));
        title.setFont(juce::Font(juce::FontOptions(12.0f, juce::Font::bold)));
        addAndMakeVisible(title);

        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 84, 18);
        slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffc7a756));
        slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff46413a));
        slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffefede6));
        slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff17191b));
        slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff383a3d));
        if (parameter->getLabel().isNotEmpty())
            slider.setTextValueSuffix(" " + parameter->getLabel());
        addAndMakeVisible(slider);
    }

    void resized() override
    {
        title.setBounds(getLocalBounds().removeFromTop(20));
        slider.setBounds(getLocalBounds());
    }

private:
    juce::Label title;
    juce::Slider slider;
    juce::AudioProcessorValueTreeState::SliderAttachment attachment;
};

class ToggleControl final : public juce::Component
{
public:
    ToggleControl(BloodyMasterProcessor& processor, const juce::String& parameterId)
        : attachment(processor.apvts, parameterId, button)
    {
        button.setButtonText(processor.apvts.getParameter(parameterId)->getName(64));
        button.setColour(juce::ToggleButton::textColourId, juce::Colour(0xffd5c9ad));
        button.setColour(juce::ToggleButton::tickColourId, juce::Colour(0xffd6453d));
        addAndMakeVisible(button);
    }

    void resized() override { button.setBounds(getLocalBounds().reduced(8, 24)); }

private:
    juce::ToggleButton button;
    juce::AudioProcessorValueTreeState::ButtonAttachment attachment;
};

class ChoiceControl final : public juce::Component
{
public:
    ChoiceControl(BloodyMasterProcessor& processor, const juce::String& parameterId)
    {
        title.setText(processor.apvts.getParameter(parameterId)->getName(64), juce::dontSendNotification);
        title.setJustificationType(juce::Justification::centred);
        title.setColour(juce::Label::textColourId, juce::Colour(0xffd5c9ad));
        title.setFont(juce::Font(juce::FontOptions(12.0f, juce::Font::bold)));
        addAndMakeVisible(title);

        if (parameterId == bm::id::profile)
            choice.addItemList({ "1 - Stereo", "2 - Wide", "3 - Mid focus", "4 - Mono" }, 1);
        else if (parameterId == bm::id::oversampling)
            choice.addItemList({ "2x", "4x", "8x" }, 1);
        else
            choice.addItemList({ "-8 LUFS", "-9 LUFS", "-11 LUFS", "-14 LUFS", "-6 LUFS" }, 1);
        choice.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff17191b));
        choice.setColour(juce::ComboBox::textColourId, juce::Colour(0xffefede6));
        choice.setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff65563a));
        attachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
            processor.apvts, parameterId, choice);
        addAndMakeVisible(choice);
    }

    void resized() override
    {
        title.setBounds(getLocalBounds().removeFromTop(24));
        choice.setBounds(getLocalBounds().reduced(5, 28).withHeight(30));
    }

private:
    juce::Label title;
    juce::ComboBox choice;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> attachment;
};

class ParameterPage final : public juce::Component
{
public:
    ParameterPage(BloodyMasterProcessor& processor, const std::vector<const char*>& ids)
    {
        for (const auto* id : ids)
        {
            auto* parameter = processor.apvts.getParameter(id);
            if (dynamic_cast<juce::AudioParameterBool*>(parameter) != nullptr)
                controls.push_back(std::make_unique<ToggleControl>(processor, id));
            else if (dynamic_cast<juce::AudioParameterChoice*>(parameter) != nullptr)
                controls.push_back(std::make_unique<ChoiceControl>(processor, id));
            else
                controls.push_back(std::make_unique<SliderControl>(processor, id));
            addAndMakeVisible(*controls.back());
        }
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xff202225));
    }

    void resized() override
    {
        constexpr int columns = 4;
        constexpr int cellHeight = 112;
        constexpr int horizontalPadding = 18;
        constexpr int verticalPadding = 12;
        const auto width = (getWidth() - horizontalPadding * 2) / columns;
        for (size_t i = 0; i < controls.size(); ++i)
        {
            const auto column = static_cast<int>(i % columns);
            const auto row = static_cast<int>(i / columns);
            controls[i]->setBounds(horizontalPadding + column * width,
                                   verticalPadding + row * cellHeight,
                                   width - 4, cellHeight - 4);
        }
    }

private:
    std::vector<std::unique_ptr<juce::Component>> controls;
};
}

BloodyMasterEditor::BloodyMasterEditor(BloodyMasterProcessor& processor)
    : AudioProcessorEditor(&processor),
      tabs(juce::TabbedButtonBar::TabsAtTop)
{
    setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(tabs);

    tabs.addTab("Equalizer", juce::Colour(0xff202225), new ParameterPage(processor,
        { bm::id::defreq, bm::id::deamount, bm::id::lowres, bm::id::hires,
          bm::id::hpf, bm::id::lpf, bm::id::xl, bm::id::thd, bm::id::bass,
          bm::id::mid, bm::id::high, bm::id::presence, bm::id::p1f, bm::id::p1g,
          bm::id::p2f, bm::id::p2g, bm::id::p3f, bm::id::p3g, bm::id::eqActive,
          bm::id::spikeListen }), true);
    tabs.addTab("Dynamics", juce::Colour(0xff202225), new ParameterPage(processor,
        { bm::id::drive, bm::id::link, bm::id::mix, bm::id::squash, bm::id::recovery,
          bm::id::glue, bm::id::clipper, bm::id::valve, bm::id::oversampling }), true);
    tabs.addTab("Stereo", juce::Colour(0xff202225), new ParameterPage(processor,
        { bm::id::monomaker, bm::id::width, bm::id::enhance, bm::id::pan,
          bm::id::foundation, bm::id::profile }), true);
    tabs.addTab("Output", juce::Colour(0xff202225), new ParameterPage(processor,
        { bm::id::trim, bm::id::turbo, bm::id::limrel, bm::id::limlink, bm::id::isp,
          bm::id::lufsTarget, bm::id::bypass }), true);

    setSize(1100, 760);
    setResizable(true, true);
    setResizeLimits(850, 600, 1500, 1000);
}

BloodyMasterEditor::~BloodyMasterEditor()
{
    setLookAndFeel(nullptr);
}

void BloodyMasterEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff151618));
    g.setColour(juce::Colour(0xffd0ae5b));
    g.setFont(juce::Font(juce::FontOptions(17.0f, juce::Font::bold)));
    g.drawText("B.R.I AUDIO SYSTEMS", 22, 8, 300, 24, juce::Justification::centredLeft);
    g.setColour(juce::Colour(0xffe6e1d5));
    g.setFont(juce::Font(juce::FontOptions(24.0f, juce::Font::bold)));
    g.drawText("BLOODY MASTER", 22, 30, 320, 32, juce::Justification::centredLeft);
    g.setColour(juce::Colour(0xff8e9298));
    g.setFont(juce::Font(juce::FontOptions(12.0f)));
    g.drawText("MASTERING PROCESSOR  |  VST3", getWidth() - 300, 33, 278, 24, juce::Justification::centredRight);
}

void BloodyMasterEditor::resized()
{
    tabs.setBounds(12, 72, getWidth() - 24, getHeight() - 84);
}
