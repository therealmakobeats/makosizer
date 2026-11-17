#include "PluginEditor.h"

#include <array>
#include <cmath>

namespace
{
const juce::Colour black { 0xff090b0a };
const juce::Colour metal { 0xffaaa397 };
const juce::Colour text { 0xffdedbd2 };
const juce::Colour red { 0xffff3935 };
const std::array<juce::String, 9> presetNames {
    "INIT", "SP303 VIBES", "DILLA DUST", "MPC CRUNCH", "CASSETTE MELT",
    "LOFI RADIO", "UNDERGROUND", "DIRTY MASTER", "CUSTOM"
};
const std::array<const char*, 14> parameterIds {
    "Drive", "VinylSim", "Destroy", "Wow", "Flutter", "Noise", "Dust",
    "Crush", "Comp", "HPF", "LPF", "Ceiling", "Width", "Mix"
};
const std::array<juce::String, 14> parameterNames {
    "DRIVE", "VINYL SIM", "DESTROY", "WOW", "FLUTTER", "NOISE", "DUST",
    "CRUSH", "COMP", "HPF", "LPF", "CEILING", "WIDTH", "MIX"
};
struct Preset { std::array<float, 14> values; int oversampling; };
const std::array<Preset, 9> presets {{
    {{{0,0,0,0,0,0,0,0,0,20,20000,-0.1f,100,100}}, 1},
    {{{18,78,85,20,28,12,36,70,80,80,12000,-0.1f,120,100}}, 1},
    {{{14,92,72,35,18,18,65,40,88,60,13000,-0.2f,110,100}}, 1},
    {{{20,45,80,8,12,6,15,85,75,40,15000,-0.1f,100,100}}, 2},
    {{{12,60,65,68,75,20,20,30,65,90,10000,-0.3f,135,100}}, 1},
    {{{8,25,55,15,40,25,45,60,90,350,4200,-0.5f,30,100}}, 0},
    {{{20,70,65,12,22,10,30,55,65,70,11000,-0.1f,115,100}}, 1},
    {{{12,40,50,10,14,8,18,35,70,30,16000,-0.1f,125,85}}, 2},
    {{{18,78,85,20,28,12,36,70,80,80,12000,-0.1f,120,100}}, 1}
}};

void panel(juce::Graphics& g, juce::Rectangle<float> r)
{
    g.setColour(juce::Colour(0xff050606));
    g.fillRoundedRectangle(r.translated(0, 3), 9.0f);
    g.setColour(juce::Colour(0xff171918));
    g.fillRoundedRectangle(r, 9.0f);
    g.setColour(juce::Colour(0xff59574f));
    g.drawRoundedRectangle(r, 9.0f, 1.0f);
}
}

MakosizerAudioProcessorEditor::MakosizerAudioProcessorEditor(MakosizerAudioProcessor& p)
    : juce::AudioProcessorEditor(&p), processor(p)
{
    setSize(1536, 1024);
    setResizable(true, true);
    setResizeLimits(1000, 667, 1920, 1280);

    for (size_t i = 0; i < sliders.size(); ++i)
    {
        auto& slider = sliders[i];
        setSliderStyle(slider, i == 2);
        addAndMakeVisible(slider);
        attachments[i] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            processor.apvts, parameterIds[i], slider);
        slider.onValueChange = [this, i, &slider]
        {
            activeText = parameterNames[i] + ": " + slider.getTextFromValue(slider.getValue());
            activeTextTicks = 50;
            repaint(260, 110, 1020, 210);
        };
    }

    for (size_t i = 0; i < presetButtons.size(); ++i)
    {
        auto& button = presetButtons[i];
        button.setButtonText(presetNames[i]);
        button.setClickingTogglesState(true);
        button.setRadioGroupId(101, juce::dontSendNotification);
        button.setToggleState(i == 0, juce::dontSendNotification);
        button.onClick = [this, i] { applyPreset(static_cast<int>(i)); };
        addAndMakeVisible(button);
    }
    for (size_t i = 0; i < oversamplingButtons.size(); ++i)
    {
        auto& button = oversamplingButtons[i];
        button.setButtonText(juce::String(1 << i) + "x");
        button.setClickingTogglesState(true);
        button.setRadioGroupId(102, juce::dontSendNotification);
        button.setToggleState(i == 1, juce::dontSendNotification);
        button.onClick = [this, i]
        {
            if (auto* parameter = processor.apvts.getParameter("Oversampling"))
                parameter->setValueNotifyingHost(parameter->convertTo0to1(static_cast<float>(i)));
        };
        addAndMakeVisible(button);
    }
    for (auto* button : { &loadButton, &saveButton })
    {
        button->onClick = [this]
        {
            activeText = "PRESET STATE STORED IN HOST";
            activeTextTicks = 50;
            repaint();
        };
        addAndMakeVisible(*button);
    }
    startTimerHz(15);
}

