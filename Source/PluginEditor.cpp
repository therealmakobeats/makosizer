#include "PluginEditor.h"

#include <array>

namespace
{
const juce::Colour ink { 0xff080a09 };
const juce::Colour panel { 0xff171918 };
const juce::Colour paper { 0xffc7c1b4 };
const juce::Colour mutedMetal { 0xff8f897c };

const std::array<juce::String, 9> presetNames {
    "INIT", "SP303 VIBES", "DILLA DUST", "MPC CRUNCH", "CASSETTE MELT",
    "LOFI RADIO", "UNDERGROUND", "DIRTY MASTER", "CUSTOM"
};

void drawPanel(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    g.setColour(juce::Colour(0xff070808));
    g.fillRoundedRectangle(bounds.translated(0.0f, 2.0f), 8.0f);
    g.setColour(juce::Colour(0xff141615));
    g.fillRoundedRectangle(bounds, 8.0f);
    g.setColour(juce::Colour(0xff5d5a52));
    g.drawRoundedRectangle(bounds, 8.0f, 1.0f);
}
}

MakosizerAudioProcessorEditor::MakosizerAudioProcessorEditor(MakosizerAudioProcessor& p)
    : juce::AudioProcessorEditor(&p), processor(p)
{
    setLookAndFeel(&lookAndFeel);
    setSize(1536, 1024);
    setResizable(false, false);

    for (size_t i = 0; i < presetButtons.size(); ++i)
    {
        auto& button = presetButtons[i];
        button.setButtonText(presetNames[i]);
        button.setClickingTogglesState(true);
        button.setRadioGroupId(101, juce::dontSendNotification);
        button.setToggleState(i == 0, juce::dontSendNotification);
        button.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff191b1a));
        button.setColour(juce::TextButton::textColourOffId, paper);
        button.setColour(juce::TextButton::textColourOnId, juce::Colour(0xffffd6c9));
        addAndMakeVisible(button);
    }

    for (auto* button : { &loadButton, &saveButton })
    {
        button->setColour(juce::TextButton::buttonColourId, juce::Colour(0xff292a27));
        button->setColour(juce::TextButton::textColourOffId, paper);
        addAndMakeVisible(button);
    }

    const std::array<juce::String, 4> oversamplingNames { "1x", "2x", "4x", "8x" };
    for (size_t i = 0; i < oversampleButtons.size(); ++i)
    {
        auto& button = oversampleButtons[i];
        button.setButtonText(oversamplingNames[i]);
        button.setClickingTogglesState(true);
        button.setRadioGroupId(102, juce::dontSendNotification);
        button.setToggleState(i == 1, juce::dontSendNotification);
        button.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff222423));
        button.setColour(juce::TextButton::textColourOffId, paper);
        button.setColour(juce::TextButton::textColourOnId, juce::Colour(0xffff7770));
        addAndMakeVisible(button);
    }

    configureKnob(driveKnob, 0.0, 1.0, 0.001);
    configureKnob(destroyKnob, 0.0, 1.0, 0.001);
    configureKnob(vinylKnob, 0.0, 1.0, 0.001);
    configureKnob(wowKnob, 0.1, 2.0, 0.01);
    configureKnob(flutterKnob, 4.0, 20.0, 0.01);
    configureKnob(noiseKnob, 0.0, 1.0, 0.001);
    configureKnob(dustKnob, 0.0, 1.0, 0.001);
    configureKnob(crushKnob, 4.0, 16.0, 0.01);
    configureKnob(compKnob, 0.0, 1.0, 0.001);
    configureKnob(widthKnob, 0.0, 2.0, 0.001);
    configureKnob(mixKnob, 0.0, 1.0, 0.001);
    configureKnob(hpfKnob, 20.0, 20000.0, 1.0);
    configureKnob(lpfKnob, 2000.0, 20000.0, 1.0);
    configureKnob(ceilingKnob, -24.0, 12.0, 0.1);

    for (auto* slider : { &driveKnob, &destroyKnob, &vinylKnob, &wowKnob, &flutterKnob,
                          &noiseKnob, &dustKnob, &crushKnob, &compKnob, &widthKnob,
                          &mixKnob, &hpfKnob, &lpfKnob, &ceilingKnob })
        addAndMakeVisible(slider);

    auto& state = processor.apvts;
    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "Drive", driveKnob);
    destroyAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "Destroy", destroyKnob);
    vinylAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "Vinyl", vinylKnob);
    wowAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "Wow", wowKnob);
    flutterAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "Flutter", flutterKnob);
    noiseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "Noise", noiseKnob);
    dustAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "Dust", dustKnob);
    crushAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "Bits", crushKnob);
    compAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "Comp", compKnob);
    widthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "Width", widthKnob);
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "Mix", mixKnob);
    hpfAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "HPF", hpfKnob);
    lpfAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "LPF", lpfKnob);
    ceilingAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "Output", ceilingKnob);

    addAndMakeVisible(sharkLogo);
    addAndMakeVisible(meter);
    startTimerHz(15);
}

