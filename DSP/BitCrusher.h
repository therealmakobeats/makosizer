#pragma once

#include <juce_dsp/juce_dsp.h>

class BitCrusher
{
public:
    void prepare(double sampleRate) { fs = static_cast<float>(sampleRate); }
    void reset() { accumulator = 0.0f; }

    void setBits(float bitsValue)
    {
        bits = juce::jlimit(4.0f, 16.0f, bitsValue);
    }

    void setRate(float rateValue)
    {
        rate = juce::jlimit(4000.0f, 44100.0f, rateValue);
    }

    float process(float sample)
    {
        const float steps = std::pow(2.0f, bits);
        const float step = 1.0f / steps;
        const float quantised = std::round(sample / step) * step;

        accumulator += 1.0f;
        if (accumulator >= fs / rate)
        {
            accumulator = 0.0f;
            return quantised;
        }

        return sample;
    }

private:
    float fs = 44100.0f;
    float bits = 12.0f;
    float rate = 22050.0f;
    float accumulator = 0.0f;
};
