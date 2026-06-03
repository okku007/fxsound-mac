#include "FxTheme.h"
#include <BinaryData.h>

const juce::uint32 FxTheme::theme_colors_[FxColor::NumColors] = {
    0x181818, // WindowBackground
    0x181818, // WidgetBackground
    0x383838, // MenuBackground
    0x2b2b2b, // Outline
    0xb1b1b1, // DefaultText
    0x000000, // DefaultFill
    0xffffff, // HighlightedText
    0x0c0c0c, // HighlightedFill
    0xffffff, // MenuText
    0x000000, // ComboBoxBackground
    0xd51535, // TextButtonBackground
    0xe63462, // ImageButton
    0x0f0f0f, // ControlBackground
    0xe33250, // SliderTrack
    0xf7546f, // SliderHighlight
    0xd51535, // GraphHigh
    0xfe566a, // GraphLow
    0xef4b65, // EqStart
    0x742834, // EqEnd
    0xf3f3f3, // VerticalSliderLow
    0x000000, // PanelBackground
};

FxTheme::FxTheme() : LookAndFeel_V4()
{
    init();
}

void FxTheme::init()
{
    setColourScheme(getFxColourScheme());

    setColour(juce::ComboBox::arrowColourId,                  juce::Colour(FXCOLOR(ImageButton)).withAlpha(1.0f));
    setColour(juce::ComboBox::backgroundColourId,              juce::Colour(FXCOLOR(ComboBoxBackground)).withAlpha(1.0f));
    setColour(juce::ComboBox::outlineColourId,                 juce::Colour(FXCOLOR(ComboBoxBackground)).withAlpha(1.0f));
    setColour(juce::ComboBox::focusedOutlineColourId,          juce::Colour(FXCOLOR(SliderHighlight)).withAlpha(0.2f));
    setColour(juce::ComboBox::textColourId,                    juce::Colour(FXCOLOR(DefaultText)).withAlpha(1.0f));
    setColour(juce::TextEditor::backgroundColourId,            juce::Colour(FXCOLOR(DefaultFill)).withAlpha(1.0f));
    setColour(juce::TextEditor::outlineColourId,               juce::Colour(FXCOLOR(DefaultFill)).withAlpha(1.0f));
    setColour(juce::TextEditor::focusedOutlineColourId,        juce::Colour(FXCOLOR(DefaultFill)).withAlpha(1.0f));
    setColour(juce::TextEditor::textColourId,                  juce::Colour(FXCOLOR(DefaultText)).withAlpha(1.0f));
    setColour(juce::TextEditor::highlightedTextColourId,       juce::Colour(FXCOLOR(HighlightedText)).withAlpha(1.0f));
    setColour(juce::TextButton::buttonColourId,                juce::Colour(FXCOLOR(TextButtonBackground)).withAlpha(1.0f));
    setColour(juce::TextButton::buttonOnColourId,              juce::Colour(FXCOLOR(TextButtonBackground)).withAlpha(1.0f));
    setColour(juce::TextButton::textColourOffId,               juce::Colour(FXCOLOR(HighlightedText)).withAlpha(1.0f));
    setColour(juce::TextButton::textColourOnId,                juce::Colour(FXCOLOR(HighlightedText)).withAlpha(1.0f));
    setColour(juce::HyperlinkButton::textColourId,             juce::Colour(FXCOLOR(HighlightedText)).withAlpha(1.0f));
    setColour(juce::CaretComponent::caretColourId,             juce::Colour(FXCOLOR(DefaultText)).withAlpha(1.0f));
    setColour(juce::PopupMenu::backgroundColourId,             juce::Colour(FXCOLOR(WidgetBackground)).withAlpha(1.0f));
    setColour(juce::PopupMenu::highlightedBackgroundColourId,  juce::Colour(FXCOLOR(ImageButton)).withAlpha(1.0f));
    setColour(juce::Slider::rotarySliderOutlineColourId,       juce::Colour(FXCOLOR(SliderTrack)).withAlpha(0.2f));
    setColour(juce::Slider::rotarySliderFillColourId,          juce::Colour(FXCOLOR(SliderTrack)).withAlpha(1.0f));
    setColour(juce::ScrollBar::thumbColourId,                  juce::Colour(FXCOLOR(SliderTrack)).withAlpha(1.0f));
    setColour(juce::Label::textColourId,                       juce::Colour(FXCOLOR(DefaultText)).withAlpha(1.0f));

    font_400_ = juce::Typeface::createSystemTypefaceFor(BinaryData::GilroyRegular_ttf,  BinaryData::GilroyRegular_ttfSize);
    font_600_ = juce::Typeface::createSystemTypefaceFor(BinaryData::GilroySemibold_ttf, BinaryData::GilroySemibold_ttfSize);
    font_700_ = juce::Typeface::createSystemTypefaceFor(BinaryData::GilroyBold_ttf,     BinaryData::GilroyBold_ttfSize);

    if (font_400_ != nullptr) setDefaultSansSerifTypeface(font_400_);

    drop_down_arrow_      = juce::Drawable::createFromImageData(BinaryData::dropdown_arrow_hover_svg, BinaryData::dropdown_arrow_hover_svgSize);
    slider_thumb_         = juce::Drawable::createFromImageData(BinaryData::Slider_Thumb_svg,         BinaryData::Slider_Thumb_svgSize);
    drop_down_arrow_grey_ = juce::Drawable::createFromImageData(BinaryData::dropdown_arrow_bw_svg,    BinaryData::dropdown_arrow_bw_svgSize);
    slider_thumb_grey_    = juce::Drawable::createFromImageData(BinaryData::Slider_Thumb_bw_svg,      BinaryData::Slider_Thumb_bw_svgSize);
}

