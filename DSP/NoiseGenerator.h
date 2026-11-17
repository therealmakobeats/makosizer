#pragma once

#include <juce_dsp/juce_dsp.h>

class NoiseGenerator
{
public:
    void prepare(double sampleRate)
    {
        fs = static_cast<float>(sampleRate > 0.0 ? sampleRate : 44100.0);
        random = juce::Random(0x00C0FFEE);
    }

    void reset() { random = juce::Random(0x00C0FFEE); }
    void setVinylLevel(float value) { vinyl = juce::jlimit(0.0f, 1.0f, value); }
    void setDust(float value) { dust = juce::jlimit(0.0f, 1.0f, value); }

    float process(float sample)
    {
        const float vinylNoise = random.nextFloat() * 2.0f - 1.0f;
        const float tapeNoise = random.nextFloat() * 2.0f - 1.0f;
        const float dustNoise = random.nextFloat() * 2.0f - 1.0f;
        const float click = random.nextFloat() > 0.998f
            ? (random.nextFloat() * 2.0f - 1.0f) * 0.6f
            : 0.0f;
        const float combined = sample + vinyl * vinylNoise * 0.18f
            + dust * dustNoise * 0.18f + tapeNoise * 0.05f + click;
        return juce::jlimit(-1.0f, 1.0f, combined);
    }

private:
    float fs = 44100.0f;
    float vinyl = 0.0f;
    float dust = 0.0f;
    juce::Random random { 0x00C0FFEE };
};
