#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "LegacyDspAdapter.h"

struct LegacyDspAdapterTest : juce::UnitTest
{
    LegacyDspAdapterTest() : juce::UnitTest("LegacyDspAdapter") {}
    void runTest() override
    {
        LegacyDspAdapter adapter;
        adapter.prepare(48000, 512);

        beginTest("effect value is clamped to [0,1]");
        adapter.setEffectValue(DfxDsp::Fidelity, 5.0f);
        expect(adapter.getEffectValue(DfxDsp::Fidelity) <= 1.0f);
        adapter.setEffectValue(DfxDsp::Fidelity, -5.0f);
        expect(adapter.getEffectValue(DfxDsp::Fidelity) >= 0.0f);

        beginTest("output gain in dB clamps to supported range");
        adapter.setOutputGainDb(999.0f);
        expect(adapter.getOutputGainDb() <= LegacyDspAdapter::maxOutputGainDb);
        adapter.setOutputGainDb(-999.0f);
        expect(adapter.getOutputGainDb() >= LegacyDspAdapter::minOutputGainDb);

        beginTest("bypass passes audio through unchanged");
        juce::AudioBuffer<float> buf(2, 64);
        for (int ch = 0; ch < 2; ++ch)
            for (int i = 0; i < 64; ++i)
                buf.setSample(ch, i, 0.5f);
        adapter.setBypassed(true);
        adapter.setOutputGainDb(0.0f);
        adapter.process(buf);
        for (int ch = 0; ch < 2; ++ch)
            for (int i = 0; i < 64; ++i)
                expectWithinAbsoluteError(buf.getSample(ch, i), 0.5f, 1.0e-4f);

        beginTest("unity output gain linear value is 1.0");
        adapter.setOutputGainDb(0.0f);
        expectWithinAbsoluteError(adapter.linearOutputGain(), 1.0f, 1.0e-6f);
    }
};
static LegacyDspAdapterTest legacyDspAdapterTest;
