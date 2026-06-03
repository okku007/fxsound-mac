#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "FxPowerButton.h"
#include "FxEffects.h"
#include "FxEqualizer.h"
#include "SetupStatusPanel.h"
#include "../Support/FxController.h"
#include "../Support/PresetLibrary.h"
#include "../Audio/MacAudioEngine.h"

class MainComponent : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void refreshOutputDevices();
    void refreshPresets();
    void startOrStopAudio();
    void updatePowerState(bool on);

    static juce::File bundledPresetDir();

    // FxController must be first — sub-components hold a reference to it
    FxController   controller;
    MacAudioEngine engine { controller };
    PresetLibrary  presetLibrary;

    std::unique_ptr<juce::Drawable> logo_;

    FxPowerButton    powerButton;
    juce::ComboBox   presetBox;
    juce::ComboBox   outputBox;
    juce::TextButton startStopButton { "Start" };

    // FxEffects / FxEqualizer declared after controller so brace-init works
    FxEffects   effectsPanel  { controller };
    FxEqualizer equalizerPanel { controller };

    SetupStatusPanel statusPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
