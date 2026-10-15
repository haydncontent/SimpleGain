#pragma once

#include "PluginProcessor.h"

class SimpleGainAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit SimpleGainAudioProcessorEditor (SimpleGainAudioProcessor&);
    ~SimpleGainAudioProcessorEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    SimpleGainAudioProcessor& audioProcessor;

    juce::Label gainLabel;
    juce::Slider gainSlider;
    juce::ToggleButton invertButton { "Invert Phase" };

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    // IMPORTANT: declared AFTER the controls so they are destroyed BEFORE them.
    std::unique_ptr<SliderAttachment> gainAttachment;
    std::unique_ptr<ButtonAttachment> invertAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleGainAudioProcessorEditor)
};
