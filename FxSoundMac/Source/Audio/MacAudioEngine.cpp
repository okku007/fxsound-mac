#include "MacAudioEngine.h"
#include "DeviceValidation.h"

MacAudioEngine::MacAudioEngine(FxController& c) : controller(c) {}
MacAudioEngine::~MacAudioEngine() { stop(); }

static juce::StringArray deviceNamesForType(juce::AudioDeviceManager& dm, bool inputs)
{
    juce::StringArray names;
    for (auto* type : dm.getAvailableDeviceTypes())
    {
        type->scanForDevices();
        names.addArray(type->getDeviceNames(inputs));
    }
    names.removeDuplicates(false);
    return names;
}

juce::StringArray MacAudioEngine::getInputDeviceNames() const
{
    return deviceNamesForType(const_cast<juce::AudioDeviceManager&>(deviceManager), true);
}

juce::StringArray MacAudioEngine::getOutputDeviceNames() const
{
    return deviceNamesForType(const_cast<juce::AudioDeviceManager&>(deviceManager), false);
}

AudioEngineStatus MacAudioEngine::start(const juce::String& outputDeviceName)
{
    auto blackHole = DeviceValidation::findBlackHoleDevice(getInputDeviceNames());
    if (blackHole.isEmpty())
    {
        setStatus({ AudioEngineState::BlackHoleNotInstalled, {} });
        return status;
    }
    if (outputDeviceName.isEmpty())
    {
        setStatus({ AudioEngineState::NoOutputSelected, {} });
        return status;
    }

    juce::AudioDeviceManager::AudioDeviceSetup setup;
    setup.inputDeviceName  = blackHole;
    setup.outputDeviceName = outputDeviceName;
    setup.useDefaultInputChannels  = true;
    setup.useDefaultOutputChannels = true;

    auto err = deviceManager.initialise(2, 2, nullptr, true, {}, &setup);
    if (err.isNotEmpty())
    {
        setStatus({ AudioEngineState::EngineFailedToStart, err });
        return status;
    }

    if (auto* dev = deviceManager.getCurrentAudioDevice())
    {
        const int inCh  = dev->getActiveInputChannels().countNumberOfSetBits();
        const int outCh = dev->getActiveOutputChannels().countNumberOfSetBits();
        if (! DeviceValidation::isSupportedChannelLayout(juce::jmin(inCh, outCh)))
        {
            setStatus({ AudioEngineState::UnsupportedChannelLayout,
                        "in=" + juce::String(inCh) + " out=" + juce::String(outCh) });
            deviceManager.closeAudioDevice();
            return status;
        }
        controller.prepare((int) dev->getCurrentSampleRate(),
                           dev->getCurrentBufferSizeSamples());
    }

    deviceManager.addAudioCallback(this);
    running = true;
    setStatus({ AudioEngineState::Running, {} });
    return status;
}

void MacAudioEngine::stop()
{
    if (! running) return;
    deviceManager.removeAudioCallback(this);
    deviceManager.closeAudioDevice();
    running = false;
}

void MacAudioEngine::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    controller.prepare((int) device->getCurrentSampleRate(),
                       device->getCurrentBufferSizeSamples());
}

void MacAudioEngine::audioDeviceStopped() {}

void MacAudioEngine::audioDeviceIOCallbackWithContext(
    const float* const* inputChannelData,
    int numInputChannels,
    float* const* outputChannelData,
    int numOutputChannels,
    int numSamples,
    const juce::AudioIODeviceCallbackContext&)
{
    const int chans = juce::jmax(numInputChannels, numOutputChannels, 2);
    if (scratch.getNumChannels() < chans || scratch.getNumSamples() < numSamples)
        scratch.setSize(chans, numSamples, false, false, true);

    // Copy input into scratch (duplicate mono to stereo if needed).
    for (int ch = 0; ch < 2; ++ch)
    {
        const float* src = (ch < numInputChannels && inputChannelData[ch] != nullptr)
                              ? inputChannelData[ch]
                              : (numInputChannels > 0 ? inputChannelData[0] : nullptr);
        if (src != nullptr) scratch.copyFrom(ch, 0, src, numSamples);
        else                scratch.clear(ch, 0, numSamples);
    }

    juce::AudioBuffer<float> view(scratch.getArrayOfWritePointers(), 2, numSamples);
    controller.processBlock(view);

    // Copy processed output to output channels.
    for (int ch = 0; ch < numOutputChannels; ++ch)
    {
        if (outputChannelData[ch] == nullptr) continue;
        const int srcCh = juce::jmin(ch, 1);
        juce::FloatVectorOperations::copy(outputChannelData[ch],
                                          scratch.getReadPointer(srcCh),
                                          numSamples);
    }
}

void MacAudioEngine::setStatus(AudioEngineStatus newStatus)
{
    status = newStatus;
    if (onStatusChanged)
        onStatusChanged(status);
}
