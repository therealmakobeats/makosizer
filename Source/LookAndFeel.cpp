#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class MakosizerLookAndFeel : public juce::LookAndFeel_V4
{
public:
    MakosizerLookAndFeel();

    void drawRotarySlider(juce::Graphics& g,
                          int x,
                          int y,
                          int width,
                          int height,
                          float sliderPos,
                          float rotaryStartAngle,
                          float rotaryEndAngle,
                          juce::Slider&) override;

    void drawComboBox(juce::Graphics& g,
                      int width,
                      int height,
                      bool isButtonDown,
                      int buttonX,
                      int buttonY,
                      int buttonW,
                      int buttonH,
                      juce::ComboBox& box) override;

    void drawButtonBackground(juce::Graphics& g,
                              juce::Button& button,
                              const juce::Colour& backgroundColour,
                              bool isMouseOver,
                              bool isButtonDown) override;

    void drawTextEditorOutline(juce::Graphics& g,
                              int width,
                              int height,
                              juce::TextEditor&) override;
};
