#include "LedMeter.h"

LedMeter::LedMeter() = default;

void LedMeter::setLevels(float leftDb, float rightDb)
{
    leftLevel = juce::jlimit(0.0f, 1.0f, (leftDb + 60.0f) / 60.0f);
    rightLevel = juce::jlimit(0.0f, 1.0f, (rightDb + 60.0f) / 60.0f);
    leftPeak = juce::jmax(leftPeak, leftLevel);
    rightPeak = juce::jmax(rightPeak, rightLevel);
    repaint();
}

void LedMeter::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff101512));
    constexpr int segmentCount = 20;
    constexpr float gap = 3.0f;
    const auto bounds = getLocalBounds().toFloat().reduced(8.0f);
    const float rowHeight = (bounds.getHeight() - gap) * 0.5f;
    const float segmentWidth = (bounds.getWidth() - gap * static_cast<float>(segmentCount - 1)) / static_cast<float>(segmentCount);

    for (int row = 0; row < 2; ++row)
    {
        const float level = row == 0 ? leftLevel : rightLevel;
        const float peak = row == 0 ? leftPeak : rightPeak;
        const float y = bounds.getY() + static_cast<float>(row) * (rowHeight + gap);
        for (int segment = 0; segment < segmentCount; ++segment)
        {
            const float x = bounds.getX() + static_cast<float>(segment) * (segmentWidth + gap);
            const float fraction = static_cast<float>(segment + 1) / static_cast<float>(segmentCount);
            const bool active = fraction <= level;
            const bool peakSegment = peakHold && fraction <= peak && fraction > level;
            juce::Colour colour = juce::Colour(0xff26332b);
            if (fraction > 0.85f)
                colour = active || peakSegment ? juce::Colour(0xffe34d4d) : colour;
            else if (fraction > 0.65f)
                colour = active || peakSegment ? juce::Colour(0xffffc84a) : colour;
            else if (active || peakSegment)
                colour = juce::Colour(0xff58df79);
            g.setColour(colour);
            g.fillRoundedRectangle(x, y, segmentWidth, rowHeight, 2.0f);
        }
    }
}

void LedMeter::resized()
{
}
