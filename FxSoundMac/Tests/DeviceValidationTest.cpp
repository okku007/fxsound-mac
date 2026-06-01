#include <juce_core/juce_core.h>
#include "DeviceValidation.h"
#include "AudioEngineStatus.h"

struct DeviceValidationTest : juce::UnitTest
{
    DeviceValidationTest() : juce::UnitTest("DeviceValidation") {}
    void runTest() override
    {
        beginTest("detects BlackHole 2ch in an input device list");
        juce::StringArray inputs { "MacBook Pro Microphone", "BlackHole 2ch" };
        expect(DeviceValidation::findBlackHoleDevice(inputs) == "BlackHole 2ch");

        beginTest("returns empty when BlackHole absent");
        juce::StringArray noBh { "MacBook Pro Microphone" };
        expect(DeviceValidation::findBlackHoleDevice(noBh).isEmpty());

        beginTest("sample-rate match check");
        expect(DeviceValidation::sampleRatesMatch(48000.0, 48000.0));
        expect(! DeviceValidation::sampleRatesMatch(48000.0, 44100.0));

        beginTest("stereo layout is supported, mono is not");
        expect(DeviceValidation::isSupportedChannelLayout(2, 2));
        expect(! DeviceValidation::isSupportedChannelLayout(1, 2));

        beginTest("status messages are non-empty for every state");
        for (auto s : { AudioEngineState::BlackHoleNotInstalled,
                        AudioEngineState::BlackHoleNotSelected,
                        AudioEngineState::NoOutputSelected,
                        AudioEngineState::SampleRateMismatch,
                        AudioEngineState::UnsupportedChannelLayout,
                        AudioEngineState::EngineFailedToStart,
                        AudioEngineState::DspFailedToInitialize,
                        AudioEngineState::PresetFailedToLoad,
                        AudioEngineState::Running })
        {
            AudioEngineStatus st { s, {} };
            expect(st.userMessage().isNotEmpty());
        }
    }
};
static DeviceValidationTest deviceValidationTest;