juce::LookAndFeel_V4::ColourScheme FxTheme::getFxColourScheme()
{
    return {
        juce::Colour(FXCOLOR(WindowBackground)).withAlpha(1.0f),
        juce::Colour(FXCOLOR(WidgetBackground)).withAlpha(1.0f),
        juce::Colour(FXCOLOR(MenuBackground)).withAlpha(1.0f),
        juce::Colour(FXCOLOR(Outline)).withAlpha(1.0f),
        juce::Colour(FXCOLOR(DefaultText)).withAlpha(1.0f),
        juce::Colour(FXCOLOR(DefaultFill)).withAlpha(0.2f),
        juce::Colour(FXCOLOR(HighlightedText)).withAlpha(1.0f),
        juce::Colour(FXCOLOR(HighlightedFill)).withAlpha(1.0f),
        juce::Colour(FXCOLOR(MenuText)).withAlpha(1.0f)
    };
}

juce::Label* FxTheme::createComboBoxTextBox(juce::ComboBox& box)
{
    auto* label = LookAndFeel_V4::createComboBoxTextBox(box);
    label->setMouseCursor(juce::MouseCursor::PointingHandCursor);
    return label;
}

juce::Font FxTheme::getComboBoxFont(juce::ComboBox&)
{
    return juce::Font(font_600_).withHeight(17.0f);
}

void FxTheme::positionComboBoxText(juce::ComboBox& box, juce::Label& label)
{
    label.setMinimumHorizontalScale(1.0f);
    LookAndFeel_V4::positionComboBoxText(box, label);
    label.setBounds(label.getBounds().withX(10));
}

void FxTheme::drawComboBox(juce::Graphics& g, int width, int height, bool,
                            int, int, int, int, juce::ComboBox& box)
{
    auto cornerSize = (float)height / 5.0f;
    juce::Rectangle<int> boxBounds(0, 0, width, height);

    g.setColour(box.findColour(juce::ComboBox::backgroundColourId));
    g.fillRoundedRectangle(boxBounds.toFloat(), cornerSize);

    if (box.hasKeyboardFocus(true))
        g.setColour(box.findColour(juce::ComboBox::focusedOutlineColourId));
    else
        g.setColour(box.findColour(juce::ComboBox::outlineColourId));
    g.drawRoundedRectangle(boxBounds.toFloat().reduced(0.5f, 0.5f), cornerSize, 1.0f);

    int margin = (width <= 150) ? 24 : 32;
    if (box.isEnabled() && drop_down_arrow_)
        drop_down_arrow_->drawWithin(g, juce::Rectangle<float>((float)(width - margin), 0.0f, 12.0f, (float)height),
                                     { juce::RectanglePlacement::centred }, 1.0f);
    else if (!box.isEnabled() && drop_down_arrow_grey_)
        drop_down_arrow_grey_->drawWithin(g, juce::Rectangle<float>((float)(width - margin), 0.0f, 12.0f, (float)height),
                                          { juce::RectanglePlacement::centred }, 1.0f);
}

