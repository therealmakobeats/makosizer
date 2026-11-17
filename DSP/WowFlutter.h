#pragma once

#include <juce_dsp/juce_dsp.h>
#include <cmath>

class WowFlutter
{
public:
    void prepare(double sampleRate)
    {
        fs = static_cast<float>(sampleRate > 0.0 ? sampleRate : 44100.0);
        reset();
    }

    void reset() { wowPhase = 0.0f; flutterPhase = 0.0f; }
    void setWow(float value) { wow = juce::jlimit(0.1f, 2.0f, value); }
    void setFlutter(float value) { flutter = juce::jlimit(4.0f, 20.0f, value); }

    float process(float sample)
    {
        constexpr float twoPi = juce::MathConstants<float>::twoPi;
        wowPhase = std::fmod(wowPhase + twoPi * wow / fs, twoPi);
        flutterPhase = std::fmod(flutterPhase + twoPi * flutter / fs, twoPi);
        const float wowMod = 1.0f + (0.12f + wow * 0.14f) * std::sin(wowPhase);
        const float flutterMod = 1.0f + (0.04f + flutter * 0.012f) * std::sin(flutterPhase);
        return sample * wowMod * flutterMod;
    }

private:
    float fs = 44100.0f;
    float wow = 0.8f;
    float flutter = 10.0f;
    float wowPhase = 0.0f;
    float flutterPhase = 0.0f;
};
