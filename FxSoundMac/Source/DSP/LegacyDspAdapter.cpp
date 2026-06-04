#include "LegacyDspAdapter.h"

LegacyDspAdapter::LegacyDspAdapter() = default;
LegacyDspAdapter::~LegacyDspAdapter() = default;

bool LegacyDspAdapter::prepare(int sampleRate, int maxBlockSize)
{
    currentSampleRate = sampleRate;
    allocatedFrames = juce::jmax(maxBlockSize, 1);
    inFloat.calloc((size_t) allocatedFrames * 2);
    outFloat.calloc((size_t) allocatedFrames * 2);

    // The DFX DSP build (PT_DSP_BUILD=PT_DSP_DFX, DSPSOFT_32_BIT) processes audio
    // natively as 32-bit float (COM_32_BIT_FLOAT_SAMPLES). Feeding int16 forced the
    // DSP to do float→int16→float internally, and the adapter added another int16
    // round trip — two quantization stages whose noise the effects (esp. the Aural
    // exciter) amplify into a gritty "cheap speaker" sound. Use 32-bit float so audio
    // stays float end to end, matching the Windows signal path. No quantization.
    const bool ok = dsp.setSignalFormat(32, 2, sampleRate, 32) == 0;

    // setSignalFormat()/dfxpBeginProcess() re-initialises the DSP from defaults
    // (filter coefficients from non-zero MIDI defaults; effect knobs reset). Since
    // (re)starting the audio device re-runs prepare(), we must re-apply the user's
    // desired control state here — otherwise effects/EQ silently revert until a
    // slider is moved. On first launch the caches are all zero, which also matches
    // the UI (sliders start at 0) and avoids the "processed even at zero" mismatch.
    if (ok)
        reapplyState();

    return ok;
}

void LegacyDspAdapter::reapplyState()
{
    dsp.powerOn(powerCache_);

    for (int e = 0; e < static_cast<int>(DfxDsp::NumEffects); ++e)
        dsp.setEffectValue(static_cast<DfxDsp::Effect>(e), effectCache_[e]);

    const int numBands = dsp.getNumEqBands();
    if (static_cast<int>(eqCache_.size()) < numBands)
        eqCache_.resize(static_cast<size_t>(numBands), 0.0f);
    for (int b = 0; b < numBands; ++b)
        dsp.setEqBandBoostCut(b, eqCache_[b]);
}

void LegacyDspAdapter::setPowerOn(bool on) { powerCache_ = on; dsp.powerOn(on); }
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
    const float clamped = juce::jlimit(0.0f, 1.0f, v);
    const int idx = static_cast<int>(e);
    if (idx >= 0 && idx < static_cast<int>(DfxDsp::NumEffects))
        effectCache_[idx] = clamped;
    dsp.setEffectValue(e, clamped);
}
float LegacyDspAdapter::getEffectValue(DfxDsp::Effect e) const
{
    return const_cast<DfxDsp&>(dsp).getEffectValue(e);
}

int LegacyDspAdapter::getNumEqBands() const { return const_cast<DfxDsp&>(dsp).getNumEqBands(); }
float LegacyDspAdapter::getEqBandFrequency(int band) const { return const_cast<DfxDsp&>(dsp).getEqBandFrequency(band); }
void LegacyDspAdapter::setEqBandBoostCut(int band, float db)
{
    if (band >= 0)
    {
        if (band >= static_cast<int>(eqCache_.size()))
            eqCache_.resize(static_cast<size_t>(band) + 1, 0.0f);
        eqCache_[static_cast<size_t>(band)] = db;
    }
    dsp.setEqBandBoostCut(band, db);
}
float LegacyDspAdapter::getEqBandBoostCut(int band) const { return const_cast<DfxDsp&>(dsp).getEqBandBoostCut(band); }

bool LegacyDspAdapter::loadPreset(const juce::File& presetFile)
{
    if (! presetFile.existsAsFile())
        return false;
    std::wstring path = presetFile.getFullPathName().toWideCharPointer();
    const bool ok = dsp.loadPreset(path) == 0; // OKAY == 0

    // Sync caches from the DSP so a later prepare()/restart re-applies the preset
    // (not the pre-preset values). loadPreset writes the new state into the DSP;
    // read it back here as the new "desired" state.
    if (ok)
    {
        for (int e = 0; e < static_cast<int>(DfxDsp::NumEffects); ++e)
            effectCache_[e] = dsp.getEffectValue(static_cast<DfxDsp::Effect>(e));

        const int numBands = dsp.getNumEqBands();
        eqCache_.assign(static_cast<size_t>(numBands), 0.0f);
        for (int b = 0; b < numBands; ++b)
            eqCache_[static_cast<size_t>(b)] = dsp.getEqBandBoostCut(b);
    }
    return ok;
}

bool LegacyDspAdapter::savePreset(const juce::File& directory, const juce::String& name)
{
    // DfxDsp::savePreset takes (preset_name, directory_path); it appends ".fac"
    // to the name and writes the current DSP state (effects + EQ) into directory.
    std::wstring dir   = directory.getFullPathName().toWideCharPointer();
    std::wstring wname = name.toWideCharPointer();
    return dsp.savePreset(wname, dir) == 0; // OKAY == 0
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

    // Interleave L/R into the 32-bit float DSP buffer. The DSP expects normalised
    // float in [-1, 1] (COM_32_BIT_FLOAT_SAMPLES) — no scaling needed. The Maximizer
    // (always active) limits peaks internally, and dfxpModifyRealtypeSamples clamps to
    // legal range, so full-scale input is safe (float has no int16-style wraparound).
    for (int i = 0; i < numFrames; ++i)
    {
        inFloat[(size_t) i * 2]     = left[i];
        inFloat[(size_t) i * 2 + 1] = right[i];
    }

    // processAudio takes short int* by signature, but dfxpUniversalModifySamples
    // reinterprets the buffer by byte according to the signal format (32-bit float
    // here), so passing float data cast to short int* is correct.
    const int dspResult = dsp.processAudio(reinterpret_cast<short*>(inFloat.getData()),
                                           reinterpret_cast<short*>(outFloat.getData()),
                                           numFrames, 0);
    (void)dspResult;

    // De-interleave + apply user output gain + final safety limit.
    float* outL = buffer.getWritePointer(0);
    float* outR = chCount > 1 ? buffer.getWritePointer(1) : nullptr;
    for (int i = 0; i < numFrames; ++i)
    {
        outL[i] = juce::jlimit(-1.0f, 1.0f, outFloat[(size_t) i * 2]     * gain);
        if (outR != nullptr) outR[i] = juce::jlimit(-1.0f, 1.0f, outFloat[(size_t) i * 2 + 1] * gain);
    }
}
