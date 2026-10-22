cmake_minimum_required(VERSION 3.22)
project(VST3EffektPlus VERSION 1.0.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(FetchContent)
FetchContent_Declare(JUCE GIT_REPOSITORY https://github.com/juce-framework/JUCE.git GIT_TAG 8.0.12 GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)
juce_add_plugin(VST3EffektPlus
    COMPANY_NAME "29yuro"
    BUNDLE_ID "com.yungos.vst3effektplus"
    PLUGIN_MANUFACTURER_CODE Yuro
    PLUGIN_CODE YfxP
    FORMATS AU VST3
    PRODUCT_NAME "VST3Effekt+"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    VST3_CATEGORIES Fx
    AU_MAIN_TYPE kAudioUnitType_Effect
)
target_sources(VST3EffektPlus PRIVATE Source/PluginProcessor.cpp Source/PluginEditor.cpp)
target_compile_definitions(VST3EffektPlus PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)
target_link_libraries(VST3EffektPlus PRIVATE juce::juce_audio_utils juce::juce_dsp PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_warning_flags)
