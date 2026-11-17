#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include <array>
#include <memory>

#include "PluginProcessor.h"

class MakosizerAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                            private juce::Timer
{
public:
    explicit MakosizerAudioProcessorEditor(MakosizerAudioProcessor& processor);
    ~MakosizerAudioProcessorEditor() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void timerCallback() override;
    void setSliderStyle(juce::Slider& slider, bool large = false);
    void applyPreset(int index);
    void setParameter(const char* id, float value);
    void drawMeter(juce::Graphics& g, int x, float db);

    MakosizerAudioProcessor& processor;
    std::array<juce::Slider, 14> sliders;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, 14> attachments;
    std::array<juce::TextButton, 9> presetButtons;
    std::array<juce::TextButton, 4> oversamplingButtons;
    juce::TextButton loadButton { "LOAD" }, saveButton { "SAVE" };
    int currentPreset = 0;
    juce::String activeText { "MASTERING READY" };
    int activeTextTicks = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MakosizerAudioProcessorEditor)
};
