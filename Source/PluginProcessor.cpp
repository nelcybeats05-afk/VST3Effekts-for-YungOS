#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>
#include <algorithm>

VST3EffektPlusAudioProcessor::VST3EffektPlusAudioProcessor()
    : AudioProcessor (BusesProperties().withInput ("Input", juce::AudioChannelSet::stereo(), true)
                                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMETERS", createLayout()) {}

juce::AudioProcessorValueTreeState::ParameterLayout VST3EffektPlusAudioProcessor::createLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout p;
    auto add = [&p] (const char* id, const char* name, float lo, float hi, float initial)
    {
        p.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { id, 1 }, name,
                juce::NormalisableRange<float> (lo, hi, 0.01f), initial));
    };
    add ("mix", "Dry / Wet", 0, 100, 50);
    add ("drive", "Drive", 0, 100, 0);
    add ("cutoff", "Lowpass", 200, 20000, 20000);
    add ("delay", "Delay Time", 50, 1000, 300);
    add ("feedback", "Delay Feedback", 0, 85, 25);
    add ("delaymix", "Delay Amount", 0, 100, 0);
    add ("reverb", "Reverb Amount", 0, 100, 0);
    add ("room", "Room Size", 0, 100, 45);
    add ("threshold", "Compressor Threshold", -40, 0, -12);
    add ("ratio", "Compressor Ratio", 1, 12, 2);
    add ("output", "Output Gain", -24, 12, 0);
    return p;
}

bool VST3EffektPlusAudioProcessor::isBusesLayoutSupported (const BusesLayout& l) const
{
    const auto out = l.getMainOutputChannelSet();
    return (out == juce::AudioChannelSet::mono() || out == juce::AudioChannelSet::stereo())
           && l.getMainInputChannelSet() == out;
}

void VST3EffektPlusAudioProcessor::prepareToPlay (double sr, int block)
{
    currentSampleRate = sr;
    juce::dsp::ProcessSpec spec { sr, static_cast<juce::uint32> (juce::jmax (1, block)), 2 };
    reverb.prepare (spec);
    reverb.reset();
    for (auto& f : lowpass) { f.prepare ({ sr, static_cast<juce::uint32> (juce::jmax (1, block)), 1 }); f.setType (juce::dsp::StateVariableTPTFilterType::lowpass); f.reset(); }
    delayBuffer.setSize (2, static_cast<int> (sr * 1.1) + 4);
    delayBuffer.clear();
    dryBuffer.setSize (2, juce::jmax (1, block));
    delayWrite = 0;
    envelope[0] = envelope[1] = 0;
}

void VST3EffektPlusAudioProcessor::releaseResources() { delayBuffer.setSize (0, 0); dryBuffer.setSize (0, 0); }

void VST3EffektPlusAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int channels = juce::jmin (buffer.getNumChannels(), 2);
    const int count = buffer.getNumSamples();
    if (channels == 0 || delayBuffer.getNumSamples() == 0) return;
    const auto value = [this] (const char* id) { return apvts.getRawParameterValue (id)->load(); };
    const float wet = value ("mix") / 100.0f;
    const float drive = value ("drive") / 100.0f;
    const float delayAmount = value ("delaymix") / 100.0f;
    const float feedback = value ("feedback") / 100.0f;
    const float revAmount = value ("reverb") / 100.0f;
    const float ratio = value ("ratio");
    const float threshold = juce::Decibels::decibelsToGain (value ("threshold"));
    const float outputGain = juce::Decibels::decibelsToGain (value ("output"));
    const int delaySamples = juce::jlimit (1, delayBuffer.getNumSamples() - 1,
                                             static_cast<int> (value ("delay") * currentSampleRate / 1000.0));
    for (int ch = 0; ch < channels; ++ch) lowpass[ch].setCutoffFrequency (value ("cutoff"));
    if (count > dryBuffer.getNumSamples()) return; // Host block size should match prepareToPlay
    for (int ch = 0; ch < channels; ++ch)
        dryBuffer.copyFrom (ch, 0, buffer, ch, 0, count);
    for (int i = 0; i < count; ++i)
    {
        const int read = (delayWrite + delayBuffer.getNumSamples() - delaySamples) % delayBuffer.getNumSamples();
        for (int ch = 0; ch < channels; ++ch)
        {
            const float input = buffer.getSample (ch, i);
            float s = std::tanh (input * (1.0f + 15.0f * drive)) / std::tanh (1.0f + 15.0f * drive);
            s = lowpass[ch].processSample (0, s);
            const float delayed = delayBuffer.getSample (ch, read);
            delayBuffer.setSample (ch, delayWrite, juce::jlimit (-2.0f, 2.0f, s + delayed * feedback));
            s += delayed * delayAmount;
            const float magnitude = std::abs (s);
            const float coeff = magnitude > envelope[ch] ? 0.015f : 0.0007f;
            envelope[ch] += coeff * (magnitude - envelope[ch]);
            if (envelope[ch] > threshold && threshold > 0.0f)
            {
                const float gain = std::pow (threshold / envelope[ch], 1.0f - 1.0f / ratio);
                s *= gain;
            }
            buffer.setSample (ch, i, s);
        }
        delayWrite = (delayWrite + 1) % delayBuffer.getNumSamples();
    }
    juce::dsp::Reverb::Parameters rp;
    rp.roomSize = value ("room") / 100.0f;
    rp.damping = 0.5f;
    rp.wetLevel = revAmount;
    rp.dryLevel = 1.0f - revAmount;
    rp.width = 1.0f;
    rp.freezeMode = 0.0f;
    reverb.setParameters (rp);
    if (channels == 2) reverb.processStereo (buffer.getWritePointer (0), buffer.getWritePointer (1), count);
    else reverb.processMono (buffer.getWritePointer (0), count);
    for (int ch = 0; ch < channels; ++ch)
        for (int i = 0; i < count; ++i)
            buffer.setSample (ch, i, (dryBuffer.getSample (ch, i) * (1.0f - wet) + buffer.getSample (ch, i) * wet) * outputGain);
}

void VST3EffektPlusAudioProcessor::getStateInformation (juce::MemoryBlock& dest)
{
    if (auto xml = apvts.copyState().createXml()) copyXmlToBinary (*xml, dest);
}
void VST3EffektPlusAudioProcessor::setStateInformation (const void* data, int size)
{
    if (auto xml = getXmlFromBinary (data, size))
        if (xml->hasTagName (apvts.state.getType())) apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new VST3EffektPlusAudioProcessor(); }
