#include <juce_core/juce_core.h>

struct SmokeTest : juce::UnitTest
{
    SmokeTest() : juce::UnitTest("Smoke") {}
    void runTest() override
    {
        beginTest("arithmetic");
        expectEquals(2 + 2, 4);
    }
};

static SmokeTest smokeTest;
