#include "FxEqualizer.h"
#include "FxTheme.h"

// ──────────────────────────────────────────────────────────── helpers

static juce::String freqLabel(float hz)
{
    if (hz >= 10000.0f)
        return juce::String(juce::roundToInt(hz / 1000.0f)) + "k";
    if (hz >= 1000.0f)
    {
        if (hz == (float)(int)hz && (int)(hz / 1000) * 1000 == (int)hz)
            return juce::String((int)(hz / 1000)) + "k";
        return juce::String(hz / 1000.0f, 1) + "k";
    }
    return juce::String((int)hz);
}

// ──────────────────────────────────────────────────────────── FxEqualizer

FxEqualizer::FxEqualizer(FxController& controller) : controller_(controller)
{
    // Do NOT call reinit() here: this constructor runs before MainComponent's body,
    // so the DSP is not yet prepared. paint() will trigger reinit() lazily on first
    // draw, by which time controller.prepare() has already been called.
    setSize(WIDTH, HEIGHT);
}

void FxEqualizer::reinit(int num_bands)
{
    stopTimer();
    highlight_mode_ = false;

    removeAllChildren();
    labels_.clear();
    band_boosts_.clear();
    band_gain_values_.clear();
    band_indices_.clear();

    // Cap display to MAX_DISPLAY_BANDS; select evenly-spaced DSP bands across the
    // full frequency range so we cover bass through treble, not just the low end.
    const int display = juce::jmin(num_bands, MAX_DISPLAY_BANDS);

    labels_.resize((size_t)display);
    band_boosts_.resize((size_t)display);
    band_gain_values_.assign((size_t)display, 0.0f);
    band_indices_.resize((size_t)display);

    for (int i = 0; i < display; ++i)
    {
        // Map display slot i to a DSP band spread evenly across [0, num_bands-1]
        const float t = (display > 1) ? (float)i / (float)(display - 1) : 0.0f;
        band_indices_[i] = (num_bands == display) ? i : juce::roundToInt(t * (float)(num_bands - 1));
    }

    for (int i = 0; i < display; ++i)
    {
        const int dsp_band = band_indices_[i];

        labels_[i] = std::make_unique<juce::Label>();
        labels_[i]->setJustificationType(juce::Justification::centredTop);
        labels_[i]->setText(freqLabel(controller_.getEqBandFrequency(dsp_band)),
                            juce::dontSendNotification);
        addAndMakeVisible(labels_[i].get());

        // Pass the DSP band index so the slider calls setEqBand(dsp_band, ...)
        band_boosts_[i] = std::make_unique<FxEqSlider>(dsp_band, MAX_GAIN, controller_);
        band_boosts_[i]->setSliderStyle(juce::Slider::LinearVertical);
        band_boosts_[i]->setRange(-MAX_GAIN, MAX_GAIN, 1.0);
        band_boosts_[i]->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        band_boosts_[i]->addListener(this);
        addAndMakeVisible(band_boosts_[i].get());
    }

    resized();
}

void FxEqualizer::update()
{
    for (int i = 0; i < (int)band_boosts_.size(); ++i)
    {
        const int dsp = band_indices_[i];
        band_boosts_[i]->setGainValue(controller_.getEqBand(dsp));
        labels_[i]->setText(freqLabel(controller_.getEqBandFrequency(dsp)), juce::dontSendNotification);
    }
}

void FxEqualizer::showValues(bool show)
{
    for (auto& s : band_boosts_)
        s->showValue(show);
}

void FxEqualizer::enablementChanged()
{
    for (int i = 0; i < (int)band_boosts_.size(); ++i)
    {
        band_boosts_[i]->setEnabled(isEnabled());
        labels_[i]->setEnabled(isEnabled());
    }
    repaint();
}

void FxEqualizer::sliderDragStarted(juce::Slider* slider)
{
    if (highlight_mode_) return;

    if (juce::ModifierKeys::getCurrentModifiersRealtime().isAltDown())
    {
        highlight_mode_ = true;
        startTimerHz(30);

        for (int i = 0; i < (int)band_boosts_.size(); ++i)
        {
            if (band_boosts_[i].get() != slider)
            {
                band_boosts_[i]->setEnabled(false);
                band_boosts_[i]->showValue(false);
                band_gain_values_[i] = controller_.getEqBand(band_indices_[i]);
            }
        }
    }
}