MakosizerAudioProcessorEditor::~MakosizerAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

void MakosizerAudioProcessorEditor::configureKnob(juce::Slider& slider, double min, double max, double step)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 72, 20);
    slider.setRange(min, max, step);
    slider.setColour(juce::Slider::textBoxTextColourId, paper);
    slider.setColour(juce::Slider::textBoxBackgroundColourId, ink);
    slider.setColour(juce::Slider::textBoxOutlineColourId, mutedMetal);
}

void MakosizerAudioProcessorEditor::drawScrew(juce::Graphics& g, float x, float y)
{
    g.setColour(juce::Colour(0xff050606));
    g.fillEllipse(x - 13.0f, y - 13.0f, 26.0f, 26.0f);
    g.setColour(juce::Colour(0xffa19b8e));
    g.drawEllipse(x - 10.0f, y - 10.0f, 20.0f, 20.0f, 1.2f);
    g.setColour(juce::Colour(0xff292a27));
    g.drawLine(x - 5.0f, y + 5.0f, x + 5.0f, y - 5.0f, 2.0f);
}

void MakosizerAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(ink);
    const auto frame = getLocalBounds().toFloat().reduced(8.0f);
    juce::ColourGradient metal(juce::Colour(0xffb7b0a3), 0.0f, 0.0f,
                               juce::Colour(0xff514b45), 0.0f, static_cast<float>(getHeight()), false);
    g.setGradientFill(metal);
    g.fillRoundedRectangle(frame, 12.0f);
    g.setColour(juce::Colour(0xff060707));
    g.drawRoundedRectangle(frame, 12.0f, 4.0f);

    // Scuffed, brushed-metal grain. Deterministic so the finish remains still while controls move.
    juce::Random grain(0x4d414b4f);
    g.setColour(juce::Colour(0x29000000));
    for (int i = 0; i < 520; ++i)
    {
        const float x = 14.0f + grain.nextFloat() * 1508.0f;
        const float y = 12.0f + grain.nextFloat() * 1000.0f;
        g.drawLine(x, y, x + 2.0f + grain.nextFloat() * 20.0f, y + (grain.nextFloat() - 0.5f), 0.7f);
    }

    g.setColour(ink);
    g.fillRect(20, 98, 1496, 848);
    g.setColour(juce::Colour(0xff777268));
    g.drawRect(20, 98, 1496, 848, 2);

    // Top metal nameplate.
    g.setColour(juce::Colour(0xffbcb6a8));
    g.fillRect(22, 16, 1492, 76);
    g.setColour(juce::Colour(0xff514d46));
    g.drawRect(22, 16, 1492, 76, 2);
    g.setColour(juce::Colour(0xff090a09));
    g.setFont(juce::Font(55.0f, juce::Font::bold));
    g.drawText("Makosizer", 66, 15, 470, 68, juce::Justification::centredLeft);
    g.setFont(juce::Font(14.0f, juce::Font::bold));
    g.drawText("LO-FI MASTERING SUITE", 646, 28, 330, 22, juce::Justification::centredLeft);
    g.setFont(juce::Font(12.0f));
    g.drawText("VINYL  //  TAPE  //  NOISE  //  CRUSH  //  LIMIT", 646, 53, 460, 20, juce::Justification::centredLeft);
    g.setFont(juce::Font(13.0f, juce::Font::bold));
    g.drawText("GOOD SOUND.\nBAD DECISIONS.", 1220, 22, 210, 42, juce::Justification::centredRight);
    g.setFont(juce::Font(12.0f));
    g.drawText("v1.0.0", 1442, 52, 48, 20, juce::Justification::centredRight);

    drawScrew(g, 42.0f, 43.0f);
    drawScrew(g, 1494.0f, 43.0f);
    drawScrew(g, 42.0f, 924.0f);
    drawScrew(g, 1494.0f, 924.0f);

    // LCD bezel, phosphor face, scanlines and readout.
    drawPanel(g, { 266.0f, 112.0f, 1008.0f, 194.0f });
    g.setColour(juce::Colour(0xff050807));
    g.fillRoundedRectangle(278.0f, 123.0f, 984.0f, 170.0f, 8.0f);
    juce::ColourGradient phosphor(juce::Colour(0xff91ffac), 0.0f, 137.0f,
                                  juce::Colour(0xff24a856), 0.0f, 282.0f, false);
    g.setGradientFill(phosphor);
    g.fillRoundedRectangle(288.0f, 133.0f, 964.0f, 150.0f, 6.0f);
    g.setColour(juce::Colour(0x3500220f));
    for (int y = 138; y < 281; y += 4)
        g.drawHorizontalLine(y, 292.0f, 1248.0f);
    g.setColour(juce::Colour(0xff082615));
    g.setFont(juce::Font(40.0f, juce::Font::bold));
    g.drawText("Makosizer", 548, 145, 420, 50, juce::Justification::centred);
    g.setFont(juce::Font(20.0f, juce::Font::bold));
    g.drawText("LO-FI IS A FEATURE", 565, 196, 390, 27, juce::Justification::centred);
    g.setFont(juce::Font(14.0f, juce::Font::bold));
    g.drawText("VINYL SIM ACTIVE...  |||||||||||||||||||||||||", 528, 246, 440, 23, juce::Justification::centredLeft);
    g.setColour(juce::Colour(0xff0a2b18));
    g.drawRect(992, 145, 240, 112, 2);
    g.setFont(juce::Font(14.0f, juce::FontTypeface::bold));
    juce::String selectedPreset = "INIT";
    for (size_t i = 0; i < presetButtons.size(); ++i)
        if (presetButtons[i].getToggleState()) selectedPreset = presetNames[i];
    g.drawText("PRESET:  " + selectedPreset + "\nSR:      44.1 kHz\nBITS:    " + juce::String(static_cast<int>(crushKnob.getValue()))
               + "\nWOW:     " + juce::String(static_cast<int>(wowKnob.getValue() * 10.0)) + "%\nFLUTTER: "
               + juce::String(static_cast<int>(flutterKnob.getValue())) + "%\nNOISE:   -24 dB",
               1002, 149, 220, 105, juce::Justification::topLeft);

    // Left preset bay.
    drawPanel(g, { 34.0f, 112.0f, 214.0f, 493.0f });
    g.setColour(juce::Colour(0xffff4747));
    g.fillEllipse(49.0f, 129.0f, 14.0f, 14.0f);
    g.setColour(paper);
    g.setFont(juce::Font(16.0f, juce::Font::bold));
    g.drawText("PRESETS", 76, 125, 140, 23, juce::Justification::centredLeft);

    // Right vertical stereo meter bay and labels.
    drawPanel(g, { 1290.0f, 112.0f, 220.0f, 552.0f });
    g.setColour(paper);
    g.setFont(juce::Font(18.0f, juce::Font::bold));
    g.drawText("L", 1312, 121, 30, 25, juce::Justification::centred);
    g.drawText("R", 1455, 121, 30, 25, juce::Justification::centred);
    g.setFont(juce::Font(10.0f));
    g.drawText("PEAK", 1306, 151, 46, 18, juce::Justification::centred);
    g.drawText("PEAK", 1448, 151, 46, 18, juce::Justification::centred);
    g.setColour(juce::Colour(0xffff4545));
    g.fillEllipse(1321.0f, 170.0f, 16.0f, 16.0f);
    g.fillEllipse(1463.0f, 170.0f, 16.0f, 16.0f);
    g.setColour(paper);
    g.setFont(juce::Font(12.0f));
    const std::array<juce::String, 8> scale { "+3", "0", "-3", "-6", "-12", "-18", "-24", "-∞" };
    for (size_t i = 0; i < scale.size(); ++i)
        g.drawText(scale[i], 1373, 213 + static_cast<int>(i) * 47, 48, 18, juce::Justification::centred);

    // Lower processing bay.
    drawPanel(g, { 264.0f, 760.0f, 302.0f, 164.0f });
    drawPanel(g, { 578.0f, 760.0f, 302.0f, 164.0f });
    drawPanel(g, { 892.0f, 760.0f, 382.0f, 164.0f });
    g.setColour(paper);
    g.setFont(juce::Font(15.0f, juce::Font::bold));
    g.drawText("FILTER", 282, 769, 266, 21, juce::Justification::centredLeft);
    g.drawText("LIMITER", 596, 769, 266, 21, juce::Justification::centredLeft);
    g.drawText("STEREO", 910, 769, 346, 21, juce::Justification::centredLeft);
    g.setColour(juce::Colour(0xff514f49));
    g.drawHorizontalLine(795, 280.0f, 550.0f);
    g.drawHorizontalLine(795, 594.0f, 864.0f);
    g.drawHorizontalLine(795, 908.0f, 1258.0f);
    g.setColour(paper);
    g.setFont(juce::Font(12.0f));
    g.drawText("HPF", 305, 801, 80, 18, juce::Justification::centred);
    g.drawText("LPF", 438, 801, 80, 18, juce::Justification::centred);
    g.drawText("CEILING", 652, 801, 100, 18, juce::Justification::centred);
    g.drawText("WIDTH", 956, 801, 100, 18, juce::Justification::centred);
    g.drawText("MIX", 1110, 801, 100, 18, juce::Justification::centred);

    // Right-side oversampling selector and hand-painted slogan.
    drawPanel(g, { 1290.0f, 678.0f, 220.0f, 116.0f });
    g.setColour(paper);
    g.setFont(juce::Font(13.0f, juce::Font::bold));
    g.drawText("OVERSAMPLING", 1305, 687, 190, 20, juce::Justification::centredLeft);
    g.setFont(juce::Font(29.0f, juce::Font::italic));
    g.drawText("MAKE", 1298, 810, 190, 37, juce::Justification::centred);
    g.drawText("IT WORSE.", 1298, 841, 190, 37, juce::Justification::centred);

    // Bottom engraved legend.
    g.setColour(juce::Colour(0xffbdb6a8));
    g.fillRect(22, 938, 1492, 52);
    g.setColour(juce::Colour(0xff4a4741));
    g.drawLine(505.0f, 963.0f, 574.0f, 963.0f, 1.0f);
    g.drawLine(962.0f, 963.0f, 1031.0f, 963.0f, 1.0f);
    g.setColour(juce::Colour(0xff111312));
    g.setFont(juce::Font(13.0f, juce::Font::bold));
    g.drawText("A N A L O G   D A M A G E   U N I T", 580, 949, 380, 28, juce::Justification::centred);
}

