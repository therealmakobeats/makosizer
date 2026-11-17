#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class Saturator
{
public:
    void prepare(double sampleRate) { juce::ignoreUnused(sampleRate); }
    void reset() {}

    void setDrive(float value) { drive = juce::jlimit(0.0f, 4.0f, value); }
    void setMix(float value) { mix = juce::jlimit(0.0f, 1.0f, value); }

    float process(float sample)
    {
        const float saturation = std::tanh(sample * (1.0f + drive * 8.0f));
        return sample * (1.0f - mix) + saturation * mix;
    }

private:
    float drive = 1.0f;
    float mix = 0.7f;
};
