/*
  ==============================================================================

    DelayLine_Frac_Lin.h
    Created: 7 Dec 2022 9:56:37am
    Author:  Marius Onofrei

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class DelayLine_Frac_Lin  : public juce::Component
{
public:
    DelayLine_Frac_Lin(int BUFFER_SIZE);
    ~DelayLine_Frac_Lin() override;

    float delayLine_Read(int delayReadPointer, float delaySamples);
    void delayLine_Write(float x, int delayReadPointer, float delaySamples);

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void set_delaySamples(double val) { delaySamples_ = val; }
    void set_delayWritePointer(double val) { delayWritePointer_ = val; }
    void set_delayReadPointer(double val) { delayReadPointer_ = val; }

private:
    
    std::vector<float> delayBuffer; // the delay buffer vector
    
    int gBUFFER_SIZE_; // public !
    float delaySamples_;
    int delayWritePointer_;
    int delayReadPointer_;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayLine_Frac_Lin)
};
