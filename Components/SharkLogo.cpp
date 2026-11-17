#include "SharkLogo.h"

void SharkLogo::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.setColour(juce::Colour(0xff7bc7a4));

    juce::Path shark;
    shark.startNewSubPath(bounds.getX() + 6.0f, bounds.getY() + 32.0f);
    shark.lineTo(bounds.getX() + 14.0f, bounds.getY() + 22.0f);
    shark.lineTo(bounds.getX() + 24.0f, bounds.getY() + 16.0f);
    shark.lineTo(bounds.getX() + 35.0f, bounds.getY() + 10.0f);
    shark.lineTo(bounds.getX() + 46.0f, bounds.getY() + 8.0f);
    shark.lineTo(bounds.getX() + 52.0f, bounds.getY() + 14.0f);
    shark.lineTo(bounds.getX() + 57.0f, bounds.getY() + 18.0f);
    shark.lineTo(bounds.getX() + 46.0f, bounds.getY() + 25.0f);
    shark.lineTo(bounds.getX() + 42.0f, bounds.getY() + 33.0f);
    shark.lineTo(bounds.getX() + 30.0f, bounds.getY() + 38.0f);
    shark.lineTo(bounds.getX() + 18.0f, bounds.getY() + 38.0f);
    shark.closeSubPath();

    g.fillPath(shark);

    g.setColour(juce::Colour(0xff101512));
    g.fillEllipse(bounds.getCentreX() - 2.5f, bounds.getCentreY() - 6.0f, 5.0f, 5.0f);
}
