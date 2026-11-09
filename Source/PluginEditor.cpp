#include "PluginEditor.h"

BloodyMasterEditor::BloodyMasterEditor(BloodyMasterProcessor& p)
    : AudioProcessorEditor(&p), generic(p)
{
    addAndMakeVisible(generic);
    setSize(520, 640);
}

void BloodyMasterEditor::resized()
{
    generic.setBounds(getLocalBounds());
}
