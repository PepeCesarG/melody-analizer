/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MelodyanalizerAudioProcessor::MelodyanalizerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

MelodyanalizerAudioProcessor::~MelodyanalizerAudioProcessor()
{
}

//==============================================================================
const juce::String MelodyanalizerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MelodyanalizerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MelodyanalizerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MelodyanalizerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MelodyanalizerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MelodyanalizerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MelodyanalizerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MelodyanalizerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MelodyanalizerAudioProcessor::getProgramName (int index)
{
    return {};
}

void MelodyanalizerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MelodyanalizerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void MelodyanalizerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MelodyanalizerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void MelodyanalizerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
}

//==============================================================================
bool MelodyanalizerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MelodyanalizerAudioProcessor::createEditor()
{
    return new MelodyanalizerAudioProcessorEditor (*this);
}

//==============================================================================
void MelodyanalizerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MelodyanalizerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void MelodyanalizerAudioProcessor::loadFile()
{
    juce::FileChooser chooser("Please load a file", juce::File::getSpecialLocation(juce::File::userDesktopDirectory), "*.wav;*.mp3;*.aiff");
    //Vemos si se a elegido un archivo
    if (chooser.browseForFileToOpen()) {
        auto file = chooser.getResult();
        //Volvemos a comprobar si el archivo es de audio
        if (isAudioFile(file)) {
            //Creamos el reader en base al archivo
            formatManager.registerBasicFormats();
            formatReader = formatManager.createReaderFor(file);

            //Leemos el audio y lo metemos en un buffer
            juce::AudioBuffer<float> audioBuffer;
            audioBuffer.setSize(formatReader->numChannels, formatReader->lengthInSamples);
            formatReader->read(&audioBuffer, 0, formatReader->lengthInSamples, 0, true, true);

            //Hacer cruces por cero y calcular la fft de cada silaba
            juce::Array<float> buffer;
            juce::MidiMessageSequence midiBuffer;
            int zeroCrossing = 0;
            int bits = 1;
            double prevMidiNote = -1;
            for (int i = 0; i < formatReader->lengthInSamples - 1; ++i) {
                float sample = audioBuffer.getSample(0, i);
                float nextSample = audioBuffer.getSample(0, i + 1);
                buffer.add(sample);
                if ((sample < 0 && nextSample > 0) || (sample > 0 && nextSample < 0)) {
                    ++zeroCrossing;
                }
                if (zeroCrossing > 1000) {
                    auto ff = getFFT(buffer, buffer.size());
                    int midiNote = log(ff / 440.0) / log(2) * 12 + 69;
                    if (prevMidiNote == -1) {
                        auto noteOn = juce::MidiMessage::noteOn(1, midiNote, 1.0f);
                        midiBuffer.addEvent(noteOn);
                        prevMidiNote = midiNote;
                    }
                    if(midiNote != prevMidiNote && midiNote > 0) {
                        auto noteOff = juce::MidiMessage::noteOff(1, prevMidiNote);
                        midiBuffer.addEvent(noteOff, bits * 24);
                        bits = 0;
                        auto noteOn = juce::MidiMessage::noteOn(1, midiNote, 1.0f);
                        prevMidiNote = midiNote;
                        midiBuffer.addEvent(noteOn);
                        
                    }else{
                      bits++; 
                    }
                    zeroCrossing = 0;
                    buffer.clear();
                }

            }
            auto noteOff = juce::MidiMessage::noteOff(1, prevMidiNote);
            midiBuffer.addEvent(noteOff, bits * 48);
            midiBuffer.updateMatchedPairs();
            juce::MidiFile midiFile;
            midiFile.setTicksPerQuarterNote(96);
            midiFile.addTrack(midiBuffer);
            
            juce::String fileName = "example-";
            auto randomInt = juce::String(juce::Random::getSystemRandom().nextInt());
            fileName.append(randomInt, 5);
            fileName.append(".midi", 5);
            juce::File outputFile = juce::File(juce::File::getSpecialLocation(juce::File::userDesktopDirectory)).getChildFile(fileName);
            outputFile.deleteFile();
            juce::FileOutputStream outputStream(outputFile);
            midiFile.writeTo(outputStream);
            outputStream.flush();
            //El audio se ha cargado
            isAudioLoaded = true;
            audioName = file.getFileName();
            formatManager.clearFormats();
            buffer.clear();

        }
    }


}

bool MelodyanalizerAudioProcessor::isAudioFile(const juce::File& file) {
    if (file.getFileName().contains(".wav") || file.getFileName().contains(".mp3") || file.getFileName().contains(".aiff")) {
        return true;
    }
    return false;

}

const float& MelodyanalizerAudioProcessor::getFFT(juce::Array<float> destBuffers, int numSamples)
{
    constexpr auto fftOrder = 15;
    constexpr auto fftSize = 1 << fftOrder;
    juce::dsp::FFT forwardFFT(fftOrder);
    std::array<float, fftSize> fifo;
    std::array<float, fftSize * 2> fftData;
    int fifoIndex = 0;
    auto ff = 0;

    for (int i = 0; i < numSamples; ++i) {
        float sample = destBuffers[i];
        if (fifoIndex == fftSize) {
            std::fill(std::next(fftData.begin(), fftSize), fftData.end(), 0.0f);
            std::copy(fifo.begin(), fifo.end(), fftData.begin());
            forwardFFT.performFrequencyOnlyForwardTransform(fftData.data());
            fifoIndex = 0;
        }

        //Aplicamos ventana senoidal a la muestra
        fifo[fifoIndex] = sample * std::sin(float(fifoIndex) * ((1.f / float(fftSize)) * juce::MathConstants<float>::pi));
        ++fifoIndex;
    }
    return getFundamentalFrecuency(1 * fftData.size() / 2, fftData, fftSize);;
}

template<std::size_t SIZE>
const float& MelodyanalizerAudioProcessor::getFundamentalFrecuency(const int size, const   std::array<float, SIZE> destBuffers, float fftSize)
{
    float index = 0.f;
    float max = 0.f;
    float absSample;
    for (auto i = 0; i < size; i++) {
        absSample = std::abs(destBuffers[i]);
        if (max < absSample) {
            max = absSample;
            index = i;

        }
    }
    auto sampleRate = formatReader->sampleRate;
    return (index * sampleRate) / fftSize;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MelodyanalizerAudioProcessor();
}
