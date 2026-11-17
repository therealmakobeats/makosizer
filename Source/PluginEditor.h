#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "PluginProcessor.h"
#include "LookAndFeel.h"
#include "LedMeter.h"
#include "DestroyKnob.h"
#include "Components/SharkLogo.h"

class MakosizerAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                           private juce::Timer
{
public:
    explicit MakosizerAudioProcessorEditor(MakosizerAudioProcessor& processor);
    ~MakosizerAudioProcessorEditor() override;

    void paint(juce::Graphics& graphics) override;
    void resized() override;
    void timerCallback() override;

private:
    MakosizerAudioProcessor& processor;
    MakosizerLookAndFeel lookAndFeel;

    juce::ComboBox presetBrowser;
    juce::TextButton loadButton { "Load" };
    juce::TextButton saveButton { "Save" };
    juce::TextButton factoryButton { "Factory" };

    juce::Slider destroyKnob, driveKnob, vinylKnob, wowKnob, flutterKnob;
    juce::Slider noiseKnob, dustKnob, crushKnob, compKnob, widthKnob, mixKnob;
    juce::Label titleLabel, subtitleLabel, presetLabel;
    LedMeter meter;
    SharkLogo sharkLogo;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> destroyAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> vinylAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> wowAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> flutterAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> noiseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dustAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> crushAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> compAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> widthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MakosizerAudioProcessorEditor)
};
