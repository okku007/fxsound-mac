#pragma once
#include <juce_audio_devices/juce_audio_devices.h>
#include <functional>
#include "AudioEngineStatus.h"
#include "../Support/FxController.h"

class MacAudioEngine : private juce::AudioIODeviceCallback
{
public:
    explicit MacAudioEngine(FxController& controllerToUse);
    ~MacAudioEngine() override;

    juce::StringArray getInputDeviceNames() const;
    juce::StringArray getOutputDeviceNames() const;

    // Detects BlackHole, opens the route, validates it, reports status.
    AudioEngineStatus start(const juce::String& outputDeviceName);
    void stop();
    bool isRunning() const { return running; }

    AudioEngineStatus getStatus() const { return status; }

    // Called on the message thread when status changes.
    std::function<void(AudioEngineStatus)> onStatusChanged;

private:
    // juce::AudioIODeviceCallback
    void audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
                                          int numInputChannels,
                                          float* const* outputChannelData,
                                          int numOutputChannels,
                                          int numSamples,
                                          const juce::AudioIODeviceCallbackContext&) override;
    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;

    void setStatus(AudioEngineStatus newStatus);

    juce::AudioDeviceManager deviceManager;
    FxController& controller;
    juce::AudioBuffer<float> scratch;
    AudioEngineStatus status;
    bool running = false;
};
