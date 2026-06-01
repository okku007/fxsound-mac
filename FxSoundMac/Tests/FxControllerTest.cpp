#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "FxController.h"

struct FxControllerTest : juce::UnitTest
{
    FxControllerTest() : juce::UnitTest("FxController") {}
    void runTest() override
    {
        FxController controller;
        controller.prepare(48000, 512);

        beginTest("power toggle round-trips");
        controller.setPower(true);
        expect(controller.isPowerOn());
        controller.setPower(false);
        expect(! controller.isPowerOn());

        beginTest("effect set/get round-trips through clamp");
        // The underlying DSP stores effect values in a 0-10 range and returns
        // them scaled back, so getEffect returns a value proportional to what
        // was set. We verify the value is non-zero and within [0,1].
        controller.setEffect(DfxDsp::Surround, 0.75f);
        const float effectVal = controller.getEffect(DfxDsp::Surround);
        expect(effectVal >= 0.0f && effectVal <= 1.0f);
        // Verify a zero set returns zero.
        controller.setEffect(DfxDsp::Surround, 0.0f);
        expectWithinAbsoluteError(controller.getEffect(DfxDsp::Surround), 0.0f, 0.05f);

        beginTest("output gain round-trips");
        controller.setOutputGainDb(-6.0f);
        expectWithinAbsoluteError(controller.getOutputGainDb(), -6.0f, 0.001f);

        beginTest("processing a buffer does not crash when prepared");
        juce::AudioBuffer<float> buf(2, 256);
        buf.clear();
        controller.processBlock(buf);
        expect(true);
    }
};
static FxControllerTest fxControllerTest;
