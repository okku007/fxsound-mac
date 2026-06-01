// Standalone compile+link probe for the DfxDsp macOS shim.
// Not part of the app build. Run via probe/build_probe.sh.
// Success = DfxDsp constructs and NumEffects == 5.
#include <cassert>
#include <cstdio>
#include "DfxDsp.h"

int main()
{
    DfxDsp dsp;
    assert((int)DfxDsp::NumEffects == 5);
    dsp.powerOn(true);
    assert(dsp.isPowerOn());
    dsp.setSignalFormat(16, 2, 48000, 16);
    std::printf("DSP probe PASSED. NumEffects=%d\n", (int)DfxDsp::NumEffects);
    return 0;
}
