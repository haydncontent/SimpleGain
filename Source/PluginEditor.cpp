#include "PluginEditor.h"

SimpleGainAudioProcessorEditor::SimpleGainAudioProcessorEditor (SimpleGainAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Gain knob
    gainSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    gainSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 90, 22);
    gainSlider.setTextValueSuffix (" dB");
    addAndMakeVisible (gainSlider);

    // Label above it
    gainLabel.setText ("Gain", juce::dontSendNotification);
    gainLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (gainLabel);

    // Phase invert toggle
    addAndMakeVisible (invertButton);

    // Attachments link each control to a parameter ID in both directions:
    // moving the knob changes the parameter; DAW automation moves the knob.
    gainAttachment   = std::make_unique<SliderAttachment> (audioProcessor.apvts, "gain",   gainSlider);
    invertAttachment = std::make_unique<ButtonAttachment> (audioProcessor.apvts, "invert", invertButton);

    setSize (240, 300);
}

void SimpleGainAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Called whenever the editor needs redrawing. Draw the background here.
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void SimpleGainAudioProcessorEditor::resized()
{
    // Layout: carve regions off a rectangle, top to bottom.
    auto area = getLocalBounds().reduced (20);

    gainLabel.setBounds (area.removeFromTop (24));
    invertButton.setBounds (area.removeFromBottom (30));
    gainSlider.setBounds (area); // knob gets whatever space is left
}
