/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Multitap_DelayAudioProcessorEditor::Multitap_DelayAudioProcessorEditor (Multitap_DelayAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    
    
    addAndMakeVisible(gFB_Slider);
    gFB_Slider.setTextValueSuffix(" [-]");
    gFB_Slider.addListener(this);
    gFB_Slider.setRange(0.0,1.0);
    gFB_Slider.setValue(0.0);
    addAndMakeVisible(gFB_Label);
    gFB_Label.setText("Feedback", juce::dontSendNotification);
    gFB_Label.attachToComponent(&gFB_Slider, true);

    gFB_SliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts,"FEEDBACK",gFB_Slider);

    
    
    
}

Multitap_DelayAudioProcessorEditor::~Multitap_DelayAudioProcessorEditor()
{
}

//==============================================================================
void Multitap_DelayAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void Multitap_DelayAudioProcessorEditor::resized()
{
    auto sliderLeft = 120;

    gFB_Slider.setBounds(sliderLeft, 20, getWidth() - sliderLeft - 10, 20);
}

void Multitap_DelayAudioProcessorEditor::sliderValueChanged(Slider* slider)
{

    if (slider == &gFB_Slider)
    {
        audioProcessor.set_gFB(gFB_Slider.getValue());
        DBG(String(gFB_Slider.getValue()));
    }
//    else if (slider == &biquadSlider)
//    {
////        audioProcessor.setF_Biquad(biquadSlider.getValue());
////        f_biquad = biquadSlider.getValue();
//    }
}
