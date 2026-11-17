#pragma once

#include <juce_dsp/juce_dsp.h>

class StereoWidth
{
public:
    void prepare(double sampleRate) { juce::ignoreUnused(sampleRate); }
    void reset() {}

    void setWidth(float value) { width = juce::jlimit(0.0f, 2.0f, value); }

    float processLeft(float left, float right)
    {
        const float mid = (left + right) * 0.5f;
        const float side = (left - right) * 0.5f;
        return mid + side * width;
    }

    float processRight(float left, float right)
    {
        const float mid = (left + right) * 0.5f;
        const float side = (left - right) * 0.5f;
        return mid - side * width;
    }

private:
    float width = 1.0f;
};
