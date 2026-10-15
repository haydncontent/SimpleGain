#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleGainAudioProcessor::SimpleGainAudioProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMETERS", createParameterLayout())
{
    gainDbParam = apvts.getRawParameterValue ("gain");
    invertParam = apvts.getRawParameterValue ("invert");
}

juce::AudioProcessorValueTreeState::ParameterLayout
SimpleGainAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // IDs ("gain", "invert") are what the editor and processBlock use to find
    // parameters. Never change them after release or old sessions break.
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "gain", 1 },
        "Gain",
        juce::NormalisableRange<float> (-60.0f, 12.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel ("dB")));

    layout.add (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { "invert", 1 },
        "Invert Phase",
        false));

    return layout;
}

//==============================================================================
void SimpleGainAudioProcessor::prepareToPlay (double /*sampleRate*/, int /*samplesPerBlock*/)
{
    // This is where you'd allocate delay lines / ring buffers and set up
    // filters using the real sample rate. Gain needs nothing, but we
    // initialise lastGain so the first block doesn't ramp from a stale value.
    const float sign = invertParam->load() > 0.5f ? -1.0f : 1.0f;
    lastGain = juce::Decibels::decibelsToGain (gainDbParam->load(), -60.0f) * sign;
}

bool SimpleGainAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& out = layouts.getMainOutputChannelSet();

    if (out != juce::AudioChannelSet::mono() && out != juce::AudioChannelSet::stereo())
        return false;

    return out == layouts.getMainInputChannelSet();
}

void SimpleGainAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                             juce::MidiBuffer& /*midi*/)
{
    juce::ScopedNoDenormals noDenormals;

    // Clear any output channels that have no matching input.
    for (int ch = getTotalNumInputChannels(); ch < getTotalNumOutputChannels(); ++ch)
        buffer.clear (ch, 0, buffer.getNumSamples());

    // Read parameters ONCE per block. No locks, no allocation on this thread.
    const float sign   = invertParam->load() > 0.5f ? -1.0f : 1.0f;
    const float target = juce::Decibels::decibelsToGain (gainDbParam->load(), -60.0f) * sign;

    // Ramp from last block's gain to the new one to avoid zipper noise.
    buffer.applyGainRamp (0, buffer.getNumSamples(), lastGain, target);
    lastGain = target;
}

//==============================================================================
juce::AudioProcessorEditor* SimpleGainAudioProcessor::createEditor()
{
    return new SimpleGainAudioProcessorEditor (*this);

    // While prototyping DSP, you can instead return an auto-generated UI:
    // return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void SimpleGainAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary (*xml, destData);
}

void SimpleGainAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

//==============================================================================
// JUCE calls this to create the plugin instance.
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleGainAudioProcessor();
}
