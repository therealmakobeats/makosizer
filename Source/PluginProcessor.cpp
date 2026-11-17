#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <cmath>
#include <memory>
#include <vector>

namespace
{
using FloatParam = juce::AudioParameterFloat;

std::unique_ptr<juce::RangedAudioParameter> makeFloat(const juce::String& id,
                                                       const juce::String& name,
                                                       float minimum, float maximum,
                                                       float initial,
                                                       const juce::String& suffix = {})
{
    auto attributes = juce::AudioParameterFloatAttributes().withLabel(suffix);
    return std::make_unique<FloatParam>(juce::ParameterID { id, 1 }, name,
                                        juce::NormalisableRange<float>(minimum, maximum),
                                        initial, attributes);
}
}

juce::AudioProcessorValueTreeState::ParameterLayout MakosizerAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;
    parameters.push_back(makeFloat("Destroy", "Destroy", 0.0f, 100.0f, 85.0f, "%"));
    parameters.push_back(makeFloat("Drive", "Drive", 0.0f, 30.0f, 18.0f, "dB"));
    parameters.push_back(makeFloat("VinylSim", "Vinyl Sim", 0.0f, 100.0f, 78.0f, "%"));
    parameters.push_back(makeFloat("Wow", "Wow", 0.0f, 100.0f, 20.0f, "%"));
    parameters.push_back(makeFloat("Flutter", "Flutter", 0.0f, 100.0f, 28.0f, "%"));
    parameters.push_back(makeFloat("Noise", "Noise", 0.0f, 30.0f, 12.0f, "dB"));
    parameters.push_back(makeFloat("Dust", "Dust", 0.0f, 100.0f, 36.0f, "%"));
    parameters.push_back(makeFloat("Crush", "Crush", 0.0f, 100.0f, 70.0f, "%"));
    parameters.push_back(makeFloat("Comp", "Comp", 0.0f, 100.0f, 80.0f, "%"));
    parameters.push_back(makeFloat("HPF", "HPF", 20.0f, 1000.0f, 80.0f, "Hz"));
    parameters.push_back(makeFloat("LPF", "LPF", 1000.0f, 20000.0f, 12000.0f, "Hz"));
    parameters.push_back(makeFloat("Ceiling", "Ceiling", -3.0f, 0.0f, -0.1f, "dB"));
    parameters.push_back(makeFloat("Width", "Width", 0.0f, 200.0f, 120.0f, "%"));
    parameters.push_back(makeFloat("Mix", "Mix", 0.0f, 100.0f, 100.0f, "%"));
    parameters.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "Oversampling", 1 }, "Oversampling",
        juce::StringArray { "1x", "2x", "4x", "8x" }, 1));
    return { parameters.begin(), parameters.end() };
}

MakosizerAudioProcessor::MakosizerAudioProcessor()
    : juce::AudioProcessor(BusesProperties()
                               .withInput("Input", juce::AudioChannelSet::stereo(), true)
                               .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
{
    for (auto& meter : meters)
        meter.store(-60.0f, std::memory_order_relaxed);
}

float MakosizerAudioProcessor::getParameter(const char* id) const noexcept
{
    if (const auto* parameter = apvts.getRawParameterValue(id))
        return parameter->load(std::memory_order_relaxed);
    return 0.0f;
}

float MakosizerAudioProcessor::softClip(float x) noexcept
{
    return std::tanh(x);
}

void MakosizerAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(samplesPerBlock);
    currentSampleRate = sampleRate > 0.0 ? sampleRate : 44100.0;
    juce::dsp::ProcessSpec spec { currentSampleRate, 1, 1 };
    for (auto* filter : { &hpfL, &hpfR, &lpfL, &lpfR, &noiseL, &noiseR })
        filter->prepare(spec);
    delayL.assign(4096, 0.0f);
    delayR.assign(4096, 0.0f);
    delayWrite = 0;
    reset();
}

void MakosizerAudioProcessor::releaseResources()
{
    delayL.clear();
    delayR.clear();
}

bool MakosizerAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto input = layouts.getMainInputChannelSet();
    const auto output = layouts.getMainOutputChannelSet();
    return input == output && (output == juce::AudioChannelSet::mono() || output == juce::AudioChannelSet::stereo());
}

void MakosizerAudioProcessor::processBlockBypassed(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ignoreUnused(midi);
    float peakL = 0.0f, peakR = 0.0f;
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        peakL = juce::jmax(peakL, std::abs(buffer.getSample(0, i)));
        if (buffer.getNumChannels() > 1)
            peakR = juce::jmax(peakR, std::abs(buffer.getSample(1, i)));
    }
    meters[0].store(juce::Decibels::gainToDecibels(peakL, -60.0f), std::memory_order_relaxed);
    meters[1].store(juce::Decibels::gainToDecibels(buffer.getNumChannels() > 1 ? peakR : peakL, -60.0f), std::memory_order_relaxed);
    meters[2].store(0.0f, std::memory_order_relaxed);
    meters[3].store(meters[0].load(std::memory_order_relaxed), std::memory_order_relaxed);
    meters[4].store(meters[1].load(std::memory_order_relaxed), std::memory_order_relaxed);
}

void MakosizerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    juce::ignoreUnused(midi);
    const int channels = juce::jmin(buffer.getNumChannels(), getTotalNumOutputChannels());
    const int frames = buffer.getNumSamples();
    if (channels == 0 || frames == 0)
        return;

    const float hpfHz = getParameter("HPF");
    const float lpfHz = getParameter("LPF");
    const float sr = static_cast<float>(currentSampleRate);
    const float safeLpf = juce::jlimit(20.0f, sr * 0.45f, lpfHz);
    *hpfL.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sr, hpfHz, 0.7071f);
    *hpfR.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sr, hpfHz, 0.7071f);
    *lpfL.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sr, safeLpf, 0.7071f);
    *lpfR.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sr, safeLpf, 0.7071f);
    *noiseL.state = *juce::dsp::IIR::Coefficients<float>::makeBandPass(sr, 1500.0f, 0.9f);
    *noiseR.state = *juce::dsp::IIR::Coefficients<float>::makeBandPass(sr, 1500.0f, 0.9f);

    const float vinyl = getParameter("VinylSim") * 0.01f;
    const float destroy = getParameter("Destroy") * 0.01f;
    const float driveDb = getParameter("Drive");
    const float wow = getParameter("Wow") * 0.01f;
    const float flutter = getParameter("Flutter") * 0.01f;
    const float noiseAmount = getParameter("Noise");
    const float dustAmount = getParameter("Dust") * 0.01f;
    const float crush = getParameter("Crush") * 0.01f;
    const float comp = getParameter("Comp") * 0.01f;
    const float width = getParameter("Width") * 0.01f;
    const float mix = getParameter("Mix") * 0.01f;
    const float ceiling = juce::Decibels::decibelsToGain(getParameter("Ceiling"));
    const float inputGain = juce::Decibels::decibelsToGain(driveDb + destroy * 16.0f);
    const float noiseGain = noiseAmount > 0.0f ? juce::Decibels::decibelsToGain(noiseAmount - 30.0f) : 0.0f;
    const float downsampleFactor = 1.0f + (crush + destroy * 0.35f) * 12.0f;
    const float bits = 16.0f - juce::jlimit(0.0f, 1.0f, crush + destroy * 0.35f) * 11.5f;
    const float quantLevels = std::pow(2.0f, bits);
    const float wowInc = 2.0f * juce::MathConstants<float>::pi * 0.55f / sr;
    const float flutterInc = 2.0f * juce::MathConstants<float>::pi * 6.8f / sr;
    const float slowInc = 2.0f * juce::MathConstants<float>::pi * 0.18f / sr;
    float inL = 0.0f, inR = 0.0f, outL = 0.0f, outR = 0.0f, grDb = 0.0f;
    const int ringSize = static_cast<int>(delayL.size());

    for (int i = 0; i < frames; ++i)
    {
        const float dryL = buffer.getSample(0, i);
        const float dryR = channels > 1 ? buffer.getSample(1, i) : dryL;
        inL = juce::jmax(inL, std::abs(dryL));
        inR = juce::jmax(inR, std::abs(dryR));
        float left = dryL, right = dryR;
        if (inputGain > 1.001f)
        {
            const float compensation = 1.0f / std::sqrt(1.0f + inputGain * 0.55f);
            left = (softClip(left * inputGain * 0.95f) * 0.85f
                    + std::copysign(left * left * inputGain * inputGain * 0.07f, left)) * compensation;
            right = (softClip(right * inputGain * 0.95f) * 0.85f
                     + std::copysign(right * right * inputGain * inputGain * 0.07f, right)) * compensation;
        }

        wowPhase = std::fmod(wowPhase + wowInc, juce::MathConstants<float>::twoPi);
        flutterPhase = std::fmod(flutterPhase + flutterInc, juce::MathConstants<float>::twoPi);
        slowPhase = std::fmod(slowPhase + slowInc, juce::MathConstants<float>::twoPi);
        const float wowEffective = juce::jlimit(0.0f, 1.5f, wow + destroy * 0.35f);
        const float flutterEffective = juce::jlimit(0.0f, 1.5f, flutter + destroy * 0.25f);
        const float baseDelay = 450.0f;
        const float wowMod = (std::sin(wowPhase) * 190.0f + std::sin(slowPhase) * 45.0f) * wowEffective;
        const float flutterMod = (std::sin(flutterPhase) * 36.0f + std::sin(flutterPhase * 1.63f) * 16.0f) * flutterEffective;
        const int delaySamplesL = juce::jlimit(1, ringSize - 1, static_cast<int>(baseDelay + wowMod + flutterMod));
        const int delaySamplesR = juce::jlimit(1, ringSize - 1, static_cast<int>(baseDelay + wowMod * 0.94f - flutterMod * 0.88f));
        if (ringSize > 0)
        {
            delayL[static_cast<size_t>(delayWrite)] = left;
            delayR[static_cast<size_t>(delayWrite)] = right;
            left = delayL[static_cast<size_t>((delayWrite - delaySamplesL + ringSize) % ringSize)];
            right = delayR[static_cast<size_t>((delayWrite - delaySamplesR + ringSize) % ringSize)];
            delayWrite = (delayWrite + 1) % ringSize;
        }

        const float effectiveCrush = juce::jlimit(0.0f, 1.0f, crush + destroy * 0.35f);
        if (effectiveCrush > 0.01f)
        {
            crushPhaseL += 1.0f;
            if (crushPhaseL >= downsampleFactor)
            {
                crushPhaseL -= downsampleFactor;
                heldL = juce::jlimit(-1.0f, 1.0f, std::round(left * quantLevels) / quantLevels);
            }
            crushPhaseR += 1.0f;
            if (crushPhaseR >= downsampleFactor)
            {
                crushPhaseR -= downsampleFactor;
                heldR = juce::jlimit(-1.0f, 1.0f, std::round(right * quantLevels) / quantLevels);
            }
            left = heldL;
            right = heldR;
        }

        const float detector = (std::abs(left) + std::abs(right)) * 0.5f;
        const float attack = std::exp(-1.0f / (0.004f * sr));
        const float release = std::exp(-1.0f / (0.25f * sr));
        envelope = (detector > envelope ? attack : release) * envelope + (1.0f - (detector > envelope ? attack : release)) * detector;
        const float threshold = juce::Decibels::decibelsToGain(-comp * 34.0f);
        float compGain = 1.0f;
        if (comp > 0.02f && envelope > threshold && envelope > 1.0e-6f)
        {
            const float ratio = 1.0f + comp * 15.0f;
            const float compressed = threshold * std::pow(envelope / threshold, 1.0f / ratio);
            compGain = compressed / envelope * juce::Decibels::decibelsToGain(comp * 6.0f);
            grDb = juce::jmin(grDb, juce::Decibels::gainToDecibels(compressed / envelope));
        }
        left *= compGain;
        right *= compGain;

        if (vinyl > 0.001f)
        {
            const float warmth = 1.0f + vinyl * 0.18f;
            left = softClip(left * warmth) * (1.0f - vinyl * 0.1f);
            right = softClip(right * warmth) * (1.0f - vinyl * 0.1f);
        }

        const float totalNoise = noiseGain * 0.035f + destroy * 0.008f;
        if (totalNoise > 0.0001f)
        {
            left += noiseL.processSample(random.nextFloat() * 2.0f - 1.0f) * totalNoise;
            right += noiseR.processSample(random.nextFloat() * 2.0f - 1.0f) * totalNoise * 0.95f;
        }
        const float effectiveDust = dustAmount + destroy * 0.2f;
        if (effectiveDust > 0.01f)
        {
            dustCountdown -= 1.0f;
            if (dustCountdown <= 0.0f)
            {
                dustCountdown = 420.0f - effectiveDust * 280.0f;
                const float chance = random.nextFloat();
                if (chance < effectiveDust * 0.55f)
                {
                    dustL = (random.nextBool() ? 1.0f : -1.0f) * (0.03f + chance * 0.12f) * effectiveDust;
                    dustR = (random.nextBool() ? 1.0f : -1.0f) * (0.03f + (1.0f - chance) * 0.12f) * effectiveDust;
                }
            }
            left += dustL;
            right += dustR;
            dustL *= 0.83f;
            dustR *= 0.83f;
        }

        left = lpfL.processSample(hpfL.processSample(left));
        right = lpfR.processSample(hpfR.processSample(right));
        const float mid = 0.5f * (left + right);
        const float side = 0.5f * (left - right) * width;
        left = mid + side;
        right = mid - side;
        const float ceilingGain = juce::jmax(0.1f, ceiling);
        left = juce::jlimit(-ceilingGain, ceilingGain, softClip(left / ceilingGain) * ceilingGain);
        right = juce::jlimit(-ceilingGain, ceilingGain, softClip(right / ceilingGain) * ceilingGain);
        left = juce::jlimit(-1.0f, 1.0f, dryL * (1.0f - mix) + left * mix);
        right = juce::jlimit(-1.0f, 1.0f, dryR * (1.0f - mix) + right * mix);
        buffer.setSample(0, i, left);
        if (channels > 1)
            buffer.setSample(1, i, right);
        outL = juce::jmax(outL, std::abs(left));
        outR = juce::jmax(outR, std::abs(right));
    }

    meters[0].store(juce::Decibels::gainToDecibels(inL, -60.0f), std::memory_order_relaxed);
    meters[1].store(juce::Decibels::gainToDecibels(inR, -60.0f), std::memory_order_relaxed);
    meters[2].store(grDb, std::memory_order_relaxed);
    meters[3].store(juce::Decibels::gainToDecibels(outL, -60.0f), std::memory_order_relaxed);
    meters[4].store(juce::Decibels::gainToDecibels(outR, -60.0f), std::memory_order_relaxed);
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

juce::AudioProcessorEditor* MakosizerAudioProcessor::createEditor()
{
    return new MakosizerAudioProcessorEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MakosizerAudioProcessor();
}
