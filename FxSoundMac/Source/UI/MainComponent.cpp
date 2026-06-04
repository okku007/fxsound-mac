#include "MainComponent.h"
#include "FxTheme.h"
#include "../Audio/DeviceValidation.h"
#include "../Support/AppSettings.h"
#include <BinaryData.h>

namespace {
// ComboBox ids for the preset-management actions (kept clear of real preset ids 1..N)
constexpr int kSavePresetId   = 9001;
constexpr int kImportPresetId = 9002;
constexpr int kDeletePresetId = 9003;

// Layout constants matching Windows FxProView proportions
constexpr int kMargin   = 40;
constexpr int kHeaderH  = 60;
constexpr int kComboH   = 50;
constexpr int kRoutingH = 34;   // "Routing Through BlackHole:" + Start/Stop row
constexpr int kGap      = 8;
constexpr int kMainH    = 242;
constexpr int kStatusH  = 62;
} // namespace

juce::File MainComponent::bundledPresetDir()
{
    return juce::File::getSpecialLocation(juce::File::currentApplicationFile)
               .getChildFile("Contents/Resources/Presets");
}

juce::File MainComponent::userPresetDir()
{
    // Writable, persists across launches: ~/Library/Application Support/FxSound/Presets
    return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
               .getChildFile("Application Support/FxSound/Presets");
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

    // Settings (gear) button — top-right; opens the close-behavior preference.
    settingsIcon_ = juce::Drawable::createFromImageData(BinaryData::settings_svg,
                                                        BinaryData::settings_svgSize);
    settingsButton_.setImages(settingsIcon_.get());
    settingsButton_.setColour(juce::DrawableButton::backgroundColourId,   juce::Colours::transparentBlack);
    settingsButton_.setColour(juce::DrawableButton::backgroundOnColourId, juce::Colours::transparentBlack);
    settingsButton_.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    settingsButton_.onClick = [this] { showSettingsMenu(); };
    addAndMakeVisible(settingsButton_);

    // Preset selector
    addAndMakeVisible(presetBox);
    presetBox.setTextWhenNothingSelected("Select Preset");
    presetBox.onChange = [this] {
        const int id = presetBox.getSelectedId();

        // Action items: run the action, restore the displayed selection, don't "load" them.
        if (id == kSavePresetId || id == kImportPresetId || id == kDeletePresetId)
        {
            presetBox.setSelectedId(currentPresetId_, juce::dontSendNotification);
            if      (id == kSavePresetId)   doSavePreset();
            else if (id == kImportPresetId) doImportPreset();
            else                            doDeletePreset();
            return;
        }

        // Real preset: combo id maps to preset index (id - 1).
        const int idx = id - 1;
        if (idx >= 0)
        {
            currentPresetId_ = id;
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
        layoutTopRow(); // selected name changed → resize combo to fit it
    };

    // Output selector
    addAndMakeVisible(outputBox);
    outputBox.setTextWhenNothingSelected("Select Output");
    outputBox.onChange = [this] {
        layoutTopRow();
        // Live re-route: if audio is already running, switch it to the new device.
        // Status panel updates via engine.onStatusChanged. Idle = handled at Start.
        if (engine.isRunning())
            engine.setOutputDevice(outputBox.getText());
    };

    // Routing control: "Routing Through BlackHole:" label + Start/Stop button.
    // Button text reflects routing state — "Stop" while running, "Start" when idle.
    routingLabel.setText("Routing Through BlackHole:", juce::dontSendNotification);
    routingLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(routingLabel);

    addAndMakeVisible(startStopButton);
    startStopButton.onClick = [this] { startOrStopAudio(); };
    updateStartStopAppearance(false); // idle on launch → grayed

    // Volume control — "Volume" label above the themed slider, with the value shown
    // in a dB read-out box right next to the slider (themed dark box, no harsh border).
    volumeLabel.setText("Volume", juce::dontSendNotification);
    volumeLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(volumeLabel);

    volumeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    volumeSlider.setRange(LegacyDspAdapter::minOutputGainDb, LegacyDspAdapter::maxOutputGainDb, 1.0);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 58, 24);
    volumeSlider.setTextValueSuffix(" dB");
    volumeSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(FXCOLOR(ControlBackground)).withAlpha(1.0f));
    volumeSlider.setColour(juce::Slider::textBoxTextColourId,        juce::Colour(FXCOLOR(DefaultText)).withAlpha(1.0f));
    volumeSlider.setColour(juce::Slider::textBoxOutlineColourId,     juce::Colours::transparentBlack);
    volumeSlider.setValue(controller.getOutputGainDb(), juce::dontSendNotification);
    volumeSlider.onValueChange = [this] {
        controller.setOutputGainDb((float) volumeSlider.getValue());
    };
    addAndMakeVisible(volumeSlider);

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

    setSize(1040, kHeaderH + kComboH + kRoutingH + kGap + kMainH + kGap + kStatusH);
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
    userPresetDir().createDirectory();
    presetLibrary.scan(bundledPresetDir(), userPresetDir());

    presetBox.clear(juce::dontSendNotification);
    for (int i = 0; i < presetLibrary.getNumPresets(); ++i)
        presetBox.addItem(presetLibrary.getPresetName(i), i + 1);

    presetBox.addSeparator();
    presetBox.addItem("Save current as...", kSavePresetId);
    presetBox.addItem("Import...",          kImportPresetId);
    presetBox.addItem("Delete current",     kDeletePresetId);

    if (presetLibrary.getNumPresets() > 0)
    {
        int id = currentPresetId_;
        if (id <= 0 || id > presetLibrary.getNumPresets())
            id = 1;
        currentPresetId_ = id;
        presetBox.setSelectedId(id, juce::dontSendNotification);
    }
}

