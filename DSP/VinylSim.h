#pragma once

#include <juce_dsp/juce_dsp.h>

class VinylSim
{
public:
    void prepare(double sampleRate)
    {
        fs = static_cast<float>(sampleRate);
        lpState = 0.0f;
        hpState = 0.0f;
    }

    void reset() { lpState = 0.0f; hpState = 0.0f; }

    void setVinyl(float value) { vinyl = juce::jlimit(0.0f, 1.0f, value); }
    void setDust(float value) { dust = juce::jlimit(0.0f, 1.0f, value); }

    float process(float sample)
    {
        const float cutoff = 6000.0f + vinyl * 12000.0f;
        const float alpha = 1.0f / (1.0f + fs / cutoff);
        lpState += alpha * (sample - lpState);

        const float hpAlpha = 1.0f / (1.0f + fs / 80.0f);
        hpState += hpAlpha * (lpState - hpState);

        const float noise = (dust * 0.06f) * static_cast<float>((std::sin(sample * 17.0f) + std::sin(sample * 41.0f)) * 0.5);
        return hpState * (0.7f + vinyl) + noise;
    }

private:
    float fs = 44100.0f;
    float lpState = 0.0f;
    float hpState = 0.0f;
    float vinyl = 0.5f;
    float dust = 0.0f;
};
