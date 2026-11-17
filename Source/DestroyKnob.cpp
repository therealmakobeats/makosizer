#include "DestroyKnob.h"

#include <cmath>

DestroyKnob::DestroyKnob()
{
    setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 18);
    setRange(0.0, 100.0, 1.0);
    setValue(38.0);
}

void DestroyKnob::paint(juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat();
    const auto centre = bounds.getCentre();
    const auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight() - 20.0f) * 0.38f;
    const auto angle = juce::jmap(static_cast<float>(getValue()), 0.0f, 100.0f,
                                  juce::MathConstants<float>::pi * 0.75f,
                                  juce::MathConstants<float>::pi * 2.25f);
    g.setColour(juce::Colour(0xff1e2623));
    g.fillEllipse(centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f);
    g.setColour(juce::Colour(0xff80f7aa));
    g.drawEllipse(centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f, 2.0f);
    const juce::Point<float> start(centre.x + std::cos(angle) * radius * 0.3f,
                                   centre.y + std::sin(angle) * radius * 0.3f);
    const juce::Point<float> end(centre.x + std::cos(angle) * radius * 0.75f,
                                 centre.y + std::sin(angle) * radius * 0.75f);
    g.drawLine(juce::Line<float>(start, end), 2.0f);
}
