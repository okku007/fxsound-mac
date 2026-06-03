#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class FxPowerButton : public juce::Button
{
public:
    FxPowerButton();
    ~FxPowerButton() override = default;

    void setPowerState(bool on, juce::NotificationType n = juce::dontSendNotification);
    bool getPowerState() const { return getToggleState(); }

private:
    void paintButton(juce::Graphics& g, bool, bool) override;
    void loadImages();

    std::unique_ptr<juce::Drawable> power_on_image_;
    std::unique_ptr<juce::Drawable> power_off_image_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FxPowerButton)
};
