#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class DestroyKnob : public juce::Slider
{
public:
    DestroyKnob();

    void paint(juce::Graphics& g) override;
};
