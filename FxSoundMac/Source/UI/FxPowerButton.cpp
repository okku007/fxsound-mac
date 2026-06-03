#include "FxPowerButton.h"
#include <BinaryData.h>

FxPowerButton::FxPowerButton() : juce::Button("power")
{
    setClickingTogglesState(true);
    setMouseCursor(juce::MouseCursor::PointingHandCursor);
    loadImages();
}

void FxPowerButton::setPowerState(bool on, juce::NotificationType n)
{
    setToggleState(on, n);
}

void FxPowerButton::paintButton(juce::Graphics& g, bool, bool)
{
    auto& img = getToggleState() ? power_on_image_ : power_off_image_;
    if (img)
        img->drawWithin(g, getLocalBounds().toFloat(),
                        juce::RectanglePlacement::centred | juce::RectanglePlacement::onlyReduceInSize,
                        1.0f);
}

void FxPowerButton::loadImages()
{
    power_on_image_  = juce::Drawable::createFromImageData(BinaryData::power_on_svg,  BinaryData::power_on_svgSize);
    power_off_image_ = juce::Drawable::createFromImageData(BinaryData::power_off_svg, BinaryData::power_off_svgSize);
}
