#include "LegacyDspAdapter.h"

LegacyDspAdapter::LegacyDspAdapter() = default;
LegacyDspAdapter::~LegacyDspAdapter() = default;

bool LegacyDspAdapter::prepare(int sampleRate, int maxBlockSize)
{
    currentSampleRate = sampleRate;
    allocatedFrames = juce::jmax(maxBlockSize, 1);
    inInt16.calloc((size_t) allocatedFrames * 2);
    outInt16.calloc((size_t) allocatedFrames * 2);
    // 16 bits per sample, 2 channels, given srate, 16 valid bits.
    return dsp.setSignalFormat(16, 2, sampleRate, 16) == 0; // OKAY == 0
}

void LegacyDspAdapter::setPowerOn(bool on) { dsp.powerOn(on); }
bool LegacyDspAdapter::isPowerOn() const { return const_cast<DfxDsp&>(dsp).isPowerOn(); }
void LegacyDspAdapter::setBypassed(bool b) { bypassed.store(b); }
bool LegacyDspAdapter::isBypassed() const { return bypassed.load(); }

void LegacyDspAdapter::setMasterGainDb(float db) { dsp.setMasterGain(db); }
float LegacyDspAdapter::getMasterGainDb() const { return const_cast<DfxDsp&>(dsp).getMasterGain(); }

void LegacyDspAdapter::setOutputGainDb(float db)
{
    outputGainDb.store(juce::jlimit(minOutputGainDb, maxOutputGainDb, db));
}
float LegacyDspAdapter::getOutputGainDb() const { return outputGainDb.load(); }
float LegacyDspAdapter::linearOutputGain() const
{
    return juce::Decibels::decibelsToGain(outputGainDb.load(), minOutputGainDb);
}

void LegacyDspAdapter::setEffectValue(DfxDsp::Effect e, float v)
{
    dsp.setEffectValue(e, juce::jlimit(0.0f, 1.0f, v));
}
float LegacyDspAdapter::getEffectValue(DfxDsp::Effect e) const
{
    return const_cast<DfxDsp&>(dsp).getEffectValue(e);
}

int LegacyDspAdapter::getNumEqBands() const { return const_cast<DfxDsp&>(dsp).getNumEqBands(); }
void LegacyDspAdapter::setEqBandBoostCut(int band, float db) { dsp.setEqBandBoostCut(band, db); }
float LegacyDspAdapter::getEqBandBoostCut(int band) const { return const_cast<DfxDsp&>(dsp).getEqBandBoostCut(band); }

bool LegacyDspAdapter::loadPreset(const juce::File& presetFile)
{
    if (! presetFile.existsAsFile())
        return false;
    std::wstring path = presetFile.getFullPathName().toWideCharPointer();
    return dsp.loadPreset(path) == 0; // OKAY == 0
}

void LegacyDspAdapter::process(juce::AudioBuffer<float>& buffer)
{
    const float gain = linearOutputGain();
    const int numFrames = buffer.getNumSamples();

    if (bypassed.load())
    {
        if (gain != 1.0f) buffer.applyGain(gain);
        return;
    }

    if (numFrames > allocatedFrames)
    {
        // This path should not be reached on the audio thread — JUCE stops the
        // audio device before calling prepare() with a new block size, so
        // allocatedFrames is always >= numFrames during normal operation.
        // The grow-on-demand call here is a safety net for unexpected oversized
        // blocks; it is not safe to call concurrently with a device-change callback.
        jassert(false && "process() called with numFrames > allocatedFrames — "
                         "ensure prepare() is called before the audio thread starts.");
        prepare(currentSampleRate, numFrames);
    }

    const int chCount = juce::jmin(buffer.getNumChannels(), 2);
    const float* left  = buffer.getReadPointer(0);
    const float* right = chCount > 1 ? buffer.getReadPointer(1) : left;

    // Interleave + float->int16.
    for (int i = 0; i < numFrames; ++i)
    {
        inInt16[(size_t) i * 2]     = (short) juce::jlimit(-32768, 32767, (int) std::lround(left[i]  * 32767.0f));
        inInt16[(size_t) i * 2 + 1] = (short) juce::jlimit(-32768, 32767, (int) std::lround(right[i] * 32767.0f));
    }

    dsp.processAudio(inInt16.getData(), outInt16.getData(), numFrames, 0);

    // De-interleave + int16->float + output gain.
    float* outL = buffer.getWritePointer(0);
    float* outR = chCount > 1 ? buffer.getWritePointer(1) : nullptr;
    for (int i = 0; i < numFrames; ++i)
    {
        const float l = (float) outInt16[(size_t) i * 2]     / 32767.0f * gain;
        const float r = (float) outInt16[(size_t) i * 2 + 1] / 32767.0f * gain;
        outL[i] = l;
        if (outR != nullptr) outR[i] = r;
    }
}
