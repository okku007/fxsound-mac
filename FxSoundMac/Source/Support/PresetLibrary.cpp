#include "PresetLibrary.h"

void PresetLibrary::scan(const juce::File& directory)
{
    presets.clear();
    if (! directory.isDirectory())
        return;

    for (const auto& f : directory.findChildFiles(juce::File::findFiles, false, "*.fac"))
        presets.add(f);

    // Sort by file name, case-insensitive, for stable UI ordering.
    std::sort(presets.begin(), presets.end(), [](const juce::File& a, const juce::File& b)
    {
        return a.getFileNameWithoutExtension().compareIgnoreCase(
               b.getFileNameWithoutExtension()) < 0;
    });
}

juce::String PresetLibrary::getPresetName(int index) const
{
    if (juce::isPositiveAndBelow(index, presets.size()))
        return presets[index].getFileNameWithoutExtension();
    return {};
}

juce::File PresetLibrary::getPresetFile(int index) const
{
    if (juce::isPositiveAndBelow(index, presets.size()))
        return presets[index];
    return {};
}
