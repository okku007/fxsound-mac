#pragma once
#include <juce_core/juce_core.h>

// Tiny persisted app preferences (survives relaunch).
// Stored as plain files under ~/Library/Application Support/FxSound/.
namespace AppSettings
{
    inline juce::File settingsDir()
    {
        return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                   .getChildFile("Application Support/FxSound");
    }

    // true  → close button quits the whole app (default)
    // false → close button hides the window; app keeps running (reopen via Dock icon)
    inline bool closeQuitsApp()
    {
        const auto f = settingsDir().getChildFile("close_quits.setting");
        if (f.existsAsFile())
            return f.loadFileAsString().trim() != "0";
        return true; // default
    }

    inline void setCloseQuitsApp(bool quits)
    {
        const auto f = settingsDir().getChildFile("close_quits.setting");
        f.getParentDirectory().createDirectory();
        f.replaceWithText(quits ? "1" : "0");
    }
}
