#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <atomic>
#include "LegacyDspAdapter.h"

// Single owner of DSP-facing state.
// UI calls setters on the message thread;
// processBlock runs on the audio thread and only touches the adapter.
// No locks or file I/O in the audio path.
class FxController
{
public:
    void prepare(int sampleRate, int maxBlockSize);

    void setPower(bool on);
    bool isPowerOn() const;
    void setBypassed(bool b);

    void setEffect(DfxDsp::Effect e, float value);
    float getEffect(DfxDsp::Effect e) const;

    void setEqBand(int band, float db);
    float getEqBand(int band) const;
    int getNumEqBands() const;
    float getEqBandFrequency(int band) const;

    void setOutputGainDb(float db);
    float getOutputGainDb() const;

    bool loadPreset(const juce::File& presetFile); // message thread only

    // Audio thread — delegates directly to adapter, no locks.
    void processBlock(juce::AudioBuffer<float>& buffer);

private:
    LegacyDspAdapter adapter;
    // Atomic so the audio thread sees the write from prepare() without a data race.
    std::atomic<bool> prepared { false };
};
