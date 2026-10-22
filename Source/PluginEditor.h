#pragma once
#include "PluginProcessor.h"

class VST3EffektPlusAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit VST3EffektPlusAudioProcessorEditor (VST3EffektPlusAudioProcessor&);
    ~VST3EffektPlusAudioProcessorEditor() override = default;
    void paint (juce::Graphics&) override;
    void resized() override;
private:
    VST3EffektPlusAudioProcessor& processor;
    struct Knob
    {
        juce::Slider slider;
        juce::Label label;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    };
    std::vector<std::unique_ptr<Knob>> knobs;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VST3EffektPlusAudioProcessorEditor)
};
