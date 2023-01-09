/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Multitap_DelayAudioProcessor::Multitap_DelayAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", createParameters())
#endif
{
}


Multitap_DelayAudioProcessor::~Multitap_DelayAudioProcessor()
{
}

//==============================================================================
const juce::String Multitap_DelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Multitap_DelayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Multitap_DelayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Multitap_DelayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Multitap_DelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Multitap_DelayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Multitap_DelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Multitap_DelayAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Multitap_DelayAudioProcessor::getProgramName (int index)
{
    return {};
}

void Multitap_DelayAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    
}

//==============================================================================
void Multitap_DelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    Logger::getCurrentLogger()->outputDebugString("Sample rate is " + String(sampleRate) + ".");
    
    auto* selection_gFB = dynamic_cast<AudioParameterFloat*> (apvts.getParameter ("FEEDBACK"));
    gFB_ = selection_gFB->get();
    gFB_ = 0.0;
    
    gOutputBuffer.resize(BUFFER_SIZE_,0.0f);
    
    delayTimes_.resize(noDelays,0.0);
    delaySamples_.resize(noDelays,0.0);
    delayWritePointers_.resize(noDelays,0);
    gFF_.resize(noDelays-1,0.0);
    
//    gFB_ = 0.8;
    
    // initialize delay lines
    for (int i = 0; i < noDelays; ++i)
    {
        if (delayLines_.size() < noDelays)
            delayLines_.push_back(std::make_unique<DelayLine_Frac_Lin>(BUFFER_SIZE_));
    }
    
    // initialize delay times with some random values
//    float LO = 1000;
//    float HI = 1000.001;
    
    float LO = 30;
    float HI = 1000;
    for (int i = 0; i < noDelays; ++i)
    {
        delayTimes_[i] = LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));
        delaySamples_[i]  = delayTimes_[i] * sampleRate / 1000;
        delayWritePointers_[i] = 1;
    }
    
    // initialize delay lines feedforward coefficients with some random values
//    LO = 0.99999999;
//    HI = 1.0;
    LO = 0.3;
    HI = 0.9;
    for (int i = 0; i < noDelays-1; ++i)
    {
        gFF_[i] = LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));
    }
}

void Multitap_DelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Multitap_DelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void Multitap_DelayAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    float* const outputL = buffer.getWritePointer(0);
    float* const outputR = buffer.getWritePointer(1);
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    
    Logger::getCurrentLogger()->outputDebugString("gFB is " + String(gFB_) + ".");
//    Logger::getCurrentLogger()->outputDebugString("Output is " + String(output_) + ".");

    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
//        auto* channelData = buffer.getWritePointer (channel);
        auto* input = buffer.getWritePointer (channel);
        
        if (channel == 0)
        {
            for (int i = 0; i < buffer.getNumSamples(); ++i)
            {
//                float in = gSampleValues[count];
//                gInputBuffer[gInputBufferPointer] = gSampleValues[count];

                float in = input[i];
                float out_lastDelayLine = delayLines_[noDelays-1]->delayLine_Read(gDelayReadPointer_, delaySamples_[noDelays-1]);
                
                float in_del_0 = in + gFB_ * out_lastDelayLine;
                
                delayLines_[0]->delayLine_Write(in_del_0, delayWritePointers_[0], delaySamples_[0]);
                
                // remaining delay lines
                for (int i = 1; i < noDelays; ++i)
                {
                    delayLines_[i]->delayLine_Write(delayLines_[i-1]->delayLine_Read(gDelayReadPointer_, delaySamples_[i-1]), delayWritePointers_[i-1], delaySamples_[i-1]);
                }
                
                for (int i = 0; i < noDelays; ++i)
                {
                    delayWritePointers_[i] = delayWritePointers_[i] + 1;
                    if (delayWritePointers_[i]>=BUFFER_SIZE_)
                        delayWritePointers_[i] = 0;
                }
                
                float outSums = 0.0f;
                for (int i = 0; i < noDelays-1; ++i)
                {
                    outSums = outSums + gFF_[i] * delayLines_[i]->delayLine_Read(gDelayReadPointer_, delaySamples_[i]);
                }
                
                gOutputBuffer[gDelayReadPointer_] = in + outSums;
                
                output_ = gOutputBuffer[gOutputBufferReadPointer_];

                
                // update gDelayWritePointer
                gDelayReadPointer_ = gDelayReadPointer_ + 1;
                if (gDelayReadPointer_>=BUFFER_SIZE_)
                    gDelayReadPointer_ = 0;
                
                // update gOutputBufferReadPointer
                gOutputBufferReadPointer_ = gOutputBufferReadPointer_ + 1;
                if (gOutputBufferReadPointer_>=BUFFER_SIZE_)
                    gOutputBufferReadPointer_ = 0;
                
                if (abs((output_)) > 1)
                {
                    Logger::getCurrentLogger()->outputDebugString("Output is too loud!");
//                    output = 1.0;
                }
                
                outputL[i] = output_ * 1.0;
                outputR[i] = outputL[i];
            }
        }
    }
}

//==============================================================================
bool Multitap_DelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Multitap_DelayAudioProcessor::createEditor()
{
    return new Multitap_DelayAudioProcessorEditor (*this);
}

//==============================================================================
void Multitap_DelayAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Multitap_DelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Multitap_DelayAudioProcessor();
}
