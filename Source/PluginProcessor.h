#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_data_structures/juce_data_structures.h>

#include "DSP/Saturator.h"
#include "DSP/VinylSim.h"
#include "DSP/BitCrusher.h"
#include "DSP/WowFlutter.h"
#include "DSP/NoiseGenerator.h"
#include "DSP/StereoWidth.h"
#include "DSP/SoftClipper.h"
#include "DSP/Limiter.h"

class MakosizerAudioProcessor : public juce::AudioProcessor
{
public:
    MakosizerAudioProcessor();
    ~Mako...