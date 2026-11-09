#pragma once
#include "PluginProcessor.h"

/** Editeur provisoire M1 : liste generique des parametres pour valider l'automation.
 *  Sera remplace par la WebView2 (HTML d'origine) a l'etape M7. */
class BloodyMasterEditor final : public juce::AudioProcessorEditor
{
public:
    explicit BloodyMasterEditor(BloodyMasterProcessor&);
    void resized() override;

private:
    juce::GenericAudioProcessorEditor generic;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BloodyMasterEditor)
};
