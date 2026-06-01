#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include "MainWindow.h"

class FxSoundMacApplication : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override { return "FxSoundMac"; }
    const juce::String getApplicationVersion() override { return "0.1.0"; }
    bool moreThanOneInstanceAllowed() override { return false; }

    void initialise(const juce::String&) override { mainWindow.reset(new MainWindow(getApplicationName())); }
    void shutdown() override { mainWindow = nullptr; }
    void systemRequestedQuit() override { quit(); }

private:
    std::unique_ptr<MainWindow> mainWindow;
};
