// Standalone DSP audio-quality probe. Feeds a clean 1 kHz sine through the DSP
// in 32-bit float mode and measures distortion for each effect in isolation.
// Run via probe/build_probe.sh.
#include <cstdio>
#include <cmath>
#include <vector>
#include "DfxDsp.h"

static const int    SR   = 48000;
static const int    BLK  = 512;
static const double FREQ = 1000.0;
static const float  AMP  = 0.5f;

// Goertzel magnitude for frequency f over a real buffer (single channel).
static double goertzel(const std::vector<float>& x, double f)
{
    const double w = 2.0 * M_PI * f / SR;
    const double c = 2.0 * std::cos(w);
    double s1 = 0, s2 = 0;
    for (float v : x) { double s0 = v + c * s1 - s2; s2 = s1; s1 = s0; }
    return std::sqrt(s1 * s1 + s2 * s2 - c * s1 * s2);
}

static void runCase(DfxDsp& dsp, const char* name, int effect, float val, float amp)
{
    for (int e = 0; e < (int)DfxDsp::NumEffects; ++e)
        dsp.setEffectValue((DfxDsp::Effect)e, 0.0f);
    if (effect == -2)   // -2 == enable ALL effects at val
        for (int e = 0; e < (int)DfxDsp::NumEffects; ++e)
            dsp.setEffectValue((DfxDsp::Effect)e, val);
    else if (effect >= 0)
        dsp.setEffectValue((DfxDsp::Effect)effect, val);

    std::vector<float> inter(BLK * 2);
    std::vector<float> outL; outL.reserve(48000);

    double phase = 0.0;
    const double dphase = 2.0 * M_PI * FREQ / SR;
    int nans = 0, clipped = 0;
    double peak = 0, sumsq = 0; long n = 0;

    const int totalBlocks = 200;   // ~2.1 s
    const int warmupBlocks = 40;   // discard lookahead/settling
    for (int b = 0; b < totalBlocks; ++b)
    {
        for (int i = 0; i < BLK; ++i)
        {
            float s = amp * (float)std::sin(phase);
            phase += dphase; if (phase > 2*M_PI) phase -= 2*M_PI;
            inter[i*2] = s; inter[i*2+1] = s;
        }
        dsp.processAudio(reinterpret_cast<short*>(inter.data()),
                         reinterpret_cast<short*>(inter.data()), BLK, 0);
        if (b < warmupBlocks) continue;
        for (int i = 0; i < BLK; ++i)
        {
            float o = inter[i*2];
            if (std::isnan(o) || std::isinf(o)) { nans++; o = 0; }
            double a = std::fabs(o);
            if (a > 1.0) clipped++;          // DSP output exceeds full scale → app jlimit will hard-clip
            outL.push_back(o);
            if (a > peak) peak = a;
            sumsq += (double)o * o; n++;
        }
    }

    const double fund = goertzel(outL, FREQ);
    double harm = 0;
    for (int h = 2; h <= 8; ++h) { double m = goertzel(outL, FREQ*h); harm += m*m; }
    const double thd = fund > 1e-9 ? 100.0 * std::sqrt(harm) / fund : 0.0;

    std::printf("%-22s amp=%.2f peak=%.4f rms=%.4f THD=%6.2f%%  clip>1.0=%d nans=%d\n",
                name, amp, peak, std::sqrt(sumsq / (n?n:1)), thd, clipped, nans);
}

int main()
{
    DfxDsp dsp;
    dsp.setSignalFormat(32, 2, SR, 32);
    dsp.powerOn(true);

    std::printf("Input: %.0f Hz sine, amplitude %.2f, 32-bit float\n", FREQ, AMP);
    std::printf("(low THD = clean; high THD = distortion source)\n\n");

    std::printf("[bypass probe] after powerOn(true): isPowerOn=%d\n", (int)dsp.isPowerOn());
    dsp.setEffectValue(DfxDsp::Fidelity, 0.0f);
    std::printf("[bypass probe] after setEffectValue(Fidelity,0): isPowerOn=%d\n", (int)dsp.isPowerOn());
    dsp.setEffectValue(DfxDsp::Fidelity, 0.5f);
    std::printf("[bypass probe] after setEffectValue(Fidelity,0.5): isPowerOn=%d\n", (int)dsp.isPowerOn());
    for (int e = 0; e < (int)DfxDsp::NumEffects; ++e) dsp.setEffectValue((DfxDsp::Effect)e, 0.0f);
    std::printf("[bypass probe] after all effects=0: isPowerOn=%d\n", (int)dsp.isPowerOn());

    runCase(dsp, "no effects",     -1,                        0.0f, 0.5f);
    runCase(dsp, "DynBoost 0.5",   (int)DfxDsp::DynamicBoost, 0.5f, 0.5f);

    // ---- EQ test: does moving an EQ band actually change the 1 kHz level? ----
    std::printf("\n-- EQ TEST: boost/cut the band nearest %.0f Hz, measure fundamental --\n", FREQ);
    for (int e = 0; e < (int)DfxDsp::NumEffects; ++e)
        dsp.setEffectValue((DfxDsp::Effect)e, 0.0f);
    dsp.powerOn(true);
    std::printf("isPowerOn (after powerOn true) = %d\n", (int)dsp.isPowerOn());

    const int nb = dsp.getNumEqBands();
    int nearest = 0; double best = 1e18;
    for (int b = 0; b < nb; ++b) {
        double d = std::fabs(dsp.getEqBandFrequency(b) - FREQ);
        if (d < best) { best = d; nearest = b; }
    }
    std::printf("numBands=%d nearestBand=%d freq=%.0f Hz\n",
                nb, nearest, dsp.getEqBandFrequency(nearest));

    auto measureFund = [&](void)->double {
        std::vector<float> inter(BLK*2), out; out.reserve(48000);
        double ph=0, dph=2.0*M_PI*FREQ/SR;
        for (int blk=0; blk<200; ++blk) {
            for (int i=0;i<BLK;++i){ float s=0.3f*(float)std::sin(ph); ph+=dph; if(ph>2*M_PI)ph-=2*M_PI; inter[i*2]=s; inter[i*2+1]=s; }
            dsp.processAudio(reinterpret_cast<short*>(inter.data()), reinterpret_cast<short*>(inter.data()), BLK, 0);
            if (blk<40) continue;
            for (int i=0;i<BLK;++i) out.push_back(inter[i*2]);
        }
        return goertzel(out, FREQ);
    };

    dsp.setEqBandBoostCut(nearest, 0.0f);   double flat  = measureFund();
    dsp.setEqBandBoostCut(nearest, 12.0f);  double boost = measureFund();
    dsp.setEqBandBoostCut(nearest, -12.0f); double cut   = measureFund();
    dsp.setEqBandBoostCut(nearest, 0.0f);
    std::printf("fundamental: flat=%.4f  +12dB=%.4f (%.1fx)  -12dB=%.4f (%.1fx)\n",
                flat, boost, flat>1e-9?boost/flat:0.0, cut, flat>1e-9?cut/flat:0.0);
    std::printf(boost/flat > 1.5 ? ">>> EQ WORKS\n" : ">>> EQ DEAD (no change)\n");
    return 0;
}
