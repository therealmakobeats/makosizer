#include "LookAndFeel.h"

#include <cmath>

MakosizerLookAndFeel::MakosizerLookAndFeel()
{
    setColour(juce::Slider::thumbColourId, juce::Colour(0xffff514c));
    setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffed403a));
    setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff817d73));
    setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff191b1a));
    setColour(juce::ComboBox::textColourId, juce::Colour(0xffd0cbbb));
}

void MakosizerLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                            float sliderPos, float startAngle, float endAngle, juce::Slider& slider)
{
    juce::ignoreUnused(slider);
    const auto bounds = juce::Rectangle<float>(static_cast<float>(x), static_cast<float>(y),
                                                static_cast<float>(width), static_cast<float>(height));
    const float size = juce::jmin(bounds.getWidth(), bounds.getHeight() - 20.0f);
    const float radius = juce::jmax(12.0f, size * 0.37f);
    const auto centre = bounds.getCentre().withY(bounds.getY() + size * 0.48f);
    const float angle = startAngle + sliderPos * (endAngle - startAngle);

    g.setColour(juce::Colour(0xff050606));
    g.fillEllipse(centre.x - radius - 5.0f, centre.y - radius - 2.0f,
                  (radius + 5.0f) * 2.0f, (radius + 5.0f) * 2.0f);
    g.setColour(juce::Colour(0xff7c7a72));
    g.drawEllipse(centre.x - radius - 3.0f, centre.y - radius - 3.0f,
                  (radius + 3.0f) * 2.0f, (radius + 3.0f) * 2.0f, 1.4f);
    g.setColour(juce::Colour(0xff262827));
    g.fillEllipse(centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f);
    juce::ColourGradient face(juce::Colour(0xff464743), centre.x - radius, centre.y - radius,
                              juce::Colour(0xff111313), centre.x + radius, centre.y + radius, false);
    g.setGradientFill(face);
    g.fillEllipse(centre.x - radius + 4.0f, centre.y - radius + 4.0f,
                  (radius - 4.0f) * 2.0f, (radius - 4.0f) * 2.0f);
    g.setColour(juce::Colour(0xffaaa69a));
    g.drawEllipse(centre.x - radius + 5.0f, centre.y - radius + 5.0f,
                  (radius - 5.0f) * 2.0f, (radius - 5.0f) * 2.0f, 0.8f);

    for (int tick = 0; tick <= 20; ++tick)
    {
        const float a = startAngle + (endAngle - startAngle) * static_cast<float>(tick) / 20.0f;
        const float inner = radius + 8.0f;
        const float outer = radius + (tick % 5 == 0 ? 17.0f : 12.0f);
        g.setColour(tick % 5 == 0 ? juce::Colour(0xffd0cbbb) : juce::Colour(0xff77756d));
        g.drawLine(centre.x + std::cos(a) * inner, centre.y + std::sin(a) * inner,
                   centre.x + std::cos(a) * outer, centre.y + std::sin(a) * outer,
                   tick % 5 == 0 ? 1.5f : 0.8f);
    }

    juce::Path valueArc;
    valueArc.addCentredArc(centre.x, centre.y, radius + 10.0f, radius + 10.0f, 0.0f,
                           startAngle, angle, true);
    g.setColour(juce::Colour(0xffee403a));
    g.strokePath(valueArc, juce::PathStrokeType(radius > 55.0f ? 3.5f : 2.2f,
                                                juce::PathStrokeType::curved,
                                                juce::PathStrokeType::rounded));

    juce::Path pointer;
    pointer.addRoundedRectangle(-1.6f, -radius + 9.0f, 3.2f, radius * 0.58f, 1.5f);
    g.setColour(juce::Colour(0xffa51e1b));
    g.fillPath(pointer, juce::AffineTransform::rotation(angle).translated(centre.x + 1.0f, centre.y + 1.0f));
    g.setColour(juce::Colour(0xffff625c));
    g.fillPath(pointer, juce::AffineTransform::rotation(angle).translated(centre.x, centre.y));
    g.setColour(juce::Colour(0xffc7c2b5));
    g.fillEllipse(centre.x - 2.3f, centre.y - 2.3f, 4.6f, 4.6f);
}

void MakosizerLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                                        int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& box)
{
    juce::ignoreUnused(isButtonDown, buttonX, buttonY, buttonW, buttonH, box);
    g.setColour(juce::Colour(0xff191b1a));
    g.fillRoundedRectangle(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 4.0f);
    g.setColour(juce::Colour(0xff6d685e));
    g.drawRoundedRectangle(0.5f, 0.5f, static_cast<float>(width) - 1.0f,
                           static_cast<float>(height) - 1.0f, 4.0f, 1.0f);
}

void MakosizerLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                                const juce::Colour& background, bool isMouseOver, bool isButtonDown)
{
    auto bounds = button.getLocalBounds().toFloat();
    auto colour = background;
    if (button.getToggleState())
        colour = juce::Colour(0xff431d1b);
    if (isButtonDown)
        colour = colour.brighter(0.15f);
    else if (isMouseOver)
        colour = colour.brighter(0.08f);

    g.setColour(juce::Colour(0xff050606));
    g.fillRoundedRectangle(bounds.translated(0.0f, 1.5f), 3.0f);
    g.setColour(colour);
    g.fillRoundedRectangle(bounds.reduced(0.5f), 3.0f);
    g.setColour(button.getToggleState() ? juce::Colour(0xfff34b43) : juce::Colour(0xff5d5a53));
    g.drawRoundedRectangle(bounds.reduced(0.5f), 3.0f, 1.0f);
}

void MakosizerLookAndFeel::drawTextEditorOutline(juce::Graphics& g, int width, int height, juce::TextEditor& editor)
{
    g.setColour(editor.hasKeyboardFocus(true) ? juce::Colour(0xfff34b43) : juce::Colour(0xff625f57));
    g.drawRoundedRectangle(0.5f, 0.5f, static_cast<float>(width) - 1.0f,
                           static_cast<float>(height) - 1.0f, 3.0f, 1.0f);
}
