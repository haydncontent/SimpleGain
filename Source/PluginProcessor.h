#pragma once

#include <JuceHeader.h>

class SimpleGainAudioProcessor : public juce::AudioProcessor
{
public:
    SimpleGainAudioProcessor();
    ~SimpleGainAudioProcessor() override = default;

    // --- Audio ---------------------------------------------------------
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    // --- Editor --------------------------------------------------------
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    // --- Boilerplate JUCE requires ------------------------------------
    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    // --- Save/load (DAW sessions, presets) -----------------------------
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Owns all parameters. The editor attaches its controls to this.
    juce::AudioProcessorValueTreeState apvts;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Raw atomic pointers: safe to read on the audio thread.
    std::atomic<float>* gainDbParam = nullptr;
    std::atomic<float>* invertParam = nullptr;

    float lastGain = 1.0f; // gain used at the end of the previous block

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleGainAudioProcessor)
};
