#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "FxPowerButton.h"
#include "FxEffects.h"
#include "FxEqualizer.h"
#include "SetupStatusPanel.h"
#include "../Support/FxController.h"
#include "../Support/PresetLibrary.h"
#include "../Audio/MacAudioEngine.h"
#include "../DSP/LegacyDspAdapter.h"

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
    void updateStartStopAppearance(bool running);  // gray when stopped, solid red when running
    void layoutTopRow();          // size preset/output combos to their content + reflow

    void doSavePreset();          // prompt for a name, save current state as a custom preset
    void doImportPreset();        // pick a .fac off disk, copy into the user preset folder
    void doDeletePreset();        // delete the current preset (custom only)
    void showSettingsMenu();      // gear button → close-behavior preference

    static juce::File bundledPresetDir();
    static juce::File userPresetDir();   // ~/Library/Application Support/FxSound/Presets

    // FxController must be first — sub-components hold a reference to it
    FxController   controller;
    MacAudioEngine engine { controller };
    PresetLibrary  presetLibrary;

    std::unique_ptr<juce::Drawable> logo_;
    std::unique_ptr<juce::Drawable> settingsIcon_;

    int currentPresetId_ = 0;                       // ComboBox id of the active preset
    std::unique_ptr<juce::FileChooser> fileChooser_; // kept alive during async import

    FxPowerButton        powerButton;
    juce::DrawableButton settingsButton_ { "settings", juce::DrawableButton::ImageFitted };
    juce::ComboBox   presetBox;
    juce::ComboBox   outputBox;
    juce::Label      routingLabel;
    juce::TextButton startStopButton { "Start" };

    juce::Label  volumeLabel;
    juce::Slider volumeSlider;

    // FxEffects / FxEqualizer declared after controller so brace-init works
    FxEffects   effectsPanel  { controller };
    FxEqualizer equalizerPanel { controller };

    SetupStatusPanel statusPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
