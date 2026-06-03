#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "FxController.h"

class FxEqualizer : public juce::Component,
                    public juce::Slider::Listener,
                    public juce::Timer
{
public:
    explicit FxEqualizer(FxController& controller);
    ~FxEqualizer() override = default;

    void reinit(int num_bands);
    void update();
    void showValues(bool show);

    void sliderValueChanged(juce::Slider*) override { repaint(); }
    void sliderDragStarted(juce::Slider* slider) override;
    void sliderDragEnded(juce::Slider* slider) override;
    void timerCallback() override;

private:
    class FxEqSlider : public juce::Slider
    {
    public:
        FxEqSlider(int band, float max_gain, FxController& controller);
        ~FxEqSlider() override = default;

        void setGainValue(float db);
        void showValue(bool show);
        void enablementChanged() override;

    private:
        static constexpr int LABEL_HEIGHT = 12;

        void resized() override;
        void valueChanged() override;
        bool keyPressed(const juce::KeyPress& key) override;
        void mouseDown(const juce::MouseEvent& e) override;

        juce::Label gain_label_;
        int band_;
        FxController& controller_;
    };

    static constexpr int WIDTH             = 776;
    static constexpr int HEIGHT            = 242;
    static constexpr int SLIDER_HEIGHT     = 190;
    static constexpr int LABEL_HEIGHT      = 12;
    static constexpr int X_MARGIN         = 16;
    static constexpr int Y_MARGIN         = 8;
    static constexpr float MAX_GAIN       = 12.0f;
    static constexpr int MAX_DISPLAY_BANDS = 10; // cap display to match Windows default

    void resized() override;
    void paint(juce::Graphics& g) override;
    void enablementChanged() override;

    FxController& controller_;
    std::vector<std::unique_ptr<juce::Label>> labels_;
    std::vector<std::unique_ptr<FxEqSlider>> band_boosts_;
    std::vector<float> band_gain_values_;
    std::vector<int>   band_indices_;  // display slot i → actual DSP band index
    bool highlight_mode_ { false };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FxEqualizer)
};
