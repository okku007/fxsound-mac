#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "FxController.h"

class FxEffects : public juce::Component
{
public:
    explicit FxEffects(FxController& controller);
    ~FxEffects() override = default;

    void update();
    void showValues(bool show);

private:
    class FxEffectSlider : public juce::Slider
    {
    public:
        FxEffectSlider(int effect, FxController& controller);
        ~FxEffectSlider() override = default;

        void setEffectValue(float displayValue);
        void showValue(bool show);
        void enablementChanged() override;

    private:
        static constexpr int LABEL_HEIGHT = 14;  // fits the 12px value font without clipping

        void resized() override;
        void valueChanged() override;
        bool keyPressed(const juce::KeyPress& key) override;

        juce::Label value_label_;
        int effect_;
        FxController& controller_;
    };

    static constexpr int WIDTH       = 168;
    static constexpr int HEIGHT      = 242;
    static constexpr int LABEL_HEIGHT = 20;  // fit 17px Gilroy incl. 'y'/'g' descenders (was 14 → clipped)
    static constexpr int LABEL_GAP    = 4;   // breathing room between label and its slider
    static constexpr int ROW_GAP      = 8;   // gap below each slider before the next effect
    static constexpr int SLIDER_WIDTH = 160;
    static constexpr int SLIDER_HEIGHT = 16; // thumb diameter is 16; keeps rows compact for more row spacing
    static constexpr int X_MARGIN    = 8;
    static constexpr int Y_MARGIN    = 8;

    void resized() override;
    void paint(juce::Graphics& g) override;
    void enablementChanged() override;

    FxController& controller_;
    std::vector<std::unique_ptr<juce::Label>> labels_;
    std::vector<std::unique_ptr<FxEffectSlider>> effects_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FxEffects)
};
