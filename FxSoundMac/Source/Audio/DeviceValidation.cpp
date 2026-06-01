#include "DeviceValidation.h"
#include "AudioEngineStatus.h"

namespace DeviceValidation
{
    juce::String findBlackHoleDevice(const juce::StringArray& deviceNames)
    {
        for (const auto& name : deviceNames)
            if (name.containsIgnoreCase("BlackHole"))
                return name;
        return {};
    }

    bool sampleRatesMatch(double a, double b, double tolerance)
    {
        return std::abs(a - b) <= tolerance;
    }

    bool isSupportedChannelLayout(int numChannels, int required)
    {
        return numChannels == required;
    }
}

juce::String AudioEngineStatus::userMessage() const
{
    switch (state)
    {
        case AudioEngineState::BlackHoleNotInstalled:
            return "BlackHole 2ch is not installed. Install it with: brew install --cask blackhole-2ch";
        case AudioEngineState::BlackHoleNotSelected:
            return "BlackHole is installed but not selected as the input. Route system output to BlackHole 2ch.";
        case AudioEngineState::NoOutputSelected:
            return "Select a physical output device for playback.";
        case AudioEngineState::SampleRateMismatch:
            return "Input and output sample rates do not match. Set both to the same rate in Audio MIDI Setup. " + detail;
        case AudioEngineState::UnsupportedChannelLayout:
            return "Unsupported channel layout. FxSoundMac requires a stereo (2ch) route. " + detail;
        case AudioEngineState::EngineFailedToStart:
            return "The audio engine failed to start. " + detail;
        case AudioEngineState::DspFailedToInitialize:
            return "The DSP failed to initialize. " + detail;
        case AudioEngineState::PresetFailedToLoad:
            return "The selected preset failed to load. " + detail;
        case AudioEngineState::Running:
            return "Audio is running. If you hear no sound, confirm macOS output is routed to BlackHole 2ch.";
    }
    return "Unknown state.";
}