MakosizerAudioProcessorEditor::~MakosizerAudioProcessorEditor()
{
    stopTimer();
}

void MakosizerAudioProcessorEditor::setSliderStyle(juce::Slider& slider, bool large)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, large ? 92 : 72, 22);
    slider.setColour(juce::Slider::rotarySliderFillColourId, red);
    slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff494b48));
    slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xffff7770));
    slider.setColour(juce::Slider::textBoxTextColourId, text);
    slider.setColour(juce::Slider::textBoxBackgroundColourId, black);
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff45443f));
    slider.setColour(juce::Slider::textBoxHighlightColourId, juce::Colour(0xff88201e));
}

void MakosizerAudioProcessorEditor::setParameter(const char* id, float value)
{
    if (auto* parameter = processor.apvts.getParameter(id))
        parameter->setValueNotifyingHost(parameter->convertTo0to1(value));
}

void MakosizerAudioProcessorEditor::applyPreset(int index)
{
    currentPreset = juce::jlimit(0, static_cast<int>(presets.size()) - 1, index);
    presetButtons[static_cast<size_t>(currentPreset)].setToggleState(true, juce::dontSendNotification);
    for (size_t i = 0; i < parameterIds.size(); ++i)
        setParameter(parameterIds[i], presets[static_cast<size_t>(currentPreset)].values[i]);
    setParameter("Oversampling", static_cast<float>(presets[static_cast<size_t>(currentPreset)].oversampling));
    for (size_t i = 0; i < oversamplingButtons.size(); ++i)
        oversamplingButtons[i].setToggleState(static_cast<int>(i) == presets[static_cast<size_t>(currentPreset)].oversampling,
                                               juce::dontSendNotification);
    activeText = "PRESET LOADED: " + presetNames[static_cast<size_t>(currentPreset)];
    activeTextTicks = 50;
    repaint();
}

void MakosizerAudioProcessorEditor::drawMeter(juce::Graphics& g, int x, float db)
{
    const std::array<float, 10> thresholds { 3.0f, 0.0f, -3.0f, -6.0f, -9.0f, -12.0f, -18.0f, -24.0f, -36.0f, -48.0f };
    for (size_t i = 0; i < thresholds.size(); ++i)
    {
        const int y = 228 + static_cast<int>(i) * 36;
        const bool active = db >= thresholds[i];
        const juce::Colour c = i < 2 ? juce::Colour(0xffff3e37)
                                : i < 4 ? juce::Colour(0xffffb629)
                                        : juce::Colour(0xff34f05a);
        g.setColour(active ? c : juce::Colour(0xff202421));
        g.fillRoundedRectangle(static_cast<float>(x), static_cast<float>(y), 22.0f, 19.0f, 2.0f);
        if (active)
        {
            g.setColour(c.withAlpha(0.45f));
            g.drawRoundedRectangle(static_cast<float>(x - 2), static_cast<float>(y - 2), 26.0f, 23.0f, 3.0f, 3.0f);
        }
    }
}

