#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include <array>

#include "Components/SharkLogo.h"
#include "LedMeter.h"
#include "LookAndFeel.h"
#include "PluginProcessor.h"

class MakosizerAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                           private juce::Timer
{
public:
    explicit MakosizerAudioProcessorEditor(MakosizerAudioProcessor& processor);
    ~MakosizerAudioProcessorEditor() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

private:
    void configureKnob(juce::Slider& slider, double min, double max, double step);
    void drawScrew(juce::Graphics& g, float x, float y);

    MakosizerAudioProcessor& processor;
    MakosizerLookAndFeel lookAndFeel;

    std::array<juce::TextButton, 9> presetButtons;
    juce::TextButton loadButton { "LOAD" };
    juce::TextButton saveButton { "SAVE" };
    std::array<juce::TextButton, 4> oversampleButtons;

    juce::Slider driveKnob, destroyKnob, vinylKnob, wowKnob, flutterKnob;
    juce::Slider noiseKnob, dustKnob, crushKnob, compKnob, widthKnob, mixKnob;
    juce::Slider hpfKnob, lpfKnob, ceilingKnob;

    LedMeter meter;
    SharkLogo sharkLogo;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> destroyAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> vinylAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> wowAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> flutterAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> noiseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dustAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> crushAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> compAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> widthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> hpfAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lpfAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ceilingAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MakosizerAudioProcessorEditor)
};
