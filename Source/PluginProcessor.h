/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class MelodyanalizerAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    MelodyanalizerAudioProcessor();
    ~MelodyanalizerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void loadFile();
    bool isAudioFile(const juce::File& file);
    const float& getFFT(juce::Array<float> destBuffers, int numSamples);
    bool isAudioLoaded = false;
    int fundamentalFrecuency = 0;
    juce::String audioName = "";

private:
    //==============================================================================
    juce::AudioFormatManager formatManager;
    juce::AudioFormatReader* formatReader{ nullptr };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MelodyanalizerAudioProcessor)
    template<std::size_t SIZE>
    const float& getFundamentalFrecuency(const int size, const std::array<float, SIZE> destBuffers, float param);
};