void MainComponent::doSavePreset()
{
    auto* aw = new juce::AlertWindow("Save Preset",
                                     "Name your preset:",
                                     juce::MessageBoxIconType::NoIcon);
    aw->addTextEditor("name", {}, {});
    aw->addButton("Save",   1, juce::KeyPress(juce::KeyPress::returnKey));
    aw->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));

    aw->enterModalState(true, juce::ModalCallbackFunction::create([this, aw](int result)
    {
        std::unique_ptr<juce::AlertWindow> owned(aw);
        if (result == 0)
            return;

        const juce::String name = aw->getTextEditorContents("name").trim();
        if (name.isEmpty())
            return;

        const juce::String safe = juce::File::createLegalFileName(name);
        const juce::File dest = userPresetDir().getChildFile(safe + ".fac");

        userPresetDir().createDirectory();
        if (controller.savePreset(userPresetDir(), dest.getFileNameWithoutExtension()))
        {
            refreshPresets();
            const int idx = presetLibrary.indexOfFile(dest);
            if (idx >= 0)
            {
                currentPresetId_ = idx + 1;
                presetBox.setSelectedId(currentPresetId_, juce::dontSendNotification);
                layoutTopRow();
            }
        }
        else
        {
            statusPanel.setStatus({ AudioEngineState::PresetFailedToLoad, name });
        }
    }), true);
}

void MainComponent::doImportPreset()
{
    fileChooser_ = std::make_unique<juce::FileChooser>(
        "Import preset (.fac)",
        juce::File::getSpecialLocation(juce::File::userHomeDirectory),
        "*.fac");

    fileChooser_->launchAsync(
        juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc)
        {
            const juce::File src = fc.getResult();
            if (! src.existsAsFile())
                return;

            userPresetDir().createDirectory();
            const juce::File dest = userPresetDir().getChildFile(src.getFileName());
            if (src.copyFileTo(dest))
            {
                refreshPresets();
                const int idx = presetLibrary.indexOfFile(dest);
                if (idx >= 0)
                    presetBox.setSelectedId(idx + 1, juce::sendNotification); // load it
            }
            else
            {
                statusPanel.setStatus({ AudioEngineState::PresetFailedToLoad,
                                        src.getFileNameWithoutExtension() });
            }
        });
}

void MainComponent::doDeletePreset()
{
    const int idx = currentPresetId_ - 1;
    if (! juce::isPositiveAndBelow(idx, presetLibrary.getNumPresets()))
        return;

    if (! presetLibrary.isCustom(idx))
    {
        juce::NativeMessageBox::showMessageBoxAsync(juce::MessageBoxIconType::InfoIcon,
            "Can't delete", "Factory presets can't be deleted.");
        return;
    }

    const juce::File file = presetLibrary.getPresetFile(idx);
    const juce::String name = presetLibrary.getPresetName(idx);

    juce::NativeMessageBox::showOkCancelBox(juce::MessageBoxIconType::QuestionIcon,
        "Delete preset", "Delete \"" + name + "\"? This can't be undone.", nullptr,
        juce::ModalCallbackFunction::create([this, file](int r)
        {
            if (r != 1)
                return;
            file.deleteFile();
            currentPresetId_ = 1;
            refreshPresets();
            presetBox.setSelectedId(currentPresetId_, juce::dontSendNotification);
            layoutTopRow();
        }));
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
        updateStartStopAppearance(false);
        statusPanel.setStatus({ AudioEngineState::NoOutputSelected,
            "Stopped. If macOS output is still routed to BlackHole, "
            "switch it back to your speakers in System Settings." });
        return;
    }

    const auto status = engine.start(outputBox.getText());
    statusPanel.setStatus(status);
    if (status.isHealthy())
        updateStartStopAppearance(true);
}

