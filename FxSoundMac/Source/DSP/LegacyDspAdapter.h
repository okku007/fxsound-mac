#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <atomic>
#include "DfxDsp.h"

// Narrow, macOS-facing wrapper around the reusable FxSound DSP.
// Converts JUCE float buffers to/from the int16 surface DfxDsp expects,
// and applies an app-level output gain stage.
class LegacyDspAdapter
{
public:
    static constexpr float minOutputGainDb = -60.0f;
    static constexpr float maxOutputGainDb = 12.0f;

    LegacyDspAdapter();
    ~LegacyDspAdapter();

    // Sets signal format (stereo, int16, given sample rate). Safe to call on device change.
    bool prepare(int sampleRate, int maxBlockSize);

    void setPowerOn(bool on);
    bool isPowerOn() const;
    void setBypassed(bool bypassed);
    bool isBypassed() const;

    void setMasterGainDb(float db);
    float getMasterGainDb() const;
    void setOutputGainDb(float db);
    float getOutputGainDb() const;
    float linearOutputGain() const;

    void setEffectValue(DfxDsp::Effect effect, float value); // clamped [0,1]
    float getEffectValue(DfxDsp::Effect effect) const;

    int getNumEqBands() const;
    void setEqBandBoostCut(int band, float db);
    float getEqBandBoostCut(int band) const;

    // Loads a .fac preset by absolute path. Returns true on success.
    bool loadPreset(const juce::File& presetFile);

    // Processes a stereo float buffer in place. Honors bypass.
    void process(juce::AudioBuffer<float>& buffer);

private:
    DfxDsp dsp;
    int currentSampleRate = 48000;
    std::atomic<bool> bypassed { false };
    std::atomic<float> outputGainDb { 0.0f };

    juce::HeapBlock<short> inInt16, outInt16;
    int allocatedFrames = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LegacyDspAdapter)
};
