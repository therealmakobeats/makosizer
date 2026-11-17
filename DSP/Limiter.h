#pragma once

#include <juce_dsp/juce_dsp.h>

class Limiter
{
public:
    void prepare(double sampleRate) { fs = static_cast<float>(sampleRate); }
    void reset() { gain = 1.0f; }

    void setThreshold(float value) { threshold = juce::jlimit(-12.0f, 0.0f, value); }
    void setCeiling(float value) { ceiling = juce::jlimit(-6.0f, 0.0f, value); }
    void setRelease(float value) { release = juce::jlimit(0.001f, 0.25f, value); }

    float process(float sample)
    {
        float absSample = std::abs(sample);
        const float peak = juce::jmax(0.0001f, absSample);
        const float dB = 20.0f * std::log10(peak + 0.000001f);
        const float limitDb = juce::jmax(dB, threshold);
        const float ceilingGain = std::pow(10.0f, ceiling / 20.0f);
        const float gainTarget = std::min(1.0f, ceilingGain / std::max(0.0001f, limitDb));
        gain = gain * (1.0f - release) + gainTarget * release;
        return sample * gain;
    }

private:
    float fs = 44100.0f;
    float threshold = -4.0f;
    float ceiling = -0.1f;
    float release = 0.05f;
    float gain = 1.0f;
};