void FxTheme::drawComboBoxTextWhenNothingSelected(juce::Graphics& g, juce::ComboBox& box, juce::Label& label)
{
    g.setColour(findColour(juce::ComboBox::textColourId).withMultipliedAlpha(0.5f));
    auto font = label.getLookAndFeel().getLabelFont(label);
    g.setFont(font);
    auto textArea = getLabelBorderSize(label).subtractedFrom(label.getLocalBounds().withX(10));
    g.drawFittedText(box.getTextWhenNothingSelected(), textArea, label.getJustificationType(),
                     juce::jmax(1, (int)((float)textArea.getHeight() / font.getHeight())),
                     label.getMinimumHorizontalScale());
}

void FxTheme::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                                float sliderPos, float, float,
                                juce::Slider::SliderStyle style, juce::Slider& slider)
{
    if (style == juce::Slider::LinearVertical)
    {
        float dash_lengths[] = { 5.0f, 2.0f };
        auto radius = getSliderThumbRadius(slider);

        juce::Colour colour1 = juce::Colour(FXCOLOR(SliderTrack)).withAlpha(0.4f);
        juce::Colour colour2 = juce::Colour(FXCOLOR(VerticalSliderLow)).withAlpha(0.4f);
        if (!slider.isEnabled())
        {
            colour1 = colour1.withSaturation(0.0f);
            colour2 = colour2.withSaturation(0.0f);
        }

        g.setGradientFill(juce::ColourGradient(colour1, { 0.0f, 0.0f }, colour2, { 0.0f, (float)height }, false));
        g.drawDashedLine(juce::Line<float>((float)(x + width / 2), (float)y,
                                           (float)(x + width / 2), (float)(y + height)),
                         dash_lengths, (int)std::size(dash_lengths), 1.0f);

        if (slider.isEnabled() && slider_thumb_)
            slider_thumb_->drawWithin(g, juce::Rectangle<float>((float)(x + width / 2 - radius),
                                                                  sliderPos - (float)radius,
                                                                  (float)(radius * 2), (float)(radius * 2)),
                                      { juce::RectanglePlacement::centred }, 1.0f);
        else if (!slider.isEnabled() && slider_thumb_grey_)
            slider_thumb_grey_->drawWithin(g, juce::Rectangle<float>((float)(x + width / 2 - radius),
                                                                       sliderPos - (float)radius,
                                                                       (float)(radius * 2), (float)(radius * 2)),
                                           { juce::RectanglePlacement::centred }, 1.0f);

        if (slider.getThumbBeingDragged() >= 0 || slider.hasKeyboardFocus(true))
        {
            g.setFillType(juce::Colour(FXCOLOR(SliderHighlight)).withAlpha(0.1f));
            g.fillRoundedRectangle(juce::Rectangle<float>((float)(x + (width - SLIDER_THUMB_RADIUS * 4) / 2),
                                                           (float)y,
                                                           (float)(SLIDER_THUMB_RADIUS * 4),
                                                           (float)height)
                                       .expanded(0.0f, (float)SLIDER_THUMB_RADIUS),
                                   20.0f);
        }
    }
    else if (style == juce::Slider::LinearHorizontal)
    {
        auto radius = getSliderThumbRadius(slider);

        juce::Colour trackColour = juce::Colour(FXCOLOR(SliderTrack)).withAlpha(0.2f);
        if (!slider.isEnabled()) trackColour = trackColour.withSaturation(0.0f);
        g.setFillType(trackColour);
        g.fillRoundedRectangle((float)x, (float)(y + (height - 3) / 2), (float)width, 3.0f, 5.6f);

        juce::Colour fillColour = juce::Colour(FXCOLOR(SliderTrack)).withAlpha(1.0f);
        if (!slider.isEnabled()) fillColour = fillColour.withSaturation(0.0f);
        g.setFillType(fillColour);
        g.fillRoundedRectangle((float)x, (float)(y + (height - 3) / 2), sliderPos, 3.0f, 5.6f);

        if (slider.isEnabled() && slider_thumb_)
            slider_thumb_->drawWithin(g, juce::Rectangle<float>(sliderPos - (float)radius,
                                                                  (float)(y + height / 2 - radius),
                                                                  (float)(radius * 2), (float)(radius * 2)),
                                      { juce::RectanglePlacement::centred }, 1.0f);
        else if (!slider.isEnabled() && slider_thumb_grey_)
            slider_thumb_grey_->drawWithin(g, juce::Rectangle<float>(sliderPos - (float)radius,
                                                                       (float)(y + height / 2 - radius),
                                                                       (float)(radius * 2), (float)(radius * 2)),
                                           { juce::RectanglePlacement::centred }, 1.0f);

        if (slider.hasKeyboardFocus(true))
        {
            g.setFillType(juce::Colour(FXCOLOR(SliderHighlight)).withAlpha(0.1f));
            g.fillRoundedRectangle(juce::Rectangle<float>((float)x, (float)y, (float)width, (float)height)
                                       .expanded((float)SLIDER_THUMB_RADIUS / 2.0f, (float)SLIDER_THUMB_RADIUS / 2.0f),
                                   (float)(height + SLIDER_THUMB_RADIUS));
        }
    }
    else
    {
        LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, 0.0f, 0.0f, style, slider);
    }
}

