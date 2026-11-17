#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <juce_audio_processors/juce_audio_processors.h>

namespace
{
    template <typename T>
    T clamp(T value, T minValue, T maxValue)
    {
        return juce::jlimit(minValue, maxValue, value);
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout MakosizerAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "Drive", "Drive", 0.0f, 1.0f, 0.55f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "Destroy", "Destroy", 0.0f, 1.0f, 0.38f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "Vinyl", "Vinyl", 0.0f, 1.0f, 0.6f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "Wow", "Wow", 0.1f, 2.0f, 0.8f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "Flutter", "Flutter", 4.0f, 20.0f, 10.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "Noise", "Noise", 0.0f, 1.0f, 0.42f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "Dust", "Dust", 0.0f, 1.0f, 0.22f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "Bits", "Bits", 4.0f, 16.0f, 12.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "Rate", "Rate", 4000.0f, 44100.0f, 22050.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "Comp", "Comp", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "Width", "Width", 0.0f, 2.0f, 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "Mix", "Mix", 0.0f, 1.0f, 0.75f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "HPF", "HPF", 20.0f, 20000.0f, 120.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "LPF", "LPF", 2000.0f, 20000.0f, 12000.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "Output", "Output", -24.0f, 12.0f, 0.0f));

    return { params.begin(), params.end() };
}

MakosizerAudioProcessor::MakosizerAudioProcessor()
    : juce::AudioProcessor(BusesProperties()
            .withInput("Input", juce::AudioChannelSet::stereo(), true)
            .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

MakosizerAudioProcessor::~MakosizerAudioProcessor() = default;

std::atomic<float>* MakosizerAudioProcessor::readParameterValue(const juce::String& parameterID)
{
    auto* param = apvts.getRawParameterValue(parameterID);
    return param;
}

void MakosizerAudioProcessor::prepareToPlay(double sampleRate, int /*samplesPerBlock*/)
{
    juce::ignoreUnused(sampleRate);
}

void MakosizerAudioProcessor::processBlockBypassed(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    juce::ignoreUnused(buffer);
}

void MakosizerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    if (numSamples == 0)
        return;

    const auto drive = apvts.getRawParameterValue("Drive")->load();
    const auto destroy = apvts.getRawParameterValue("Destroy")->load();
    const auto vinyl = apvts.getRawParameterValue("Vinyl")->load();
    const auto wow = apvts.getRawParameterValue("Wow")->load();
    const auto flutter = apvts.getRawParameterValue("Flutter")->load();
    const auto noise = apvts.getRawParameterValue("Noise")->load();
    const auto dust = apvts.getRawParameterValue("Dust")->load();
    const auto bits = apvts.getRawParameterValue("Bits")->load();
    const auto rate = apvts.getRawParameterValue("Rate")->load();
    const auto comp = apvts.getRawParameterValue("Comp")->load();
    const auto width = apvts.getRawParameterValue("Width")->load();
    const auto mix = apvts.getRawParameterValue("Mix")->load();
    const auto hpf = apvts.getRawParameterValue("HPF")->load();
    const auto lpf = apvts.getRawParameterValue("LPF")->load();
    const auto output = apvts.getRawParameterValue("Output")->load();

    const float destroyAmount = juce::jlimit(0.0f, 1.0f, destroy);
    const float driveAmount = juce::jlimit(0.0f, 1.0f, drive);

    float channel0Gain = 1.0f;
    float channel1Gain = 1.0f;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        float left = buffer.getSample(0, sample);
        float right = numChannels > 1 ? buffer.getSample(1, sample) : left;

        const float leftDry = left;
        const float rightDry = right;

        const float destroyBlend = 0.1f + destroyAmount * 1.8f;
        const float saturationDrive = 1.0f + driveAmount * 12.0f + destroyBlend * 10.0f;

        left = std::tanh(left * saturationDrive);
        right = std::tanh(right * saturationDrive);

        const float wowFreq = juce::jlimit(0.1f, 2.0f, wow);
        const float flutterFreq = juce::jlimit(4.0f, 20.0f, flutter);
        const float bitDepth = juce::jlimit(4.0f, 16.0f, bits);
        const float sampleRateHz = juce::jlimit(4000.0f, 44100.0f, rate);

        const float mod = 1.0f + (wowFreq * 0.06f) * std::sin((2.0f * float_M_PI * wowFreq * sample) / 44100.0f)
            + (flutterFreq * 0.02f) * std::sin((2.0f * float_M_PI * flutterFreq * sample) / 44100.0f);

        left *= mod;
        right *= mod;

        const float qDepth = std::pow(2.0f, bitDepth);
        const float step = 1.0f / qDepth;
        left = std::round(left / step) * step;
        right = std::round(right / step) * step;

        if (sampleRateHz < 44100.0f)
        {
            const float downSampleRatio = sampleRateHz / 44100.0f;
            const int stepSize = static_cast<int>(std::max(1.0f, 1.0f / downSampleRatio));
            if ((sample % stepSize) != 0)
            {
                left = leftDry;
                right = rightDry;
            }
        }

        const float hpfFreq = juce::jlimit(20.0f, 20000.0f, hpf);
        const float lpfFreq = juce::jlimit(2000.0f, 20000.0f, lpf);
        const float hp = std::max(0.0f, 1.0f - (hpfFreq / 20000.0f));
        const float lp = std::min(1.0f, lpfFreq / 20000.0f);
        left = left * (0.45f + lp) + leftDry * (0.18f + hp);
        right = right * (0.45f + lp) + rightDry * (0.18f + hp);

        const float noiseGain = 0.05f + noise * 0.25f + dust * 0.15f;
        const float noiseSample = (std::sin((sample + 1) * 0.93f) + std::sin((sample + 2) * 1.77f)) * noiseGain * 0.15f;
        left += noiseSample;
        right += noiseSample * 0.9f;

        const float mid = (left + right) * 0.5f;
        const float side = (left - right) * 0.5f;
        const float widthFactor = juce::jlimit(0.0f, 2.0f, width);
        left = mid + side * widthFactor;
        right = mid - side * widthFactor;

        const float wetMix = juce::jlimit(0.0f, 1.0f, mix);
        left = leftDry * (1.0f - wetMix) + left * wetMix;
        right = rightDry * (1.0f - wetMix) + right * wetMix;

        const float compGain = 1.0f + comp * 0.85f;
        const float limiterThreshold = 0.95f - comp * 0.25f;
        const float outputDb = output;
        const float outputGain = std::pow(10.0f, outputDb / 20.0f);

        left = std::tanh(left * compGain) * limiterThreshold * outputGain;
        right = std::tanh(right * compGain) * limiterThreshold * outputGain;

        left = juce::jlimit(-1.0f, 1.0f, left);
        right = juce::jlimit(-1.0f, 1.0f, right);

        buffer.setSample(0, sample, left);
        if (numChannels > 1)
            buffer.setSample(1, sample, right);
    }
}

juce::AudioProcessorEditor* MakosizerAudioProcessor::createEditor()
{
    return new MakosizerAudioProcessorEditor(*this);
}

void MakosizerAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MakosizerAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml = getXmlFromBinary(data, sizeInBytes);
    if (xml != nullptr)
    {
        auto tree = juce::ValueTree::fromXml(*xml);
        if (! tree.isValid())
            return;

        apvts.replaceState(tree);
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MakosizerAudioProcessor();
}
