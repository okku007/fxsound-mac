#include "MainComponent.h"
#include "../Audio/DeviceValidation.h"

namespace {
const char* kEffectNames[DfxDsp::NumEffects] = {
    "Fidelity", "Ambience", "Surround", "Dynamic Boost", "Bass"
};
}

juce::File MainComponent::bundledPresetDir()
{
    return juce::File::getSpecialLocation(juce::File::currentApplicationFile)
               .getChildFile("Contents/Resources/Presets");
}

MainComponent::MainComponent()
{
    controller.prepare(48000, 512);

    // Power on by default — DSP initializes with bypass=1 (power off),
    // so we must explicitly enable it on startup.
    controller.setPower(true);

    // Power
    addAndMakeVisible(powerButton);
    powerButton.setToggleState(true, juce::dontSendNotification);
    powerButton.onClick = [this] {
        controller.setPower(powerButton.getToggleState());
    };

    // Bypass
    addAndMakeVisible(bypassButton);
    bypassButton.onClick = [this] {
        controller.setBypassed(bypassButton.getToggleState());
    };

    // Preset selector
    addAndMakeVisible(presetBox);
    presetBox.onChange = [this] {
        const int idx = presetBox.getSelectedItemIndex();
        if (idx >= 0)
        {
            const bool ok = controller.loadPreset(presetLibrary.getPresetFile(idx));
            if (! ok)
                statusPanel.setStatus({ AudioEngineState::PresetFailedToLoad,
                                        presetLibrary.getPresetName(idx) });
        }
    };

    // Output selector
    addAndMakeVisible(outputBox);

    // Start/Stop
    addAndMakeVisible(startStopButton);
    startStopButton.onClick = [this] { startOrStopAudio(); };

    // Effect sliders
    for (int i = 0; i < DfxDsp::NumEffects; ++i)
    {
        auto& s = effectSliders[i];
        s.setRange(0.0, 1.0, 0.01);
        s.setValue(0.5, juce::dontSendNotification);
        s.onValueChange = [this, i] {
            controller.setEffect(static_cast<DfxDsp::Effect>(i),
                                 (float) effectSliders[i].getValue());
        };
        addAndMakeVisible(s);

        effectLabels[i].setText(kEffectNames[i], juce::dontSendNotification);
        effectLabels[i].setJustificationType(juce::Justification::centred);
        addAndMakeVisible(effectLabels[i]);
    }

    // Output gain
    outputGainSlider.setRange(LegacyDspAdapter::minOutputGainDb,
                              LegacyDspAdapter::maxOutputGainDb, 0.1);
    outputGainSlider.setValue(0.0, juce::dontSendNotification);
    outputGainSlider.onValueChange = [this] {
        controller.setOutputGainDb((float) outputGainSlider.getValue());
    };
    addAndMakeVisible(outputGainSlider);
    addAndMakeVisible(outputGainLabel);

    // EQ sliders
    buildEqSliders();

    // Status panel
    addAndMakeVisible(statusPanel);

    // Engine status callback (called on message thread)
    engine.onStatusChanged = [this](AudioEngineStatus s) {
        juce::MessageManager::callAsync([this, s] { statusPanel.setStatus(s); });
    };

    // Populate UI
    refreshPresets();
    refreshOutputDevices();

    // Initial status guidance
    const auto bh = DeviceValidation::findBlackHoleDevice(engine.getInputDeviceNames());
    statusPanel.setStatus({ bh.isEmpty() ? AudioEngineState::BlackHoleNotInstalled
                                         : AudioEngineState::NoOutputSelected, {} });

    setSize(720, 580);
}

MainComponent::~MainComponent()
{
    engine.stop();
}

void MainComponent::buildEqSliders()
{
    const int n = juce::jmax(0, controller.getNumEqBands());
    for (int b = 0; b < n; ++b)
    {
        auto* s = new juce::Slider();
        s->setSliderStyle(juce::Slider::LinearVertical);
        s->setRange(-12.0, 12.0, 0.1);
        s->setValue(0.0, juce::dontSendNotification);
        s->onValueChange = [this, b, s] {
            controller.setEqBand(b, (float) s->getValue());
        };
        addAndMakeVisible(s);
        eqSliders.add(s);
    }
}

void MainComponent::refreshPresets()
{
    presetLibrary.scan(bundledPresetDir());
    presetBox.clear(juce::dontSendNotification);
    for (int i = 0; i < presetLibrary.getNumPresets(); ++i)
        presetBox.addItem(presetLibrary.getPresetName(i), i + 1);
    if (presetLibrary.getNumPresets() > 0)
        presetBox.setSelectedItemIndex(0, juce::dontSendNotification);
}

void MainComponent::refreshOutputDevices()
{
    outputBox.clear(juce::dontSendNotification);
    int id = 1;
    for (const auto& name : engine.getOutputDeviceNames())
        if (! name.containsIgnoreCase("BlackHole"))
            outputBox.addItem(name, id++);
}

void MainComponent::startOrStopAudio()
{
    if (engine.isRunning())
    {
        engine.stop();
        startStopButton.setButtonText("Start Audio");
        statusPanel.setStatus({ AudioEngineState::NoOutputSelected,
            "Stopped. If macOS output is still routed to BlackHole, "
            "switch it back to your speakers in System Settings." });
        return;
    }

    const auto status = engine.start(outputBox.getText());
    statusPanel.setStatus(status);
    if (status.isHealthy())
        startStopButton.setButtonText("Stop Audio");
}

void MainComponent::resized()
{
    auto r = getLocalBounds().reduced(12);

    // Row 1: power, bypass, start/stop
    auto row1 = r.removeFromTop(30);
    powerButton.setBounds(row1.removeFromLeft(80));
    row1.removeFromLeft(8);
    bypassButton.setBounds(row1.removeFromLeft(80));
    startStopButton.setBounds(row1.removeFromRight(120));

    r.removeFromTop(8);

    // Row 2: preset + output selectors
    auto row2 = r.removeFromTop(28);
    presetBox.setBounds(row2.removeFromLeft(row2.getWidth() / 2 - 4));
    row2.removeFromLeft(8);
    outputBox.setBounds(row2);

    r.removeFromTop(8);

    // Effect sliders
    auto fxArea = r.removeFromTop(160);
    const int fxW = fxArea.getWidth() / DfxDsp::NumEffects;
    for (int i = 0; i < DfxDsp::NumEffects; ++i)
    {
        auto col = fxArea.removeFromLeft(fxW);
        effectLabels[i].setBounds(col.removeFromTop(20));
        effectSliders[i].setBounds(col.reduced(4, 0));
    }

    r.removeFromTop(8);

    // EQ sliders
    if (! eqSliders.isEmpty())
    {
        auto eqArea = r.removeFromTop(160);
        const int w = eqArea.getWidth() / eqSliders.size();
        for (auto* s : eqSliders)
            s->setBounds(eqArea.removeFromLeft(w).reduced(2, 0));
    }

    r.removeFromTop(8);

    // Output gain
    outputGainLabel.setBounds(r.removeFromTop(20));
    outputGainSlider.setBounds(r.removeFromTop(32));

    r.removeFromTop(8);

    // Status panel — takes remaining space
    statusPanel.setBounds(r);
}
