#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../Audio/AudioEngineStatus.h"

// Displays the current audio engine status with user-facing guidance text.
class SetupStatusPanel : public juce::Component
{
public:
    SetupStatusPanel();
    void setStatus(const AudioEngineStatus& status);
    void resized() override;

private:
    juce::Label statusLabel;
};
