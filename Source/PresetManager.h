#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

class PresetManager
{
public:
    explicit PresetManager(juce::AudioProcessorValueTreeState& state)
        : parameters(state)
    {
    }

    bool savePreset(const juce::File& file) const;
    bool loadPreset(const juce::File& file);

private:
    juce::AudioProcessorValueTreeState& parameters;
};
