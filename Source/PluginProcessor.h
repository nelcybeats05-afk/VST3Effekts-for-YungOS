#pragma once
#include <JuceHeader.h>

class VST3EffektPlusAudioProcessor final : public juce::AudioProcessor
{
public:
    VST3EffektPlusAudioProcessor();
    ~VST3EffektPlusAudioProcessor() override = default;
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "VST3Effekt+"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 4.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}
    void getStateInformation (juce::MemoryBlock&) override;
    void setStateInformation (const void*, int) override;
    bool isBusesLayoutSupported (const BusesLayout&) const override;
    juce::AudioProcessorValueTreeState apvts;
    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();
private:
    juce::dsp::Reverb reverb;
    juce::dsp::StateVariableTPTFilter<float> lowpass[2];
    juce::AudioBuffer<float> delayBuffer;
    int delayWrite = 0;
    double currentSampleRate = 44100.0;
    float envelope[2] = { 0.0f, 0.0f };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VST3EffektPlusAudioProcessor)
};
