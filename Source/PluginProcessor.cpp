#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Parameters/ParameterIDs.h"
#include "Parameters/ParameterLayout.h"

BloodyMasterProcessor::BloodyMasterProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "BloodyMasterState", bm::createParameterLayout())
{
}

void BloodyMasterProcessor::prepareToPlay(double, int)
{
    // M1 : aucun etat DSP a preparer. Latence = 0.
    setLatencySamples(0);
}

bool BloodyMasterProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto& in  = layouts.getMainInputChannelSet();
    const auto& out = layouts.getMainOutputChannelSet();
    if (in != out) return false;
    return out == juce::AudioChannelSet::mono() || out == juce::AudioChannelSet::stereo();
}

void BloodyMasterProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    // Les canaux de sortie sans entree correspondante sont mis a zero (pas de bruit residuel).
    for (int ch = getTotalNumInputChannels(); ch < getTotalNumOutputChannels(); ++ch)
        buffer.clear(ch, 0, buffer.getNumSamples());

    // M1 : le buffer est traite sur place et laisse inchange (passthrough bit-exact).
    // Le bypass hote (getBypassParameter) sera relie au rendu dry/wet des qu'un traitement existera.
}

juce::AudioProcessorEditor* BloodyMasterProcessor::createEditor()
{
    return new BloodyMasterEditor(*this);
}

juce::AudioProcessorParameter* BloodyMasterProcessor::getBypassParameter() const
{
    return apvts.getParameter(bm::id::bypass);
}

void BloodyMasterProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary(*xml, destData);
}

void BloodyMasterProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
        if (xml->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BloodyMasterProcessor();
}
