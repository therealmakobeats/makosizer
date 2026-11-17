#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

#include <array>
#include <atomic>
#include <vector>

class MakosizerAudioProcessor final : public juce::AudioProcessor
{
public:
    MakosizerAudioProcessor();
    ~MakosizerAudioProcessor() override = default;

    const juce::String getName() const override { return "Makosizer"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    void processBlockBypassed(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;

    bool hasEditor() const override { return true; }
    juce::AudioProcessorEditor* createEditor() override;
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;
    std::array<std::atomic<float>, 5> meters;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    float getParameter(const char* id) const noexcept;
    static float softClip(float x) noexcept;

    double currentSampleRate = 44100.0;
    juce::dsp::IIR::Filter<float> hpfL, hpfR, lpfL, lpfR, noiseL, noiseR;
    juce::Random random;
    std::vector<float> delayL, delayR;
    int delayWrite = 0;
    float wowPhase = 0.0f, flutterPhase = 0.0f, slowPhase = 0.0f;
    float crushPhaseL = 0.0f, crushPhaseR = 0.0f, heldL = 0.0f, heldR = 0.0f;
    float envelope = 0.0f, dustCountdown = 0.0f, dustL = 0.0f, dustR = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MakosizerAudioProcessor)
};
