#include <juce_core/juce_core.h>
#include "DfxDsp.h"

struct DspProbeTest : juce::UnitTest
{
    DspProbeTest() : juce::UnitTest("DspProbe") {}
    void runTest() override
    {
        beginTest("constructs and reports effect count");
        DfxDsp dsp;
        expectEquals((int)DfxDsp::NumEffects, 5);

        beginTest("power on/off round-trips");
        dsp.powerOn(true);
        expect(dsp.isPowerOn());
        dsp.powerOn(false);
        expect(!dsp.isPowerOn());

        beginTest("setSignalFormat returns OKAY");
        expectEquals(dsp.setSignalFormat(16, 2, 48000, 16), 0);
    }
};
static DspProbeTest dspProbeTest;
