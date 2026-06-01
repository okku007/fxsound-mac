#!/usr/bin/env bash
# Standalone DSP compile probe — Option B pre-flight.
# Run from the repo root: bash FxSoundMac/probe/build_probe.sh
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
DSP="${REPO_ROOT}/dsp"
SHIM="${REPO_ROOT}/FxSoundMac/Source/DSP"
PROBE="${REPO_ROOT}/FxSoundMac/probe"
OUT="${PROBE}/probe_out"

mkdir -p "$OUT"

INCLUDES=(
    "-I${DSP}"
    "-I${DSP}/include"
    "-I${DSP}/ptutil/include"
    "-I${DSP}/ptutil/dfxp"
    "-I${DSP}/ptutil/COM"
    "-I${DSP}/ptutil/VALS"
    "-I${DSP}/ptutil/PRELST"
    "-I${DSP}/ptutil/Filt"
    "-I${DSP}/ptutil/Qnt"
    "-I${DSP}/ptutil/realSample"
    "-I${DSP}/ptutil/SOS"
    "-I${DSP}/ptutil/DspUtil"
    "-I${DSP}/ptutil/DspUtil/GraphicEq"
    "-I${DSP}/ptutil/DspUtil/BinauralSync"
    "-I${DSP}/ptutil/DspUtil/SurroundSyn"
    "-I${DSP}/ptutil/DspUtil/spectrum"
    "-I${DSP}/ptutil/dfxSharedUtil"
    "-I${DSP}/ptutil/PWAV"
    "-I${DSP}/ptComSftDfx"
    "-I${DSP}/ptechDsp/Aural/Aural032"
    "-I${DSP}/ptechDsp/Lex/Lex32"
    "-I${DSP}/ptechDsp/Maximizer/Maxi32"
    "-I${DSP}/ptechDsp/Peq/Peq832"
    "-I${DSP}/ptechDsp/Play/Play32"
    "-I${DSP}/ptechDsp/wide/Wide32"
    "-I${DSP}/ptechDsp/Dly32/Dly832"
    "-I${SHIM}"
    "-I${REPO_ROOT}/audiopassthru/include"
    "-I${REPO_ROOT}/audiopassthru/src/SLOUT"
    "-I${REPO_ROOT}/audiopassthru/src/FILE"
    "-I${REPO_ROOT}/audiopassthru/src/MTH"
    "-I${REPO_ROOT}/audiopassthru/src/pstr"
    "-I${REPO_ROOT}/audiopassthru/src/reg"
    "-I${REPO_ROOT}/audiopassthru/src/ptime"
)

COMMON_FLAGS=(
    -DPT_TURN_OFF_MEM_TRACE
    -DDSPSOFT_TARGET
    -DDSPSOFT_32_BIT
    "-DPT_DSP_BUILD=PT_DSP_DFX"
    -Wno-everything
    "${INCLUDES[@]}"
)

# C sources — must be compiled with clang (C mode) to preserve C linkage
C_SOURCES=(
    "${DSP}/ptComSftDfx/Comsftwr.c"
    "${DSP}/ptechDsp/Aural/Aural032/Auralp32.c"
    "${DSP}/ptechDsp/Lex/Lex32/Lex32.c"
    "${DSP}/ptechDsp/Maximizer/Maxi32/Maxi32.c"
    "${DSP}/ptechDsp/Peq/Peq832/peq8p32.c"
    "${DSP}/ptechDsp/Play/Play32/Play32.c"
    "${DSP}/ptechDsp/wide/Wide32/Wide32.c"
    "${DSP}/ptechDsp/Dly32/Dly832/dly8p32.c"
    "${SHIM}/dsps_alias_stub.c"
)

