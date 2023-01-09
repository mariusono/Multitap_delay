/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DelayLine_Frac_Lin.h"

//==============================================================================
/**
*/
class Multitap_DelayAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    Multitap_DelayAudioProcessor();
    ~Multitap_DelayAudioProcessor() override;

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

    void set_gFB(float val) { gFB_ = val; }
    
    
    // FOR PARAMETERS ! 
    juce::AudioProcessorValueTreeState apvts;

private:
    
    int BUFFER_SIZE_ = 262144;

    int gDelayWritePointer_ = 1;
    int gDelayReadPointer_ = 1;
    
    int gOutputBufferReadPointer_ = 0;
    
    int noDelays = 7;
    
    std::vector<std::unique_ptr<DelayLine_Frac_Lin>> delayLines_;
    
    std::vector<float> gOutputBuffer;
    
    std::vector<float> delayTimes_;
    std::vector<float> delaySamples_;
    std::vector<int> delayWritePointers_;
    std::vector<float> gFF_; // feedforward coeficients for (noDelays-1) delay lines;
    float gFB_; // feedback coeficient for last delay line;

    
    float output_;
    
    
    // AUDIO PARAMS
    
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters()
    {
        std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
        
        params.push_back(std::make_unique<AudioParameterFloat>("FEEDBACK","Feedback",0.0f,1.0f,0.0f));

        return { params.begin(), params.end()};
    }
    
    float gFB_param;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Multitap_DelayAudioProcessor)
};
