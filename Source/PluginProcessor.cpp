#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <cmath>
#include <vector>

juce::AudioProcessorValueTreeState::ParameterLayout MakosizerAudioProcessor::createParameterLayout()
{
    using Parameter = juce::AudioParameterFloat;
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;
    parameters.push_back(std::make_unique<Parameter>("Drive", "Drive", 0.0f, 1.0f, 0.55f));
    parameters.push_back(std::make_unique<Parameter>("Destroy", "Destroy", 0.0f, 1.0f, 0.38f));
    parameters.push_back(std::make_unique<Parameter>("Vinyl", "Vinyl", 0.0f, 1.0f, 0.6f));
    parameters.push_back(std::make_unique<Parameter>("Wow", "Wow", 0.1f, 2.0f, 0.8f));
    parameters.push_back(std::make_unique<Parameter>("Flutter", "Flutter", 4.0f, 20.0f, 10.0f));
    parameters.push_back(std::make_unique<Parameter>("Noise", "Noise", 0.0f, 1.0f, 0.42f));
    parameters.push_back(std::make_unique<Parameter>("Dust", "Dust", 0.0f, 1.0f, 0.22f));
    parameters.push_back(std::make_unique<Parameter>("Bits", "Bits", 4.0f, 16.0f, 12.0f));
    parameters.push_back(std::make_unique<Parameter>("Rate", "Rate", 4000.0f, 44100.0f, 22050.0f));
    parameters.push_back(std::make_unique<Parameter>("Comp", "Comp", 0.0f, 1.0f, 0.5f));
    parameters.push_back(std::make_unique<Parameter>("Width", "Width", 0.0f, 2.0f, 1.0f));
    parameters.push_back(std::make_unique<Parameter>("Mix", "Mix", 0.0f, 1.0f, 0.75f));
    parameters.push_back(std::make_unique<Parameter>("HPF", "HPF", 20.0f, 20000.0f, 120.0f));
    parameters.push_back(std::make_unique<Parameter>("LPF", "LPF", 2000.0f, 20000.0f, 12000.0f));
    parameters.push_back(std::make_unique<Parameter>("Output", "Output", -24.0f, 12.0f, 0.0f));
    return { parameters.begin(), parameters.end() };
}

MakosizerAudioProcessor::MakosizerAudioProcessor()
    : juce::AudioProcessor(BusesProperties()
                               .withInput("Input", juce::AudioChannelSet::stereo(), true)
                               .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

void MakosizerAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(samplesPerBlock);
    currentSampleRate = sampleRate > 0.0 ? sampleRate : 44100.0;
}

bool MakosizerAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto input = layouts.getMainInputChannelSet();
    const auto output = layouts.getMainOutputChannelSet();
    return input == output && (output == juce::AudioChannelSet::mono() || output == juce::AudioChannelSet::stereo());
}

void MakosizerAudioProcessor::processBlockBypassed(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    const int channels = juce::jmin(buffer.getNumChannels(), getTotalNumOutputChannels());
    float peakLeft = 0.0f;
    float peakRight = 0.0f;
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        peakLeft = juce::jmax(peakLeft, std::abs(buffer.getSample(0, sample)));
        if (channels > 1)
            peakRight = juce::jmax(peakRight, std::abs(buffer.getSample(1, sample)));
    }
    meterLeftDb.store(juce::Decibels::gainToDecibels(peakLeft, -60.0f), std::memory_order_relaxed);
    meterRightDb.store(juce::Decibels::gainToDecibels(channels > 1 ? peakRight : peakLeft, -60.0f), std::memory_order_relaxed);
}

void MakosizerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    juce::ignoreUnused(midiMessages);

    const int channels = juce::jmin(buffer.getNumChannels(), getTotalNumOutputChannels());
    const int samples = buffer.getNumSamples();
    if (channels <= 0 || samples <= 0)
        return;

    const auto value = [this](const char* id) { return apvts.getRawParameterValue(id)->load(); };
    const float drive = value("Drive");
    const float destroy = value("Destroy");
    const float noise = value("Noise");
    const float width = value("Width");
    const float mix = value("Mix");
    const float comp = value("Comp");
    const float outputGain = juce::Decibels::decibelsToGain(value("Output"));
    const float saturation = 1.0f + 12.0f * drive + 10.0f * destroy;
    const float wet = juce::jlimit(0.0f, 1.0f, mix);
    const float finalGain = outputGain * (1.0f + comp * 0.2f);
    float peakLeft = 0.0f;
    float peakRight = 0.0f;

    for (int sample = 0; sample < samples; ++sample)
    {
        const float leftDry = buffer.getSample(0, sample);
        float left = std::tanh(leftDry * saturation);
        const float noiseSample = noise * 0.005f * std::sin(static_cast<float>(sample) * 0.731f);
        left += noiseSample;

        if (channels > 1)
        {
            const float rightDry = buffer.getSample(1, sample);
            float right = std::tanh(rightDry * saturation) + noiseSample;
            const float mid = 0.5f * (left + right);
            const float side = 0.5f * (left - right) * width;
            left = mid + side;
            right = mid - side;
            const float outRight = juce::jlimit(-1.0f, 1.0f, (rightDry * (1.0f - wet) + right * wet) * finalGain);
            buffer.setSample(1, sample, outRight);
            peakRight = juce::jmax(peakRight, std::abs(outRight));
        }

        const float outLeft = juce::jlimit(-1.0f, 1.0f, (leftDry * (1.0f - wet) + left * wet) * finalGain);
        buffer.setSample(0, sample, outLeft);
        peakLeft = juce::jmax(peakLeft, std::abs(outLeft));
    }

    for (int channel = channels; channel < buffer.getNumChannels(); ++channel)
        buffer.clear(channel, 0, samples);

    meterLeftDb.store(juce::Decibels::gainToDecibels(peakLeft, -60.0f), std::memory_order_relaxed);
    meterRightDb.store(juce::Decibels::gainToDecibels(channels > 1 ? peakRight : peakLeft, -60.0f), std::memory_order_relaxed);
    juce::ignoreUnused(currentSampleRate);
}

juce::AudioProcessorEditor* MakosizerAudioProcessor::createEditor()
{
    return new MakosizerAudioProcessorEditor(*this);
}

void MakosizerAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    if (const auto xml = apvts.copyState().createXml())
        copyXmlToBinary(*xml, destData);
}

void MakosizerAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
    {
        const auto tree = juce::ValueTree::fromXml(*xml);
        if (tree.isValid() && tree.hasType(apvts.state.getType()))
            apvts.replaceState(tree);
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MakosizerAudioProcessor();
}
