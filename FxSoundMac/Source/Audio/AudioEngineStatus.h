#pragma once
#include <juce_core/juce_core.h>

enum class AudioEngineState
{
    BlackHoleNotInstalled,
    BlackHoleNotSelected,
    NoOutputSelected,
    SampleRateMismatch,
    UnsupportedChannelLayout,
    EngineFailedToStart,
    DspFailedToInitialize,
    PresetFailedToLoad,
    Running
};

struct AudioEngineStatus
{
    AudioEngineState state = AudioEngineState::BlackHoleNotInstalled;
    juce::String detail;

    juce::String userMessage() const;
    bool isHealthy() const { return state == AudioEngineState::Running; }
};
