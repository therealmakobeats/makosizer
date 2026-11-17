#include "PluginEditor.h"

MakosizerAudioProcessorEditor::MakosizerAudioProcessorEditor(MakosizerAudioProcessor& p)
    : juce::AudioProcessorEditor(&p), processor(p)
{
    setLookAndFeel(&lookAndFeel);
    setSize(1120, 800);

    titleLabel.setText("Makosizer", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(28.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(titleLabel);
    subtitleLabel.setText("LO-FI MASTERING SUITE", juce::dontSendNotification);
    subtitleLabel.setFont(juce::Font(12.0f, juce::Font::plain));
    subtitleLabel.setColour(juce::Label::textColourId, juce::Colour(0xff86dfb4));
    addAndMakeVisible(subtitleLabel);
    presetLabel.setText("INIT", juce::dontSendNotification);
    presetLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    presetLabel.setJustificationType(juce::Justification::centredLeft);
    presetLabel.setColour(juce::Label::textColourId, juce::Colour(0xffb2ffb0));
    addAndMakeVisible(presetLabel);

    presetBrowser.setTextWhenNothingSelected("Preset");
    const juce::StringArray presets { "INIT", "SP303 Vibes", "Dilla Dust", "MPC Crunch", "Cassette Melt", "Lofi Radio", "Underground", "Dirty Master" };
    for (int i = 0; i < presets.size(); ++i)
        presetBrowser.addItem(presets[i], i + 1);
    presetBrowser.setSelectedId(1);
    addAndMakeVisible(presetBrowser);

    for (auto* button : { &loadButton, &saveButton, &factoryButton })
    {
        button->setColour(juce::TextButton::buttonColourId, juce::Colour(0xff2f3b36));
        addAndMakeVisible(button);
    }

    const auto configureKnob = [](juce::Slider& slider, double minimum, double maximum, double interval)
    {
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 18);
        slider.setRange(minimum, maximum, interval);
    };
    configureKnob(destroyKnob, 0.0, 1.0, 0.001);
    configureKnob(driveKnob, 0.0, 1.0, 0.001);
    configureKnob(vinylKnob, 0.0, 1.0, 0.001);
    configureKnob(wowKnob, 0.1, 2.0, 0.01);
    configureKnob(flutterKnob, 4.0, 20.0, 0.01);
    configureKnob(noiseKnob, 0.0, 1.0, 0.001);
    configureKnob(dustKnob, 0.0, 1.0, 0.001);
    configureKnob(crushKnob, 4.0, 16.0, 0.01);
    configureKnob(compKnob, 0.0, 1.0, 0.001);
    configureKnob(widthKnob, 0.0, 2.0, 0.001);
    configureKnob(mixKnob, 0.0, 1.0, 0.001);
    destroyKnob.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff7ff7a9));
    for (auto* slider : { &destroyKnob, &driveKnob, &vinylKnob, &wowKnob, &flutterKnob,
                          &noiseKnob, &dustKnob, &crushKnob, &compKnob, &widthKnob, &mixKnob })
        addAndMakeVisible(slider);

    auto& state = processor.apvts;
    destroyAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "Destroy", destroyKnob);
    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "Drive", driveKnob);
    vinylAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "Vinyl", vinylKnob);
    wowAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "Wow", wowKnob);
    flutterAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "Flutter", flutterKnob);
    noiseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "Noise", noiseKnob);
    dustAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "Dust", dustKnob);
    crushAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "Bits", crushKnob);
    compAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "Comp", compKnob);
    widthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "Width", widthKnob);
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "Mix", mixKnob);

    addAndMakeVisible(sharkLogo);
    addAndMakeVisible(meter);
    startTimerHz(10);
}

MakosizerAudioProcessorEditor::~MakosizerAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

void MakosizerAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff101512));
    g.setColour(juce::Colour(0xff27352d));
    g.fillRoundedRectangle(18.0f, 18.0f, static_cast<float>(getWidth() - 36), static_cast<float>(getHeight() - 36), 18.0f);
    g.setColour(juce::Colour(0xff111915));
    g.fillRoundedRectangle(30.0f, 76.0f, static_cast<float>(getWidth() - 60), static_cast<float>(getHeight() - 130), 16.0f);
    g.setColour(juce::Colour(0xff7ec39a));
    g.drawText("VINYL SIM ACTIVE", 760, 30, 220, 24, juce::Justification::centredRight);
}

void MakosizerAudioProcessorEditor::resized()
{
    titleLabel.setBounds(88, 25, 260, 30);
    subtitleLabel.setBounds(88, 52, 260, 22);
    presetLabel.setBounds(760, 54, 220, 24);
    sharkLogo.setBounds(20, 20, 60, 60);
    presetBrowser.setBounds(30, 110, 250, 30);
    loadButton.setBounds(30, 155, 75, 30);
    saveButton.setBounds(110, 155, 75, 30);
    factoryButton.setBounds(190, 155, 90, 30);
    destroyKnob.setBounds(420, 160, 180, 180);
    driveKnob.setBounds(160, 320, 130, 130);
    vinylKnob.setBounds(295, 320, 130, 130);
    wowKnob.setBounds(430, 320, 130, 130);
    flutterKnob.setBounds(565, 320, 130, 130);
    noiseKnob.setBounds(700, 320, 130, 130);
    dustKnob.setBounds(835, 320, 130, 130);
    crushKnob.setBounds(160, 520, 130, 130);
    compKnob.setBounds(295, 520, 130, 130);
    widthKnob.setBounds(430, 520, 130, 130);
    mixKnob.setBounds(565, 520, 130, 130);
    meter.setBounds(760, 120, 220, 130);
}

void MakosizerAudioProcessorEditor::timerCallback()
{
    meter.repaint();
}
