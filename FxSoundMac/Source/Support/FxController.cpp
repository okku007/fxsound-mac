#include "FxController.h"

void FxController::prepare(int sampleRate, int maxBlockSize)
{
    adapter.prepare(sampleRate, maxBlockSize);
    prepared = true;
}

void FxController::setPower(bool on)   { adapter.setPowerOn(on); }
bool FxController::isPowerOn() const   { return adapter.isPowerOn(); }
void FxController::setBypassed(bool b) { adapter.setBypassed(b); }

void FxController::setEffect(DfxDsp::Effect e, float v) { adapter.setEffectValue(e, v); }
float FxController::getEffect(DfxDsp::Effect e) const   { return adapter.getEffectValue(e); }

void FxController::setEqBand(int band, float db)  { adapter.setEqBandBoostCut(band, db); }
float FxController::getEqBand(int band) const     { return adapter.getEqBandBoostCut(band); }
int FxController::getNumEqBands() const           { return adapter.getNumEqBands(); }
float FxController::getEqBandFrequency(int b) const { return adapter.getEqBandFrequency(b); }

void FxController::setOutputGainDb(float db)  { adapter.setOutputGainDb(db); }
float FxController::getOutputGainDb() const   { return adapter.getOutputGainDb(); }

bool FxController::loadPreset(const juce::File& presetFile)
{
    return adapter.loadPreset(presetFile);
}

bool FxController::savePreset(const juce::File& directory, const juce::String& name)
{
    return adapter.savePreset(directory, name);
}

void FxController::processBlock(juce::AudioBuffer<float>& buffer)
{
    if (! prepared) return;
    adapter.process(buffer);
}
