#pragma once
#include <juce_core/juce_core.h>

namespace DeviceValidation
{
    // Returns the first device name containing "BlackHole" (case-insensitive), or empty.
    juce::String findBlackHoleDevice(const juce::StringArray& deviceNames);
    bool sampleRatesMatch(double a, double b, double tolerance = 0.5);
    bool isSupportedChannelLayout(int numChannels, int required = 2);
}
