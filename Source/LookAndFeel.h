#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class MakosizerLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    MakosizerLookAndFeel();

    void drawRotarySlider(juce::Graphics&, int, int, int, int, float, float, float, juce::Slider&) override;
    void drawComboBox(juce::Graphics&, int, int, bool, int, int, int, int, juce::ComboBox&) override;
    void drawButtonBackground(juce::Graphics&, juce::Button&, const juce::Colour&, bool, bool) override;
    void drawTextEditorOutline(juce::Graphics&, int, int, juce::TextEditor&) override;
};
