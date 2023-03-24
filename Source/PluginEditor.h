/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class MelodyanalizerAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    MelodyanalizerAudioProcessorEditor (MelodyanalizerAudioProcessor&);
    ~MelodyanalizerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MelodyanalizerAudioProcessor& audioProcessor;
    juce::TextButton loadButton{ "Load" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MelodyanalizerAudioProcessorEditor)
};
