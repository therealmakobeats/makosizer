#include "PluginEditor.h"

MakosizerAudioProcessorEditor::MakosizerAudioProcessorEditor(MakosizerAudioProcessor& p)
    : juce::AudioProcessorEditor(&p),
      processor(p)
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
    presetBrowser.addItem("INIT", 1);
    presetBrowser.addItem("SP303 Vibes", 2);
    presetBrowser.addItem("Dilla Dust", 3);
    presetBrowser.addItem("MPC Crunch", 4);
    presetBrowser.addItem("Cassette Melt", 5);
    presetBrowser.addItem("Lofi Radio", 6);
    presetBrowser.addItem("Underground", 7);
    presetBrowser.addItem("Dirty Master", 8);
    presetBrowser.setSelectedId(1);
    addAndMakeVisible(presetBrowser);

    loadButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff2f3b36));
    saveButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff2f3b36));
    factoryButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff2f3b36));
    addAndMakeVisible(loadButton);
    addAndMakeVisible(saveButton);
    addAndMakeVisible(factoryButton);

    destroyKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    destroyKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 18);
    destroyKnob.setRange(0.0, 100.0, 1.0);
    destroyKnob.setValue(38.0);
    destroyKnob.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff7ff7a9));
    addAndMakeVisible(destroyKnob);

    driveKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    driveKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 18);
    driveKnob.setRange(0.0, 1.0, 0.001);
    addAndMakeVisible(driveKnob);

    vinylKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    vinylKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 18);
    vinylKnob.setRange(0.0, 1.0, 0.001);
    addAndMakeVisible(vinylKnob);

    wowKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    wowKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 18);
    wowKnob.setRange(0.1, 2.0, 0.01);
    addAndMakeVisible(wowKnob);

    flutterKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    flutterKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 18);
    flutterKnob.setRange(4.0, 20.0, 0.01);
    addAndMakeVisible(flutterKnob);

    noiseKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    noiseKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 18);
    noiseKnob.setRange(0.0, 1.0, 0.001);
    addAndMakeVisible(noiseKnob);

    dustKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    dustKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 18);
    dustKnob.setRange(0.0, 1.0, 0.001);
    addAndMakeVisible(dustKnob);

    crushKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    crushKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 18);
    crushKnob.setRange(4.0, 16.0, 0.01);
    addAndMakeVisible(crushKnob);

    compKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    compKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 18);
    compKnob.setRange(0.0, 1.0, 0.001);
    addAndMakeVisible(compKnob);

    widthKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    widthKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 18);
    widthKnob.setRange(0.0, 2.0, 0.001);
    addAndMakeVisible(widthKnob);

    mixKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    mixKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 18);
    mixKnob.setRange(0.0, 1.0, 0.001);
    addAndMakeVisible(mixKnob);

    destroyAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "Destroy", destroyKnob);
    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "Drive", driveKnob);
    vinylAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "Vinyl", vinylKnob);
    wowAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "Wow", wowKnob);
    flutterAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "Flutter", flutterKnob);
    noiseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "Noise", noiseKnob);
    dustAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "Dust", dustKnob);
    crushAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "Bits", crushKnob);
    compAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "Comp", compKnob);
    widthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "Width", widthKnob);
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "Mix", mixKnob);

    addAndMakeVisible(sharkLogo);
    addAndMakeVisible(meter);

    startTimerHz(10);
}

MakosizerAudioProcessorEditor::~MakosizerAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void MakosizerAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff101512));

    g.setColour(juce::Colour(0xff27352d));
    g.fillRoundedRectangle(juce::Rectangle<float>(18.0f, 18.0f, getWidth() - 36.0f, getHeight() - 36.0f), 18.0f);

    g.setColour(juce::Colour(0xff111915));
    g.fillRoundedRectangle(juce::Rectangle<float>(30.0f, 76.0f, getWidth() - 60.0f, getHeight() - 130.0f), 16.0f);

    g.setColour(juce::Colour(0xffa3d5a2));
    g.drawText("Makosizer", juce::Rectangle<float>(40.0f, 25.0f, 220.0f, 32.0f), juce::Justification::centredLeft, false);

    g.setColour(juce::Colour(0xff7ec39a));
    g.drawText("VINYL SIM ACTIVE", juce::Rectangle<float>(760.0f, 30.0f, 220.0f, 24.0f), juce::Justification::centredRight, false);
}

void MakosizerAudioProcessorEditor::resized()
{
    const auto area = getLocalBounds().reduced(38);

    titleLabel.setBounds(40, 28, 260, 30);
    subtitleLabel.setBounds(40, 54, 260, 22);
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