void MakosizerAudioProcessorEditor::paint(juce::Graphics& g)
{
    const float sx = static_cast<float>(getWidth()) / 1536.0f;
    const float sy = static_cast<float>(getHeight()) / 1024.0f;
    g.saveState();
    g.addTransform(juce::AffineTransform::scale(sx, sy));
    g.fillAll(black);
    juce::ColourGradient frame(juce::Colour(0xffc2bbac), 0, 0, juce::Colour(0xff4b4741), 0, 1024, false);
    g.setGradientFill(frame);
    g.fillRoundedRectangle(8, 8, 1520, 1008, 12);
    g.setColour(juce::Colour(0xff070808));
    g.drawRoundedRectangle(8, 8, 1520, 1008, 12, 5);
    g.setColour(juce::Colour(0x25000000));
    juce::Random grain(0x4d414b4f);
    for (int i = 0; i < 850; ++i)
    {
        const float x = 14.0f + grain.nextFloat() * 1508.0f;
        const float y = 13.0f + grain.nextFloat() * 997.0f;
        g.drawLine(x, y, x + 3.0f + grain.nextFloat() * 17.0f, y + grain.nextFloat() - 0.5f, 0.7f);
    }
    g.setColour(black);
    g.fillRect(20, 98, 1496, 848);
    g.setColour(juce::Colour(0xff777268));
    g.drawRect(20, 98, 1496, 848, 2);

    g.setColour(metal);
    g.fillRect(22, 16, 1492, 76);
    g.setColour(juce::Colour(0xff514d46));
    g.drawRect(22, 16, 1492, 76, 2);
    g.setColour(black);
    g.setFont(juce::Font(55, juce::Font::bold));
    g.drawText("Makosizer", 66, 15, 470, 68, juce::Justification::centredLeft);
    g.setFont(juce::Font(14, juce::Font::bold));
    g.drawText("LO-FI MASTERING SUITE", 646, 28, 330, 22, juce::Justification::centredLeft);
    g.setFont(juce::Font(12));
    g.drawText("VINYL  //  TAPE  //  NOISE  //  CRUSH  //  LIMIT", 646, 53, 460, 20, juce::Justification::centredLeft);
    g.setFont(juce::Font(13, juce::Font::bold));
    g.drawText("GOOD SOUND.\nBAD DECISIONS.", 1220, 22, 210, 42, juce::Justification::centredRight);
    g.setFont(juce::Font(12));
    g.drawText("v2.0.0", 1442, 52, 48, 20, juce::Justification::centredRight);
    for (auto point : { juce::Point<float>(42,43), {1494,43}, {42,924}, {1494,924} })
    {
        g.setColour(juce::Colour(0xff050606)); g.fillEllipse(point.x - 13, point.y - 13, 26, 26);
        g.setColour(metal); g.drawEllipse(point.x - 10, point.y - 10, 20, 20, 1.2f);
        g.setColour(juce::Colour(0xff292a27)); g.drawLine(point.x - 5, point.y + 5, point.x + 5, point.y - 5, 2);
    }

    panel(g, { 266,112,1008,194 });
    g.setColour(juce::Colour(0xff050807)); g.fillRoundedRectangle(278,123,984,170,8);
    juce::ColourGradient lcd(juce::Colour(0xff91ffac),0,137,juce::Colour(0xff24a856),0,282,false);
    g.setGradientFill(lcd); g.fillRoundedRectangle(288,133,964,150,6);
    g.setColour(juce::Colour(0x3500220f));
    for (int y=138; y<281; y+=4) g.drawHorizontalLine(y,292,1248);
    g.setColour(juce::Colour(0xff082615));
    g.setFont(juce::Font(40,juce::Font::bold)); g.drawText("Makosizer",548,145,420,50,juce::Justification::centred);
    g.setFont(juce::Font(20,juce::Font::bold)); g.drawText("LO-FI IS A FEATURE",565,196,390,27,juce::Justification::centred);
    g.setFont(juce::Font(14,juce::Font::bold));
    const float vinylValue = processor.apvts.getRawParameterValue("VinylSim")->load();
    g.drawText(activeTextTicks > 0 ? activeText : (vinylValue > 1.0f ? "VINYL SIM ACTIVE...  |||||||||||||||||||||" : "MASTERING READY"),
               528,246,440,23,juce::Justification::centredLeft);
    g.setColour(juce::Colour(0xff0a2b18)); g.drawRect(992,145,240,112,2);
    g.setFont(juce::Font(14,juce::Font::bold));
    const auto val = [this](const char* id) { return processor.apvts.getRawParameterValue(id)->load(); };
    const float crush = val("Crush");
    const float sampleRateReadout = 44.1f / (1.0f + crush * 0.095f);
    const int bits = juce::jmax(4, 16 - static_cast<int>(std::round(crush * 0.105f)));
    const juce::String readout = "PRESET:  " + presetNames[static_cast<size_t>(currentPreset)]
        + "\nSR:      " + juce::String(sampleRateReadout, 1) + " kHz"
        + "\nBITS:    " + juce::String(bits)
        + "\nWOW:     " + juce::String(static_cast<int>(val("Wow"))) + "%"
        + "\nFLUTTER: " + juce::String(static_cast<int>(val("Flutter"))) + "%"
        + "\nNOISE:   -" + juce::String(static_cast<int>(30.0f - val("Noise"))) + " dB";
    g.drawText(readout,1002,149,220,105,juce::Justification::topLeft);

    panel(g,{34,112,214,493});
    g.setColour(red); g.fillEllipse(49,129,14,14);
    g.setColour(text); g.setFont(juce::Font(16,juce::Font::bold)); g.drawText("PRESETS",76,125,140,23,juce::Justification::centredLeft);
    panel(g,{1290,112,220,552});
    g.setColour(text); g.setFont(juce::Font(18,juce::Font::bold));
    g.drawText("L",1312,121,30,25,juce::Justification::centred); g.drawText("R",1455,121,30,25,juce::Justification::centred);
    g.setFont(juce::Font(10)); g.drawText("PEAK",1306,151,46,18,juce::Justification::centred); g.drawText("PEAK",1448,151,46,18,juce::Justification::centred);
    const float peakL = processor.meters[3].load(std::memory_order_relaxed);
    const float peakR = processor.meters[4].load(std::memory_order_relaxed);
    g.setColour(peakL >= -0.1f ? red : juce::Colour(0xff5d1817)); g.fillEllipse(1321,170,16,16);
    g.setColour(peakR >= -0.1f ? red : juce::Colour(0xff5d1817)); g.fillEllipse(1463,170,16,16);
    g.setColour(text); g.setFont(juce::Font(12));
    const std::array<juce::String,8> scale { "+3","0","-3","-6","-12","-18","-24","-∞" };
    for (size_t i=0;i<scale.size();++i) g.drawText(scale[i],1373,213+static_cast<int>(i)*47,48,18,juce::Justification::centred);
    drawMeter(g,1330,peakL); drawMeter(g,1472,peakR);

    panel(g,{264,760,302,164}); panel(g,{578,760,302,164}); panel(g,{892,760,382,164});
    g.setColour(text); g.setFont(juce::Font(15,juce::Font::bold));
    g.drawText("FILTER",282,769,266,21,juce::Justification::centredLeft);
    g.drawText("LIMITER",596,769,266,21,juce::Justification::centredLeft);
    g.drawText("STEREO",910,769,346,21,juce::Justification::centredLeft);
    g.setColour(juce::Colour(0xff514f49)); g.drawHorizontalLine(795,280,550); g.drawHorizontalLine(795,594,864); g.drawHorizontalLine(795,908,1258);
    g.setColour(text); g.setFont(juce::Font(12));
    for (const auto& label : std::array<std::pair<juce::String,juce::Rectangle<int>>,5>{{
        {"HPF",{305,801,80,18}},{"LPF",{438,801,80,18}},{"CEILING",{652,801,100,18}},
        {"WIDTH",{956,801,100,18}},{"MIX",{1110,801,100,18}} }})
        g.drawText(label.first,label.second,juce::Justification::centred);
    panel(g,{1290,678,220,116}); g.setColour(text); g.setFont(juce::Font(13,juce::Font::bold));
    g.drawText("OVERSAMPLING",1305,687,190,20,juce::Justification::centredLeft);
    g.setFont(juce::Font(29,juce::Font::italic)); g.drawText("MAKE",1298,810,190,37,juce::Justification::centred); g.drawText("IT WORSE.",1298,841,190,37,juce::Justification::centred);
    g.setColour(metal); g.fillRect(22,938,1492,52); g.setColour(juce::Colour(0xff4a4741));
    g.drawLine(505,963,574,963,1); g.drawLine(962,963,1031,963,1); g.setColour(black);
    g.setFont(juce::Font(13,juce::Font::bold)); g.drawText("A N A L O G   D A M A G E   U N I T",580,949,380,28,juce::Justification::centred);
    g.restoreState();
}

void MakosizerAudioProcessorEditor::resized()
{
    const auto place = [this](size_t i, int x, int y, int w, int h) { sliders[i].setBounds(x,y,w,h); };
    for (size_t i=0;i<presetButtons.size();++i) presetButtons[i].setBounds(48,158+static_cast<int>(i)*42,184,35);
    loadButton.setBounds(49,557,89,36); saveButton.setBounds(145,557,89,36);
    place(0,282,326,132,150); place(1,455,326,132,150); place(2,635,311,216,222);
    place(3,893,326,132,150); place(4,1066,326,132,150);
    place(5,282,523,132,150); place(6,455,523,132,150); place(7,893,523,132,150); place(8,1066,523,132,150);
    place(9,292,817,112,104); place(10,426,817,112,104); place(11,674,817,112,104);
    place(12,956,817,112,104); place(13,1110,817,112,104);
    for (size_t i=0;i<oversamplingButtons.size();++i) oversamplingButtons[i].setBounds(1304+static_cast<int>(i)*47,720,42,42);
}

void MakosizerAudioProcessorEditor::timerCallback()
{
    if (activeTextTicks > 0) --activeTextTicks;
    repaint();
}
