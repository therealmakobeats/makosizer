#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class LedMeter final : public juce::Component
{
public:
    LedMeter();
    void paint(juce::Graphics& graphics) override;
    void resized() override;
    void setLevels(float leftDb, float rightDb);
    void setPeakHold(bool enabled) noexcept { peakHold = enabled; }

private:
    float leftLevel = 0.0f;
    float rightLevel = 0.0f;
    float leftPeak = 0.0f;
    float rightPeak = 0.0f;
    bool peakHold = true;
};