void FxTheme::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                juce::Slider& slider)
{
    auto outline = slider.findColour(juce::Slider::rotarySliderOutlineColourId);
    auto fill    = slider.findColour(juce::Slider::rotarySliderFillColourId);

    if (!slider.isEnabled())
    {
        outline = outline.withSaturation(0.0f);
        fill    = fill.withSaturation(0.0f);
    }

    auto bounds  = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(2.0f);
    auto radius  = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW   = 5.0f;
    auto arcRadius = radius - lineW * 0.5f;

    juce::Path backgroundArc;
    backgroundArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(),
                                 arcRadius, arcRadius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(outline);
    g.strokePath(backgroundArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    juce::Path valueArc;
    valueArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(),
                            arcRadius, arcRadius, 0.0f, rotaryStartAngle, toAngle, true);
    g.setColour(fill);
    g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    auto thumbRadius = (float)getSliderThumbRadius(slider);
    juce::Point<float> thumbPoint(
        bounds.getCentreX() + arcRadius * std::cos(toAngle - juce::MathConstants<float>::halfPi),
        bounds.getCentreY() + arcRadius * std::sin(toAngle - juce::MathConstants<float>::halfPi));

    if (slider.isEnabled() && slider_thumb_)
        slider_thumb_->drawWithin(g, juce::Rectangle<float>(thumbPoint.getX() - thumbRadius,
                                                              thumbPoint.getY() - thumbRadius,
                                                              thumbRadius * 2.0f, thumbRadius * 2.0f),
                                  { juce::RectanglePlacement::centred }, 1.0f);
    else if (!slider.isEnabled() && slider_thumb_grey_)
        slider_thumb_grey_->drawWithin(g, juce::Rectangle<float>(thumbPoint.getX() - thumbRadius,
                                                                   thumbPoint.getY() - thumbRadius,
                                                                   thumbRadius * 2.0f, thumbRadius * 2.0f),
                                       { juce::RectanglePlacement::centred }, 1.0f);
}

int FxTheme::getSliderThumbRadius(juce::Slider& slider)
{
    return (slider.getSliderStyle() == juce::Slider::Rotary) ? ROTARY_SLIDER_THUMB_RADIUS : SLIDER_THUMB_RADIUS;
}

