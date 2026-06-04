#include "FxEffects.h"
#include "FxTheme.h"
#include "DfxDsp.h"

namespace {
const char* kEffectNames[DfxDsp::NumEffects] = {
    "Clarity", "Ambience", "Surround Sound", "Dynamic Boost", "Bass Boost"
};
} // namespace

// ──────────────────────────────────────────────────────────── FxEffects

FxEffects::FxEffects(FxController& controller) : controller_(controller)
{
    labels_.resize(DfxDsp::NumEffects);
    effects_.resize(DfxDsp::NumEffects);

    for (int i = 0; i < DfxDsp::NumEffects; ++i)
    {
        labels_[i] = std::make_unique<juce::Label>(kEffectNames[i], kEffectNames[i]);
        labels_[i]->setJustificationType(juce::Justification::topLeft);
        auto border = labels_[i]->getBorderSize();
        border.setLeft(0);
        labels_[i]->setBorderSize(border);
        addAndMakeVisible(labels_[i].get());

        effects_[i] = std::make_unique<FxEffectSlider>(i, controller_);
        effects_[i]->setSliderStyle(juce::Slider::LinearHorizontal);
        effects_[i]->setRange(0.0, 10.0, 1.0);
        effects_[i]->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible(effects_[i].get());
    }

    setSize(WIDTH, HEIGHT);
}

void FxEffects::update()
{
    for (int i = 0; i < DfxDsp::NumEffects; ++i)
    {
        const float value = controller_.getEffect(static_cast<DfxDsp::Effect>(i));
        effects_[i]->setEffectValue(value * 10.0f);
    }
}

void FxEffects::showValues(bool show)
{
    for (int i = 0; i < DfxDsp::NumEffects; ++i)
        effects_[i]->showValue(show);
}

void FxEffects::enablementChanged()
{
    for (int i = 0; i < DfxDsp::NumEffects; ++i)
    {
        labels_[i]->setEnabled(isEnabled());
        effects_[i]->setEnabled(isEnabled());
    }
    repaint();
}

void FxEffects::resized()
{
    int y = Y_MARGIN;
    for (int i = 0; i < DfxDsp::NumEffects; ++i)
    {
        labels_[i]->setBounds(X_MARGIN + FxTheme::SLIDER_THUMB_RADIUS, y, SLIDER_WIDTH, LABEL_HEIGHT);
        effects_[i]->setBounds(X_MARGIN, labels_[i]->getBottom() + LABEL_GAP, SLIDER_WIDTH, SLIDER_HEIGHT);
        y = effects_[i]->getBottom() + ROW_GAP;
    }
}

void FxEffects::paint(juce::Graphics& g)
{
    g.setFillType(juce::FillType(juce::Colour(FXCOLOR(ControlBackground)).withAlpha(1.0f)));
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 8.0f);
}

// ──────────────────────────────────────────────────────── FxEffectSlider

FxEffects::FxEffectSlider::FxEffectSlider(int effect, FxController& controller)
    : effect_(effect), controller_(controller)
{
    setMouseCursor(juce::MouseCursor::PointingHandCursor);
    setWantsKeyboardFocus(true);

    value_label_.setFont(juce::Font(12.0f));   // smaller than the 17px label font so 0–10 isn't clipped
    value_label_.setJustificationType(juce::Justification::centredLeft);
    value_label_.setInterceptsMouseClicks(false, false);
    addChildComponent(value_label_);
}

void FxEffects::FxEffectSlider::setEffectValue(float displayValue)
{
    setValue(displayValue, juce::dontSendNotification);

    value_label_.setText(juce::String::formatted("%.0f", displayValue), juce::dontSendNotification);
    const auto pos = getPositionOfValue(displayValue);
    value_label_.setBounds(value_label_.getBounds().withX((int)(pos + FxTheme::SLIDER_THUMB_RADIUS + 1)));
}

void FxEffects::FxEffectSlider::showValue(bool show)
{
    value_label_.setVisible(show && isEnabled());
}

void FxEffects::FxEffectSlider::enablementChanged()
{
    setMouseCursor(isEnabled() ? juce::MouseCursor::PointingHandCursor
                               : juce::MouseCursor::NormalCursor);
    value_label_.setVisible(isEnabled() && value_label_.isVisible());
}

void FxEffects::FxEffectSlider::resized()
{
    juce::Slider::resized();
    value_label_.setBounds(value_label_.getX(), (getHeight() - LABEL_HEIGHT) / 2,
                           FxTheme::SLIDER_THUMB_RADIUS * 3, LABEL_HEIGHT);
}

void FxEffects::FxEffectSlider::valueChanged()
{
    const float displayValue = (float)getValue();
    const float controlValue = displayValue / 10.0f;
    const float current = controller_.getEffect(static_cast<DfxDsp::Effect>(effect_));

    if (std::abs(controlValue - current) > 0.001f)
    {
        controller_.setEffect(static_cast<DfxDsp::Effect>(effect_), controlValue);

        value_label_.setText(juce::String::formatted("%.0f", displayValue), juce::dontSendNotification);
        const auto pos = getPositionOfValue(displayValue);
        value_label_.setBounds(value_label_.getBounds().withX((int)(pos + FxTheme::SLIDER_THUMB_RADIUS + 1)));
    }
}

bool FxEffects::FxEffectSlider::keyPressed(const juce::KeyPress& key)
{
    if (!isEnabled()) return false;

    if (key.isKeyCode(juce::KeyPress::upKey))
    {
        setValue(getValue() + getInterval());
        return true;
    }
    if (key.isKeyCode(juce::KeyPress::downKey))
    {
        setValue(getValue() - getInterval());
        return true;
    }
    return false;
}
