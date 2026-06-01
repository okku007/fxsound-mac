#pragma once
#include <juce_gui_extra/juce_gui_extra.h>

class MainWindow : public juce::DocumentWindow
{
public:
    explicit MainWindow(const juce::String& name);
    void closeButtonPressed() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};
