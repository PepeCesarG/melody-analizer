/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MelodyanalizerAudioProcessorEditor::MelodyanalizerAudioProcessorEditor (MelodyanalizerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    loadButton.onClick = [&]() {
        audioProcessor.loadFile();
    };
    setSize(800, 300);
    addAndMakeVisible(loadButton);
}

MelodyanalizerAudioProcessorEditor::~MelodyanalizerAudioProcessorEditor()
{
}

//==============================================================================
void MelodyanalizerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
}

void MelodyanalizerAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    loadButton.setBounds(getWidth() / 2 - 50, getHeight() / 2 - 50, 100, 100);
}