void FxEqualizer::sliderDragEnded(juce::Slider* slider)
{
    if (!highlight_mode_) return;

    highlight_mode_ = false;

    for (int i = 0; i < (int)band_boosts_.size(); ++i)
    {
        if (band_boosts_[i].get() != slider)
        {
            band_boosts_[i]->setEnabled(true);
            band_boosts_[i]->showValue(true);
            controller_.setEqBand(band_indices_[i], band_gain_values_[i]);
            band_boosts_[i]->setValue(band_gain_values_[i], juce::dontSendNotification);
        }
    }
}

void FxEqualizer::timerCallback()
{
    if (!highlight_mode_) { stopTimer(); return; }

    bool done = false;
    for (int i = 0; i < (int)band_boosts_.size(); ++i)
    {
        if (!band_boosts_[i]->isEnabled())
        {
            const float gain    = controller_.getEqBand(band_indices_[i]);
            const float target  = -(MAX_GAIN - 2.0f);
            if (std::abs(gain - target) > 0.5f)
            {
                done = true;
                const float next = gain + (gain < target ? 1.0f : -1.0f);
                controller_.setEqBand(band_indices_[i], next);
                band_boosts_[i]->setValue(next, juce::dontSendNotification);
            }
        }
    }
    if (!done) stopTimer();
}

void FxEqualizer::resized()
{
    const int num_bands = (int)band_boosts_.size();
    if (num_bands == 0) return;

    const int usable   = getWidth() - X_MARGIN * 2;
    const int band_w   = usable / num_bands;
    const int thumb4   = FxTheme::SLIDER_THUMB_RADIUS * 4;
    // Clamp slider so it never overflows its column
    const int slider_w = juce::jmin(thumb4, band_w);

    // Scale font down as columns narrow
    const float fontSize = (band_w >= 50) ? 12.0f : (band_w >= 30) ? 10.0f : 8.0f;
    // Skip labels to add breathing room between them for very narrow columns:
    //   < 20 px/band → show every 3rd   (~60 px between visible labels)
    //   < 35 px/band → show every 2nd   (~70 px between visible labels)
    //   otherwise    → show all
    const int labelSkip = (band_w < 20) ? 3 : (band_w < 35) ? 2 : 1;

    for (int i = 0; i < num_bands; ++i)
    {
        const int x        = X_MARGIN + i * band_w;
        const int slider_x = x + (band_w - slider_w) / 2;
        band_boosts_[i]->setBounds(slider_x, Y_MARGIN, slider_w, SLIDER_HEIGHT);

        // Remove default border so the full column is available for text;
        // allow horizontal compression before JUCE falls back to "..."
        labels_[i]->setFont(juce::Font(fontSize));
        labels_[i]->setBorderSize(juce::BorderSize<int>(0));
        labels_[i]->setMinimumHorizontalScale(0.5f);
        // Hide intermediate labels — visible labels now have multi-column spacing
        labels_[i]->setVisible(i % labelSkip == 0 || i == num_bands - 1);
        labels_[i]->setBounds(x, band_boosts_[i]->getBottom() + 6, band_w, LABEL_HEIGHT);
    }
}

void FxEqualizer::paint(juce::Graphics& g)
{
    // Detect band count change — use the same cap as reinit() so we don't loop
    const int num_bands    = controller_.getNumEqBands();
    const int display_bands = juce::jmin(num_bands, MAX_DISPLAY_BANDS);
    if (display_bands != (int)band_boosts_.size())
    {
        reinit(num_bands);
        return;
    }

    g.setFillType(juce::FillType(juce::Colour(FXCOLOR(ControlBackground)).withAlpha(1.0f)));
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 8.0f);

    // Use display count (capped), not the raw DSP band count, for all drawing loops
    const int n = (int)band_boosts_.size();
    if (n < 2) return;

    juce::Colour line_colour = juce::Colour(FXCOLOR(SliderTrack)).withAlpha(1.0f);
    juce::Colour grad_hi     = juce::Colour(FXCOLOR(EqStart)).withAlpha(0.34f);
    juce::Colour grad_lo     = juce::Colour(FXCOLOR(EqEnd)).withAlpha(0.0f);

    if (!isEnabled())
    {
        line_colour = line_colour.withSaturation(0.0f);
        grad_hi     = grad_hi.withSaturation(0.0f);
        grad_lo     = grad_lo.withSaturation(0.0f);
    }
    if (highlight_mode_)
        grad_lo = grad_lo.withSaturation(0.0f);

    // Connecting line between band thumbs
    for (int i = 0; i < n - 1; ++i)
    {
        const float x0 = (float)(band_boosts_[i]->getX()     + band_boosts_[i]->getWidth()     / 2);
        const float x1 = (float)(band_boosts_[i+1]->getX()   + band_boosts_[i+1]->getWidth()   / 2);
        const float y0 = band_boosts_[i]->getPositionOfValue(band_boosts_[i]->getValue())     + Y_MARGIN;
        const float y1 = band_boosts_[i+1]->getPositionOfValue(band_boosts_[i+1]->getValue()) + Y_MARGIN;

        juce::Path seg;
        seg.addLineSegment(juce::Line<float>(x0, y0, x1, y1), 1.0f);
        g.setColour(line_colour);
        g.strokePath(seg, juce::PathStrokeType(1.0f));
    }

    // Filled shape under the curve
    juce::Path fill_path;
    for (int i = 0; i < n; ++i)
    {
        const float x = (float)(band_boosts_[i]->getX() + band_boosts_[i]->getWidth() / 2);
        const float y = band_boosts_[i]->getPositionOfValue(band_boosts_[i]->getValue()) + Y_MARGIN;

        if (i == 0)
            fill_path.startNewSubPath(x, (float)(band_boosts_[i]->getBottom() - FxTheme::SLIDER_THUMB_RADIUS));

        fill_path.lineTo(x, y);

        if (i == n - 1)
            fill_path.lineTo(x, (float)(band_boosts_[i]->getBottom() - FxTheme::SLIDER_THUMB_RADIUS));
    }
    fill_path.closeSubPath();

    const auto gradient = juce::ColourGradient(grad_hi, 0.0f, (float)band_boosts_[1]->getY(),
                                                grad_lo, 0.0f, (float)band_boosts_[1]->getBottom(), false);
    g.setFillType(juce::FillType(gradient));
    g.fillPath(fill_path);
}

