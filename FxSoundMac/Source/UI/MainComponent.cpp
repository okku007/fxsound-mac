#include "MainComponent.h"
#include "FxTheme.h"
#include "../Audio/DeviceValidation.h"
#include <BinaryData.h>

namespace {
// Layout constants matching Windows FxProView proportions
constexpr int kMargin   = 40;
constexpr int kHeaderH  = 60;
constexpr int kComboH   = 50;
constexpr int kGap      = 8;
constexpr int kMainH    = 242;
constexpr int kStatusH  = 62;
} // namespace

juce::File MainComponent::bundledPresetDir()
{
    return juce::File::getSpecialLocation(juce::File::currentApplicationFile)
               .getChildFile("Contents/Resources/Presets");
}

MainComponent::MainComponent()
{
    controller.prepare(48000, 512);
    controller.setPower(true);

    // Logo SVG (drawn in paint)
    logo_ = juce::Drawable::createFromImageData(BinaryData::logowhite_svg,
                                                 BinaryData::logowhite_svgSize);

    // Power button
    powerButton.setPowerState(true);
    powerButton.onClick = [this] { updatePowerState(powerButton.getToggleState()); };
    addAndMakeVisible(powerButton);

    // Preset selector
    addAndMakeVisible(presetBox);
    presetBox.setTextWhenNothingSelected("Select Preset");
    presetBox.onChange = [this] {
        const int idx = presetBox.getSelectedItemIndex();
        if (idx >= 0)
        {
            const bool ok = controller.loadPreset(presetLibrary.getPresetFile(idx));
            if (ok)
            {
                effectsPanel.update();
                equalizerPanel.update();
            }
            else
            {
                statusPanel.setStatus({ AudioEngineState::PresetFailedToLoad,
                                        presetLibrary.getPresetName(idx) });
            }
        }
    };

    // Output selector
    addAndMakeVisible(outputBox);
    outputBox.setTextWhenNothingSelected("Select Output");

    // Start / Stop
    addAndMakeVisible(startStopButton);
    startStopButton.onClick = [this] { startOrStopAudio(); };

    // Main panels
    addAndMakeVisible(effectsPanel);
    addAndMakeVisible(equalizerPanel);

    // Status
    addAndMakeVisible(statusPanel);

    // Engine status callback (always called on message thread by MacAudioEngine)
    engine.onStatusChanged = [this](AudioEngineStatus s) {
        juce::MessageManager::callAsync([this, s] { statusPanel.setStatus(s); });
    };

    // Populate dropdowns
    refreshPresets();
    refreshOutputDevices();

    // Prime panels from current DSP state
    updatePowerState(true);
    effectsPanel.update();
    equalizerPanel.update();
    equalizerPanel.showValues(true);
    effectsPanel.showValues(true);

    // Initial status hint
    const auto bh = DeviceValidation::findBlackHoleDevice(engine.getInputDeviceNames());
    statusPanel.setStatus({ bh.isEmpty() ? AudioEngineState::BlackHoleNotInstalled
                                         : AudioEngineState::NoOutputSelected, {} });

    setSize(1040, kHeaderH + kComboH + kGap + kMainH + kGap + kStatusH);
}

MainComponent::~MainComponent()
{
    engine.stop();
}

void MainComponent::updatePowerState(bool on)
{
    controller.setPower(on);
    effectsPanel.setEnabled(on);
    equalizerPanel.setEnabled(on);
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
        if (!name.containsIgnoreCase("BlackHole"))
            outputBox.addItem(name, id++);
}

void MainComponent::startOrStopAudio()
{
    if (engine.isRunning())
    {
        engine.stop();
        startStopButton.setButtonText("Start");
        statusPanel.setStatus({ AudioEngineState::NoOutputSelected,
            "Stopped. If macOS output is still routed to BlackHole, "
            "switch it back to your speakers in System Settings." });
        return;
    }

    const auto status = engine.start(outputBox.getText());
    statusPanel.setStatus(status);
    if (status.isHealthy())
        startStopButton.setButtonText("Stop");
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(FXCOLOR(WindowBackground)));

    // FxSound logo in header area
    if (logo_)
        logo_->drawWithin(g, juce::Rectangle<float>(16.0f, 10.0f, 110.0f, 40.0f),
                          juce::RectanglePlacement::centred
                              | juce::RectanglePlacement::onlyReduceInSize,
                          1.0f);

    // Subtle panel background behind the effects + EQ area
    g.setFillType(juce::FillType(juce::Colour(FXCOLOR(PanelBackground)).withAlpha(0.15f)));
    g.fillRoundedRectangle(20.0f,
                            (float)(kHeaderH + kComboH - 4),
                            (float)(getWidth() - 40),
                            (float)(kMainH + 20),
                            8.0f);
}

void MainComponent::resized()
{
    // Header: power button sits right of logo area
    powerButton.setBounds(135, 12, 36, 36);

    // Combos row
    const int comboY = kHeaderH + 6;
    presetBox.setBounds(kMargin, comboY, 450, 38);
    outputBox.setBounds(530, comboY, 390, 38);
    startStopButton.setBounds(925, comboY, 80, 38);

    // Main panels — matching Windows FxProView geometry
    const int mainY = kHeaderH + kComboH + kGap;
    effectsPanel.setBounds(kMargin, mainY, 168, 242);
    equalizerPanel.setBounds(kMargin + 168 + 16, mainY, 776, 242);

    // Status
    const int statusY = kHeaderH + kComboH + kGap + kMainH + kGap;
    statusPanel.setBounds(kMargin, statusY, getWidth() - kMargin * 2, kStatusH);
}