# C++ sources
CPP_SOURCES=(
    # Top-level DfxDsp wrappers
    "${DSP}/DfxDsp.cpp"
    "${DSP}/DfxDspPrivate.cpp"
    "${DSP}/DfxDspEq.cpp"
    "${DSP}/DfxDspPreset.cpp"
    "${DSP}/DfxDspRegistry.cpp"

    # dfxp — core DSP processing layer
    "${DSP}/ptutil/dfxp/dfxpComm.cpp"
    "${DSP}/ptutil/dfxp/dfxpEq.cpp"
    "${DSP}/ptutil/dfxp/dfxpGet.cpp"
    "${DSP}/ptutil/dfxp/dfxpInit.cpp"
    "${DSP}/ptutil/dfxp/dfxpProcess.cpp"
    "${DSP}/ptutil/dfxp/dfxpProcessClear.cpp"
    "${DSP}/ptutil/dfxp/dfxpProcessInt.cpp"
    "${DSP}/ptutil/dfxp/dfxpProcessReal.cpp"
    "${DSP}/ptutil/dfxp/dfxpQnt.cpp"
    "${DSP}/ptutil/dfxp/dfxpQuit.cpp"
    "${DSP}/ptutil/dfxp/dfxpRegistryStandard.cpp"
    "${DSP}/ptutil/dfxp/dfxpSession.cpp"
    "${DSP}/ptutil/dfxp/dfxpSet.cpp"
    "${DSP}/ptutil/dfxp/dfxpSpectrum.cpp"
    "${DSP}/ptutil/dfxp/dfxpUniversal.cpp"

    # VALS — preset file format
    "${DSP}/ptutil/VALS/Vals.cpp"
    "${DSP}/ptutil/VALS/Valscfg.cpp"
    "${DSP}/ptutil/VALS/Valsfile.cpp"
    "${DSP}/ptutil/VALS/Valsget.cpp"
    "${DSP}/ptutil/VALS/Valsset.cpp"
    "${DSP}/ptutil/VALS/Valswarp.cpp"

    # QNT — quantization
    "${DSP}/ptutil/Qnt/Qnt.cpp"
    "${DSP}/ptutil/Qnt/Qnt2But.cpp"
    "${DSP}/ptutil/Qnt/QntitoBoostCut.cpp"
    "${DSP}/ptutil/Qnt/Qntitol.cpp"
    "${DSP}/ptutil/Qnt/Qntitor.cpp"
    "${DSP}/ptutil/Qnt/Qntitor2.cpp"
    "${DSP}/ptutil/Qnt/Qntrtoi.cpp"
    "${DSP}/ptutil/Qnt/Qntrtol.cpp"
    "${DSP}/ptutil/Qnt/Qntrtor.cpp"

    # PRELST — preset list
    "${DSP}/ptutil/PRELST/Prelst.cpp"

    # COM — communications layer
    "${DSP}/ptutil/COM/Com.cpp"
    "${DSP}/ptutil/COM/ComMem.cpp"
    "${DSP}/ptutil/COM/Comeprom.cpp"
    "${DSP}/ptutil/COM/Comget.cpp"
    "${DSP}/ptutil/COM/Compass.cpp"
    "${DSP}/ptutil/COM/Comread.cpp"
    "${DSP}/ptutil/COM/Comwave.cpp"
    "${DSP}/ptutil/COM/Comwrite.cpp"

    # Filt — filter utilities
    "${DSP}/ptutil/Filt/Fil12But.cpp"
    "${DSP}/ptutil/Filt/FiltCalcBiqd.cpp"
    "${DSP}/ptutil/Filt/FiltCalcFilterResponse.cpp"
    "${DSP}/ptutil/Filt/FiltRun.cpp"
    "${DSP}/ptutil/Filt/FiltbiqdSos.cpp"
    "${DSP}/ptutil/Filt/Filtpoly.cpp"

    # SOS — second-order sections
    "${DSP}/ptutil/SOS/Sos.cpp"
    "${DSP}/ptutil/SOS/SosGet.cpp"
    "${DSP}/ptutil/SOS/SosProcess.cpp"
    "${DSP}/ptutil/SOS/SosSet.cpp"

    # DspUtil — GraphicEq, BinauralSyn, SurroundSyn, spectrum
    "${DSP}/ptutil/DspUtil/GraphicEq/GraphicEqGet.cpp"
    "${DSP}/ptutil/DspUtil/GraphicEq/GraphicEqInit.cpp"
    "${DSP}/ptutil/DspUtil/GraphicEq/GraphicEqInitBands.cpp"
    "${DSP}/ptutil/DspUtil/GraphicEq/GraphicEqInitSections.cpp"
    "${DSP}/ptutil/DspUtil/GraphicEq/GraphicEqProcess.cpp"
    "${DSP}/ptutil/DspUtil/GraphicEq/GraphicEqSet.cpp"
    "${DSP}/ptutil/DspUtil/BinauralSync/BinauralSynGet.cpp"
    "${DSP}/ptutil/DspUtil/BinauralSync/BinauralSynInit.cpp"
    "${DSP}/ptutil/DspUtil/BinauralSync/BinauralSynProcess.cpp"
    "${DSP}/ptutil/DspUtil/BinauralSync/BinauralSynSet.cpp"
    "${DSP}/ptutil/DspUtil/SurroundSyn/SurroundSynInit.cpp"
    "${DSP}/ptutil/DspUtil/SurroundSyn/SurroundSynProcess.cpp"
    "${DSP}/ptutil/DspUtil/spectrum/spectrumGet.cpp"
    "${DSP}/ptutil/DspUtil/spectrum/spectrumInit.cpp"
    "${DSP}/ptutil/DspUtil/spectrum/spectrumMessageValues.cpp"
    "${DSP}/ptutil/DspUtil/spectrum/spectrumProcess.cpp"
    "${DSP}/ptutil/DspUtil/spectrum/spectrumReset.cpp"
    "${DSP}/ptutil/DspUtil/spectrum/spectrumSet.cpp"

    # dfxSharedUtil
    "${DSP}/ptutil/dfxSharedUtil/dfxSharedUtil.cpp"

    # realSample
    "${DSP}/ptutil/realSample/realSampleForceLegalValues.cpp"

    # PWAV — wave format conversion (macOS stub, full PWAV is Windows-specific)
    "${SHIM}/pwav_mac_stub.cpp"

    # ptComSftDfx C++ wrapper
    "${DSP}/ptComSftDfx/ComsftwrCPP.cpp"

    # Probe
    "${PROBE}/dsp_probe.cpp"

    # CSlout implementation (from audiopassthru)
    "${REPO_ROOT}/audiopassthru/src/SLOUT/Slout.cpp"

    # FILE utilities — macOS stub
    "${SHIM}/file_mac_stub.cpp"

    # MTH utilities (from audiopassthru)
    "${REPO_ROOT}/audiopassthru/src/MTH/MthBuffer.cpp"
    "${REPO_ROOT}/audiopassthru/src/MTH/MthUtil.cpp"
    "${REPO_ROOT}/audiopassthru/src/MTH/Mthfreq.cpp"
    "${REPO_ROOT}/audiopassthru/src/MTH/MthWarp.cpp"
    "${REPO_ROOT}/audiopassthru/src/MTH/MthRand.cpp"
    "${REPO_ROOT}/audiopassthru/src/MTH/MthStat.cpp"
    "${REPO_ROOT}/audiopassthru/src/MTH/MthTrig.cpp"
    "${REPO_ROOT}/audiopassthru/src/MTH/MthRegion.cpp"
    "${REPO_ROOT}/audiopassthru/src/MTH/MthLinearRegression.cpp"
    "${REPO_ROOT}/audiopassthru/src/MTH/Mthcrypt.cpp"

    # pstr utilities — macOS stubs
    "${SHIM}/pstrWide_mac.cpp"
    "${SHIM}/pstr_mac_stub.cpp"

    # macOS stubs for Windows-only subsystems
    "${SHIM}/reg_mac_stub.cpp"
    "${SHIM}/hrdwr_mac_stub.cpp"
    "${SHIM}/mth_mac_stub.cpp"
)

# Step 1: Compile C sources with clang (preserves C linkage)
echo "Compiling C sources..."
C_OBJECTS=()
for src in "${C_SOURCES[@]}"; do
    obj="${OUT}/$(basename "${src%.c}").o"
    clang -std=c11 -arch arm64 -arch x86_64 \
        "${COMMON_FLAGS[@]}" \
        -c "$src" -o "$obj" 2>&1
    C_OBJECTS+=("$obj")
done

# Step 2: Compile and link C++ sources with clang++
echo "Compiling and linking C++ sources..."
clang++ -std=c++17 -arch arm64 -arch x86_64 \
    "${COMMON_FLAGS[@]}" \
    "${CPP_SOURCES[@]}" \
    "${C_OBJECTS[@]}" \
    -o "${OUT}/dsp_probe" \
    2>&1

echo ""
echo "Build succeeded. Running probe..."
"${OUT}/dsp_probe"
