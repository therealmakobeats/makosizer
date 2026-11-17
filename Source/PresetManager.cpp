#include "PresetManager.h"

bool PresetManager::savePreset(const juce::File& file) const
{
    const auto xml = parameters.copyState().createXml();

    return xml != nullptr
        && xml->writeTo(file);
}

bool PresetManager::loadPreset(const juce::File& file)
{
    if (!file.existsAsFile())
        return false;

    const auto xml = juce::XmlDocument::parse(file);

    if (xml == nullptr)
        return false;

    const auto tree = juce::ValueTree::fromXml(*xml);

    if (!tree.isValid())
        return false;

    if (!tree.hasType(parameters.state.getType()))
        return false;

    parameters.replaceState(tree);

    return true;
}