// ──────────────────────────────────────────────────────────── FxEqSlider

FxEqualizer::FxEqSlider::FxEqSlider(int band, float max_gain, FxController& controller)
    : band_(band), controller_(controller)
{
    (void)max_gain;
    setMouseCursor(juce::MouseCursor::PointingHandCursor);
    setWantsKeyboardFocus(true);

    gain_label_.setFont(juce::Font(LABEL_HEIGHT));
    gain_label_.setJustificationType(juce::Justification::centred);
    gain_label_.setBorderSize(juce::BorderSize<int>(0));
    gain_label_.setMinimumHorizontalScale(0.5f);
    gain_label_.setInterceptsMouseClicks(false, false);
    addChildComponent(gain_label_);
}

void FxEqualizer::FxEqSlider::setGainValue(float db)
{
    setValue(db, juce::dontSendNotification);

    gain_label_.setText(juce::String::formatted(db == 0.0f ? "%.0f" : "%+.0f", db),
                        juce::dontSendNotification);
    const int y = (int)(getPositionOfValue(db)) - FxTheme::SLIDER_THUMB_RADIUS * 3;
    gain_label_.setBounds(gain_label_.getBounds().withY(y));
}

void FxEqualizer::FxEqSlider::showValue(bool show)
{
    gain_label_.setVisible(show && isEnabled());
}

void FxEqualizer::FxEqSlider::enablementChanged()
{
    setMouseCursor(isEnabled() ? juce::MouseCursor::PointingHandCursor
                               : juce::MouseCursor::NormalCursor);
    if (!isEnabled())
        gain_label_.setVisible(false);
}

void FxEqualizer::FxEqSlider::resized()
{
    juce::Slider::resized();
    gain_label_.setBounds(gain_label_.getX(), gain_label_.getY(), getWidth(), LABEL_HEIGHT);
}

void FxEqualizer::FxEqSlider::valueChanged()
{
    const float db = (float)getValue();
    if (std::abs(db - controller_.getEqBand(band_)) > 0.001f)
    {
        controller_.setEqBand(band_, db);

        gain_label_.setText(juce::String::formatted(db == 0.0f ? "%.0f" : "%+.0f", db),
                            juce::dontSendNotification);
        const int y = (int)(getPositionOfValue(db)) - FxTheme::SLIDER_THUMB_RADIUS * 3;
        gain_label_.setBounds(gain_label_.getBounds().withY(y));
    }
}

bool FxEqualizer::FxEqSlider::keyPressed(const juce::KeyPress& key)
{
    if (!isEnabled()) return false;

    if (key.isKeyCode(juce::KeyPress::upKey))   { setValue(getValue() + getInterval()); return true; }
    if (key.isKeyCode(juce::KeyPress::downKey))  { setValue(getValue() - getInterval()); return true; }
    return false;
}

void FxEqualizer::FxEqSlider::mouseDown(const juce::MouseEvent& e)
{
    if (e.mods.isRightButtonDown())
        setValue(0.0, juce::sendNotification);
    else
        juce::Slider::mouseDown(e);
}
