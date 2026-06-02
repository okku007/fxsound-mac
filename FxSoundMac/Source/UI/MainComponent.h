#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "SetupStatusPanel.h"
#include "../Support/FxController.h"
#include "../Support/PresetLibrary.h"
#include "../Audio/MacAudioEngine.h"

class MainComponent : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override;
    void resized() override;

private:
    void refreshOutputDevices();
    void refreshPresets();
    void startOrStopAudio();
    void buildEqSliders();

    static juce::File bundledPresetDir();

    FxController controller;
    MacAudioEngine engine { controller };
    PresetLibrary presetLibrary;

    juce::ToggleButton powerButton  { "Power" };
    juce::ToggleButton bypassButton { "Bypass" };
    juce::ComboBox presetBox;
    juce::ComboBox outputBox;
    juce::TextButton startStopButton { "Start Audio" };

    juce::Slider effectSliders[DfxDsp::NumEffects];
    juce::Label  effectLabels[DfxDsp::NumEffects];

    juce::OwnedArray<juce::Slider> eqSliders;
    juce::OwnedArray<juce::Label>  eqFreqLabels;

    juce::Slider outputGainSlider;
    juce::Label  outputGainLabel { {}, "Output Gain (dB)" };

    SetupStatusPanel statusPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
