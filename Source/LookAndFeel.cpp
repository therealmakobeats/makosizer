#include "LookAndFeel.h"

MakosizerLookAndFeel::MakosizerLookAndFeel()
{
    setColour(juce::Slider::thumbColourId, juce::Colour(0xffd9ffe3));
    setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff78e3a1));
    setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff34423a));
    setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff18211c));
    setColour(juce::ComboBox::textColourId, juce::Colour(0xffc7e4cc));
}

void MakosizerLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                            float sliderPos, float startAngle, float endAngle, juce::Slider& slider)
{
    juce::ignoreUnused(slider);
    const auto bounds = juce::Rectangle<float>(static_cast<float>(x), static_cast<float>(y),
                                                static_cast<float>(width), static_cast<float>(height)).reduced(8.0f);
    const auto diameter = juce::jmin(bounds.getWidth(), bounds.getHeight()) - 18.0f;
    const auto radius = diameter * 0.5f;
    const auto centre = bounds.getCentre();
    const auto angle = startAngle + sliderPos * (endAngle - startAngle);
    g.setColour(juce::Colour(0xff151c18));
    g.fillEllipse(centre.x - radius, centre.y - radius, diameter, diameter);
    g.setColour(juce::Colour(0xff43564a));
    g.drawEllipse(centre.x - radius, centre.y - radius, diameter, diameter, 2.0f);
    juce::Path arc;
    arc.addCentredArc(centre.x, centre.y, radius + 5.0f, radius + 5.0f, 0.0f, startAngle, angle, true);
    g.setColour(juce::Colour(0xff78e3a1));
    g.strokePath(arc, juce::PathStrokeType(3.0f));
    juce::Path pointer;
    pointer.addRoundedRectangle(-2.0f, -radius + 8.0f, 4.0f, radius * 0.55f, 2.0f);
    g.setColour(juce::Colour(0xffd9ffe3));
    g.fillPath(pointer, juce::AffineTransform::rotation(angle).translated(centre.x, centre.y));
}

void MakosizerLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                                        int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& box)
{
    juce::ignoreUnused(isButtonDown, buttonX, buttonY, buttonW, buttonH, box);
    g.setColour(juce::Colour(0xff18211c));
    g.fillRoundedRectangle(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 5.0f);
    g.setColour(juce::Colour(0xff53695b));
    g.drawRoundedRectangle(0.5f, 0.5f, static_cast<float>(width) - 1.0f, static_cast<float>(height) - 1.0f, 5.0f, 1.0f);
}

void MakosizerLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                                const juce::Colour& background, bool isMouseOver, bool isButtonDown)
{
    auto colour = background;
    if (isButtonDown)
        colour = colour.brighter(0.15f);
    else if (isMouseOver)
        colour = colour.brighter(0.08f);
    g.setColour(colour);
    g.fillRoundedRectangle(button.getLocalBounds().toFloat(), 4.0f);
}

void MakosizerLookAndFeel::drawTextEditorOutline(juce::Graphics& g, int width, int height, juce::TextEditor& editor)
{
    g.setColour(editor.hasKeyboardFocus(true) ? juce::Colour(0xff78e3a1) : juce::Colour(0xff43564a));
    g.drawRoundedRectangle(0.5f, 0.5f, static_cast<float>(width) - 1.0f, static_cast<float>(height) - 1.0f, 3.0f, 1.0f);
}
