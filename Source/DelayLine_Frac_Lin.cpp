/*
  ==============================================================================

    DelayLine_Frac_Lin.cpp
    Created: 7 Dec 2022 9:56:37am
    Author:  Marius Onofrei

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DelayLine_Frac_Lin.h"

//==============================================================================
DelayLine_Frac_Lin::DelayLine_Frac_Lin(int BUFFER_SIZE)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    
    gBUFFER_SIZE_ = BUFFER_SIZE;
    delayBuffer.resize(gBUFFER_SIZE_,0.0f);
}

DelayLine_Frac_Lin::~DelayLine_Frac_Lin()
{
}


float DelayLine_Frac_Lin::delayLine_Read(int delayReadPointer, float delaySamples)
{
//    delayReadPointer = delayReadPointer_;
    
    float delaySamples_floor = floorf(delaySamples);
    float frac_part = delaySamples - delaySamples_floor;
    
    int readIndex1 = (delayReadPointer - static_cast<int>(delaySamples_floor) + gBUFFER_SIZE_) % gBUFFER_SIZE_;
    int readIndex2 = (delayReadPointer - static_cast<int>(delaySamples_floor+1) + gBUFFER_SIZE_) % gBUFFER_SIZE_;

    float y = (1-frac_part) * delayBuffer[readIndex1] + frac_part * delayBuffer[readIndex2];
    
//    delayReadPointer_++;
//    if (delayReadPointer_>=gBUFFER_SIZE_)
//        delayReadPointer_ = 0;
    
//    delayBuffer[delayWritePointer_] = x;
//
//    delayWritePointer_++;
//    if (delayWritePointer_>=gBUFFER_SIZE_)
//        delayWritePointer_ = 0;
    
    return y;
}

void DelayLine_Frac_Lin::delayLine_Write(float x, int delayWritePointer, float delaySamples)
{
//    delayWritePointer = delayWritePointer_;
    
    delayBuffer[delayWritePointer] = x;

    delayWritePointer++;
    if (delayWritePointer>=gBUFFER_SIZE_)
        delayWritePointer = 0;
    
//    delayWritePointer_ = delayWritePointer;
}



void DelayLine_Frac_Lin::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    
//    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
//
//    g.setColour (juce::Colours::grey);
//    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
//
//    g.setColour (juce::Colours::white);
//    g.setFont (14.0f);
//    g.drawText ("DelayLine_Frac_Lin", getLocalBounds(),
//                juce::Justification::centred, true);   // draw some placeholder text
}

void DelayLine_Frac_Lin::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
