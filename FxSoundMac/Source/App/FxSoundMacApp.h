#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include "MainWindow.h"
#include "FxTheme.h"

class FxSoundMacApplication : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override { return "FxSound"; }
    const juce::String getApplicationVersion() override { return "0.1.0"; }
    bool moreThanOneInstanceAllowed() override { return false; }

    void initialise(const juce::String&) override
    {
        // Theme must be set before any Component is created.
        theme_ = std::make_unique<FxTheme>();
        juce::LookAndFeel::setDefaultLookAndFeel(theme_.get());

        mainWindow = std::make_unique<MainWindow>(getApplicationName());
    }

    void shutdown() override
    {
        mainWindow = nullptr;
        juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
        theme_ = nullptr;
    }

    void systemRequestedQuit() override { quit(); }

private:
    std::unique_ptr<FxTheme>     theme_;
    std::unique_ptr<MainWindow>  mainWindow;
};
