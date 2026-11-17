#pragma once

#include <juce_dsp/juce_dsp.h>

class WowFlutter
{
public:
    void prepare(double sampleRate) { fs = static_cast<float>(sampleRate); }
    void reset() { wowPhase = 0.0f; flutterPhase = 0.0f; }

    void setWow(float value) { wow = juce::jlimit(0.1f, 2.0f, value); }
    void setFlutter(float value) { flutter = juce::jlimit(4.0f, 20.0f, value); }

    float process(float sample)
    {
        const float wowDepth = 0.12f + wow * 0.14f;
        const float flutterDepth = 0.04f + flutter * 0.012f;

        wowPhase += 2.0f * float_M_PI * wow / fs;
        flutterPhase += 2.0f * float_M_PI * flutter / fs;

        const float wowMod = 1.0f + wowDepth * std::sin(wowPhase);
        const float flutterMod = 1.0f + flutterDepth * std::sin(flutterPhase);

        return sample * wowMod * flutterMod;
    }

private:
    float fs = 44100.0f;
    float wow = 0.8f;
    float flutter = 10.0f;
    float wowPhase = 0.0f;
    float flutterPhase = 0.0f;
};