juce::Slider::SliderLayout FxTheme::getSliderLayout(juce::Slider& slider)
{
    auto layout = LookAndFeel_V4::getSliderLayout(slider);
    auto style  = slider.getSliderStyle();

    if (style == juce::Slider::LinearVertical)
    {
        layout.sliderBounds.setY(layout.sliderBounds.getY() + SLIDER_THUMB_RADIUS * 2);
        layout.sliderBounds.setHeight(layout.sliderBounds.getHeight() - SLIDER_THUMB_RADIUS * 2);
    }
    else if (style == juce::Slider::LinearHorizontal)
    {
        layout.sliderBounds.setWidth(layout.sliderBounds.getWidth() - SLIDER_THUMB_RADIUS * 4);
    }
    return layout;
}

void FxTheme::drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                                 bool is_separator, bool is_active, bool is_highlighted,
                                 bool is_ticked, bool has_submenu, const juce::String& text,
                                 const juce::String& shortcut_key_text, const juce::Drawable* icon,
                                 const juce::Colour* text_colour)
{
    LookAndFeel_V4::drawPopupMenuItem(g, area, is_separator, is_active,
                                      is_highlighted || is_ticked, is_ticked,
                                      has_submenu, text, shortcut_key_text, icon, text_colour);
    if (is_ticked)
    {
        g.setColour(findColour(juce::PopupMenu::textColourId).withAlpha(1.0f));
        g.drawRect(area.toFloat());
    }
}

juce::Font FxTheme::getPopupMenuFont()
{
    return juce::Font(font_600_).withHeight(17.0f);
}

void FxTheme::preparePopupMenuWindow(juce::Component& new_window)
{
    new_window.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    for (auto* child : new_window.getChildren())
        child->setMouseCursor(juce::MouseCursor::PointingHandCursor);
}

juce::Font FxTheme::getTextButtonFont(juce::TextButton&, int button_height)
{
    return juce::Font(font_600_).withHeight(juce::jmin(17.0f, (float)button_height));
}

juce::Font FxTheme::getLabelFont(juce::Label&)
{
    return getNormalFont();
}

juce::Font FxTheme::getNormalFont()
{
    return juce::Font(font_600_).withHeight(17.0f);
}

juce::Font FxTheme::getSmallFont()
{
    return juce::Font(font_400_).withHeight(14.0f);
}

juce::Font FxTheme::getTitleFont()
{
    return juce::Font(font_700_).withHeight(17.0f);
}

juce::uint32 FxTheme::getColor(FxColor color)
{
    return theme_colors_[color];
}

juce::Rectangle<int> FxTheme::getTooltipBounds(const juce::String& tipText,
                                                 juce::Point<int> screenPos,
                                                 juce::Rectangle<int> parentArea)
{
    const auto tl = layoutTooltipText(tipText, juce::Colours::black);
    auto w = (int)(tl.getWidth() + 20.0f);
    auto h = (int)(tl.getHeight() + 12.0f);
    return juce::Rectangle<int>(
        screenPos.x > parentArea.getCentreX() ? screenPos.x - (w + 18) : screenPos.x + 36,
        screenPos.y > parentArea.getCentreY() ? screenPos.y - (h + 12) : screenPos.y + 12,
        w, h).constrainedWithin(parentArea);
}

void FxTheme::drawTooltip(juce::Graphics& g, const juce::String& text, int width, int height)
{
    juce::Rectangle<int> bounds(width, height);
    g.setColour(findColour(juce::TooltipWindow::backgroundColourId));
    g.fillRoundedRectangle(bounds.toFloat(), 5.0f);
    g.setColour(findColour(juce::TooltipWindow::outlineColourId));
    g.drawRoundedRectangle(bounds.toFloat().reduced(0.5f, 0.5f), 5.0f, 1.0f);
    layoutTooltipText(text, findColour(juce::TooltipWindow::textColourId))
        .draw(g, bounds.toFloat().reduced(10.0f, 0.0f));
}

juce::TextLayout FxTheme::layoutTooltipText(const juce::String& text, juce::Colour colour) noexcept
{
    juce::AttributedString s;
    s.setWordWrap(juce::AttributedString::byWord);
    s.setJustification(juce::Justification::centredLeft);
    s.append(text, getNormalFont().withHeight(14.0f), colour);
    juce::TextLayout tl;
    tl.createLayout(s, 400.0f);
    return tl;
}
