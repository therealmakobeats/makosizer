#pragma once

#include <juce_dsp/juce_dsp.h>

class SoftClipper
{
public:
    void prepare(double sampleRate) { juce::ignoreUnused(sampleRate); }
    void reset() {}

    void setDrive(float value) { drive = juce::jlimit(0.0f, 6.0f, value); }

    float process(float sample)
    {
        const float scaled = sample * drive;
        return std::tanh(scaled);
    }

private:
    float drive = 1.0f;
};