void MakosizerAudioProcessorEditor::resized()
{
    for (size_t i = 0; i < presetButtons.size(); ++i)
        presetButtons[i].setBounds(48, 158 + static_cast<int>(i) * 42, 184, 35);
    loadButton.setBounds(49, 557, 89, 36);
    saveButton.setBounds(145, 557, 89, 36);

    driveKnob.setBounds(282, 326, 132, 150);
    vinylKnob.setBounds(455, 326, 132, 150);
    destroyKnob.setBounds(635, 311, 216, 222);
    wowKnob.setBounds(893, 326, 132, 150);
    flutterKnob.setBounds(1066, 326, 132, 150);
    noiseKnob.setBounds(282, 523, 132, 150);
    dustKnob.setBounds(455, 523, 132, 150);
    crushKnob.setBounds(893, 523, 132, 150);
    compKnob.setBounds(1066, 523, 132, 150);

    hpfKnob.setBounds(292, 817, 112, 104);
    lpfKnob.setBounds(426, 817, 112, 104);
    ceilingKnob.setBounds(674, 817, 112, 104);
    widthKnob.setBounds(956, 817, 112, 104);
    mixKnob.setBounds(1110, 817, 112, 104);

    meter.setBounds(1304, 192, 192, 448);
    sharkLogo.setBounds(300, 158, 150, 100);

    for (size_t i = 0; i < oversampleButtons.size(); ++i)
        oversampleButtons[i].setBounds(1304 + static_cast<int>(i) * 47, 720, 42, 42);
}

void MakosizerAudioProcessorEditor::timerCallback()
{
    meter.setLevels(processor.meterLeftDb.load(std::memory_order_relaxed),
                    processor.meterRightDb.load(std::memory_order_relaxed));
    repaint(266, 112, 1008, 194);
}
