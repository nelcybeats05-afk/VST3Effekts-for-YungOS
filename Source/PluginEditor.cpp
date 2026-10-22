#include "PluginEditor.h"

VST3EffektPlusAudioProcessorEditor::VST3EffektPlusAudioProcessorEditor (VST3EffektPlusAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    const std::pair<const char*, const char*> controls[] = {
        {"mix", "DRY / WET"}, {"drive", "DRIVE"}, {"cutoff", "LOWPASS"},
        {"delay", "DELAY TIME"}, {"feedback", "FEEDBACK"}, {"delaymix", "DELAY MIX"},
        {"reverb", "REVERB"}, {"room", "ROOM SIZE"}, {"threshold", "THRESHOLD"},
        {"ratio", "RATIO"}, {"output", "OUTPUT"}
    };
    for (auto [id, title] : controls)
    {
        auto k = std::make_unique<Knob>();
        k->slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        k->slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 100, 22);
        k->slider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xffa75bff));
        k->slider.setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (0xff30324f));
        k->slider.setColour (juce::Slider::thumbColourId, juce::Colours::white);
        k->slider.setColour (juce::Slider::textBoxTextColourId, juce::Colours::white);
        k->slider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        k->label.setText (title, juce::dontSendNotification);
        k->label.setJustificationType (juce::Justification::centred);
        k->label.setColour (juce::Label::textColourId, juce::Colour (0xffc6c0df));
        addAndMakeVisible (k->slider);
        addAndMakeVisible (k->label);
        k->attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processor.apvts, id, k->slider);
        knobs.push_back (std::move (k));
    }
    setSize (840, 540);
}

void VST3EffektPlusAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff0c0b18));
    juce::ColourGradient bg (juce::Colour (0xff25143e), 0, 0, juce::Colour (0xff0c0b18), 840, 540, false);
    g.setGradientFill (bg);
    g.fillRect (getLocalBounds());
    g.setColour (juce::Colour (0xffad6aff));
    g.fillRoundedRectangle (22.0f, 20.0f, 796.0f, 84.0f, 15.0f);
    g.setColour (juce::Colour (0xff160f28));
    g.fillRoundedRectangle (25.0f, 23.0f, 790.0f, 78.0f, 13.0f);
    g.setColour (juce::Colours::white);
    g.setFont (juce::Font (juce::FontOptions (30.0f, juce::Font::bold)));
    g.drawText ("VST3Effekt+", 45, 29, 420, 42, juce::Justification::centredLeft);
    g.setColour (juce::Colour (0xffb6a5d5));
    g.setFont (juce::Font (juce::FontOptions (15.0f)));
    g.drawText ("for YungOS by 29yuro", 480, 34, 305, 32, juce::Justification::centredRight);
    g.setColour (juce::Colour (0xff28213e));
    g.fillRoundedRectangle (22.0f, 124.0f, 796.0f, 380.0f, 15.0f);
    g.setColour (juce::Colour (0xffaaa0c4));
    g.setFont (juce::Font (juce::FontOptions (14.0f, juce::Font::bold)));
    g.drawText ("VOCAL FX  /  EFFECT CHAIN", 42, 132, 450, 26, juce::Justification::centredLeft);
    g.setFont (juce::Font (juce::FontOptions (12.0f)));
    g.drawText ("AU  •  Logic Pro  •  macOS", 42, 511, 380, 22, juce::Justification::centredLeft);
}

void VST3EffektPlusAudioProcessorEditor::resized()
{
    const int columns = 4, cellWidth = 180, cellHeight = 104;
    for (int i = 0; i < static_cast<int> (knobs.size()); ++i)
    {
        const int x = 45 + (i % columns) * 195;
        const int y = 165 + (i / columns) * 108;
        knobs[static_cast<size_t> (i)]->slider.setBounds (x, y, cellWidth, cellHeight - 20);
        knobs[static_cast<size_t> (i)]->label.setBounds (x, y + 80, cellWidth, 22);
    }
}

juce::AudioProcessorEditor* VST3EffektPlusAudioProcessor::createEditor()
{
    return new VST3EffektPlusAudioProcessorEditor (*this);
}
