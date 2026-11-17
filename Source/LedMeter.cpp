#include "LedMeter.h"

LedMeter::LedMeter() = default;

void LedMeter::setLevels(float leftDb, float rightDb)
{
    leftLevel = juce::jlimit(0.0f, 1.0f, (leftDb + 60.0f) / 60.0f);
    rightLevel = juce::jlimit(0.0f, 1.0f, (rightDb + 60.0f) / 60.0f);
    leftPeak = juce::jmax(leftPeak * 0.985f, leftLevel);
    rightPeak = juce::jmax(rightPeak * 0.985f, rightLevel);
    repaint();
}

void LedMeter::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff101312));
    constexpr int segmentCount = 24;
    constexpr float segmentGap = 4.0f;
    const auto area = getLocalBounds().toFloat().reduced(10.0f, 4.0f);
    const float barWidth = 28.0f;
    const float barHeight = (area.getHeight() - segmentGap * static_cast<float>(segmentCount - 1))
                            / static_cast<float>(segmentCount);
    const std::array<float, 2> levels { leftLevel, rightLevel };
    const std::array<float, 2> peaks { leftPeak, rightPeak };

    for (int channel = 0; channel < 2; ++channel)
    {
        const float x = area.getX() + (channel == 0 ? 8.0f : area.getWidth() - barWidth - 8.0f);
        for (int segment = 0; segment < segmentCount; ++segment)
        {
            // Low signal starts at the bottom, like the tall LED ladders in the reference.
            const float fraction = static_cast<float>(segment + 1) / static_cast<float>(segmentCount);
            const bool active = fraction <= levels[static_cast<size_t>(channel)];
            const bool held = peakHold && fraction <= peaks[static_cast<size_t>(channel)] && !active;
            const float y = area.getBottom() - static_cast<float>(segment + 1) * (barHeight + segmentGap) + segmentGap;

            juce::Colour colour(0xff282b28);
            if (fraction > 0.88f)
                colour = active || held ? juce::Colour(0xffff423a) : colour;
            else if (fraction > 0.70f)
                colour = active || held ? juce::Colour(0xffffcf34) : colour;
            else if (active || held)
                colour = juce::Colour(0xff38f06f);

            g.setColour(colour);
            g.fillRoundedRectangle(x, y, barWidth, barHeight, 3.0f);
            if (active || held)
            {
                g.setColour(colour.withAlpha(0.25f));
                g.fillRoundedRectangle(x - 3.0f, y - 2.0f, barWidth + 6.0f, barHeight + 4.0f, 4.0f);
            }
        }
    }
}

void LedMeter::resized()
{
}
