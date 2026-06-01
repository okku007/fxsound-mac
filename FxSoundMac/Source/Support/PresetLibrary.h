#pragma once
#include <juce_core/juce_core.h>

class PresetLibrary
{
public:
    void scan(const juce::File& directory);
    int getNumPresets() const { return presets.size(); }
    juce::String getPresetName(int index) const;
    juce::File getPresetFile(int index) const;

private:
    juce::Array<juce::File> presets; // sorted alphabetically by name, case-insensitive
};