void MainComponent::updateStartStopAppearance(bool running)
{
    startStopButton.setButtonText(running ? "Stop" : "Start");

    if (running)
    {
        // Active routing — solid accent red, bright text.
        startStopButton.setColour(juce::TextButton::buttonColourId,
                                  juce::Colour(FXCOLOR(TextButtonBackground)).withAlpha(1.0f));
        startStopButton.setColour(juce::TextButton::textColourOffId,
                                  juce::Colour(FXCOLOR(HighlightedText)).withAlpha(1.0f));
    }
    else
    {
        // Idle — grayed-out/muted look (still clickable).
        startStopButton.setColour(juce::TextButton::buttonColourId,
                                  juce::Colour(FXCOLOR(Outline)).withAlpha(1.0f));
        startStopButton.setColour(juce::TextButton::textColourOffId,
                                  juce::Colour(FXCOLOR(DefaultText)).withAlpha(0.7f));
    }
    startStopButton.repaint();
}

void MainComponent::showSettingsMenu()
{
    juce::PopupMenu m;
    const bool quits = AppSettings::closeQuitsApp();
    m.addSectionHeader("Close button behavior");
    m.addItem(1, "Quit FxSound",                 true, quits);
    m.addItem(2, "Keep running (hide window)",   true, !quits);

    m.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(settingsButton_),
        [](int result)
        {
            if (result == 1)      AppSettings::setCloseQuitsApp(true);
            else if (result == 2) AppSettings::setCloseQuitsApp(false);
        });
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
    const int mainY = kHeaderH + kComboH + kRoutingH + kGap;
    g.setFillType(juce::FillType(juce::Colour(FXCOLOR(PanelBackground)).withAlpha(0.15f)));
    g.fillRoundedRectangle(20.0f,
                            (float)(mainY - 12),
                            (float)(getWidth() - 40),
                            (float)(kMainH + 20),
                            8.0f);
}

void MainComponent::resized()
{
    // Header: power button (left) + volume control right beside it, styled like the
    // effect sliders ("Volume" label above, value next to the thumb, no dB box).
    powerButton.setBounds(135, 12, 36, 36);
    settingsButton_.setBounds(getWidth() - kMargin - 26, 14, 26, 26);

    const int volBlockW = 260; // slider track + dB read-out box
    const int volX = powerButton.getRight() + 24;
    volumeLabel.setBounds(volX, 6, 80, 18);
    volumeSlider.setBounds(volX, 26, volBlockW, 26);

    // Combos row — widths fit their content (see layoutTopRow)
    layoutTopRow();

    // Routing row: "Routing Through BlackHole:" + Start/Stop, above the panels
    const int routingY = kHeaderH + kComboH;
    auto rlFont = getLookAndFeel().getLabelFont(routingLabel);
    const int rlW = rlFont.getStringWidth(routingLabel.getText()) + 4;
    const int btnH = 28;
    routingLabel.setBounds(kMargin, routingY + (kRoutingH - 22) / 2, rlW, 22);
    startStopButton.setBounds(routingLabel.getRight() + 10,
                              routingY + (kRoutingH - btnH) / 2, 90, btnH);

    // Main panels — matching Windows FxProView geometry
    const int mainY = kHeaderH + kComboH + kRoutingH + kGap;
    effectsPanel.setBounds(kMargin, mainY, 168, 242);
    equalizerPanel.setBounds(kMargin + 168 + 16, mainY, 776, 242);

    // Status
    const int statusY = mainY + kMainH + kGap;
    statusPanel.setBounds(kMargin, statusY, getWidth() - kMargin * 2, kStatusH);
}

// Size the preset + output combos to fit their currently-selected text (with a
// sensible min/max), then lay them left-to-right. Avoids the fixed oversized boxes.
void MainComponent::layoutTopRow()
{
    const int comboY = kHeaderH + 6;
    const int comboH = 38;

    auto contentWidth = [](juce::ComboBox& box, int minW, int maxW)
    {
        juce::Font f = box.getLookAndFeel().getComboBoxFont(box);
        juce::String t = box.getText();
        if (t.isEmpty()) t = box.getTextWhenNothingSelected();
        const int w = f.getStringWidth(t) + 10 /*left pad*/ + 42 /*arrow + right pad*/;
        return juce::jlimit(minW, maxW, w);
    };

    presetBox.setBounds(kMargin, comboY, contentWidth(presetBox, 130, 320), comboH);
    outputBox.setBounds(presetBox.getRight() + 12, comboY,
                        contentWidth(outputBox, 160, 460), comboH);
}
