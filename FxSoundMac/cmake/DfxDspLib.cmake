# DfxDspLib.cmake — builds the proven DfxDsp macOS shim as a static library.
# Source list and flags are taken directly from FxSoundMac/probe/build_probe.sh
# (Task 3a, already proven). Do NOT modify without re-running the probe.
#
# Callers must set REPO_ROOT before including this file.
# CMakeLists.txt sets: set(REPO_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)

# ---------------------------------------------------------------------------
# C sources — compiled as C (not C++) to preserve C linkage for DSP algorithms
# ---------------------------------------------------------------------------
set(DFXDSP_C_SOURCES
    ${REPO_ROOT}/dsp/ptComSftDfx/Comsftwr.c
    ${REPO_ROOT}/dsp/ptechDsp/Aural/Aural032/Auralp32.c
    ${REPO_ROOT}/dsp/ptechDsp/Lex/Lex32/Lex32.c
    ${REPO_ROOT}/dsp/ptechDsp/Maximizer/Maxi32/Maxi32.c
    ${REPO_ROOT}/dsp/ptechDsp/Peq/Peq832/peq8p32.c
    ${REPO_ROOT}/dsp/ptechDsp/Play/Play32/Play32.c
    ${REPO_ROOT}/dsp/ptechDsp/wide/Wide32/Wide32.c
    ${REPO_ROOT}/dsp/ptechDsp/Dly32/Dly832/dly8p32.c
    ${REPO_ROOT}/FxSoundMac/Source/DSP/dsps_alias_stub.c
)

# ---------------------------------------------------------------------------
# C++ sources
# ---------------------------------------------------------------------------
set(DFXDSP_CXX_SOURCES
    ${REPO_ROOT}/dsp/DfxDsp.cpp
    ${REPO_ROOT}/dsp/DfxDspPrivate.cpp
    ${REPO_ROOT}/dsp/DfxDspEq.cpp
    ${REPO_ROOT}/dsp/DfxDspPreset.cpp
    ${REPO_ROOT}/dsp/DfxDspRegistry.cpp
    ${REPO_ROOT}/dsp/ptutil/dfxp/dfxpComm.cpp
    ${REPO_ROOT}/dsp/ptutil/dfxp/dfxpEq.cpp
    ${REPO_ROOT}/dsp/ptutil/dfxp/dfxpGet.cpp
    ${REPO_ROOT}/dsp/ptutil/dfxp/dfxpInit.cpp
    ${REPO_ROOT}/dsp/ptutil/dfxp/dfxpProcess.cpp
    ${REPO_ROOT}/dsp/ptutil/dfxp/dfxpProcessClear.cpp
    ${REPO_ROOT}/dsp/ptutil/dfxp/dfxpProcessInt.cpp
    ${REPO_ROOT}/dsp/ptutil/dfxp/dfxpProcessReal.cpp
    ${REPO_ROOT}/dsp/ptutil/dfxp/dfxpQnt.cpp
    ${REPO_ROOT}/dsp/ptutil/dfxp/dfxpQuit.cpp
    ${REPO_ROOT}/dsp/ptutil/dfxp/dfxpRegistryStandard.cpp
    ${REPO_ROOT}/dsp/ptutil/dfxp/dfxpSession.cpp
    ${REPO_ROOT}/dsp/ptutil/dfxp/dfxpSet.cpp
    ${REPO_ROOT}/dsp/ptutil/dfxp/dfxpSpectrum.cpp
    ${REPO_ROOT}/dsp/ptutil/dfxp/dfxpUniversal.cpp
    ${REPO_ROOT}/dsp/ptutil/VALS/Vals.cpp
    ${REPO_ROOT}/dsp/ptutil/VALS/Valscfg.cpp
    ${REPO_ROOT}/dsp/ptutil/VALS/Valsfile.cpp
    ${REPO_ROOT}/dsp/ptutil/VALS/Valsget.cpp
    ${REPO_ROOT}/dsp/ptutil/VALS/Valsset.cpp
    ${REPO_ROOT}/dsp/ptutil/VALS/Valswarp.cpp
    ${REPO_ROOT}/dsp/ptutil/Qnt/Qnt.cpp
    ${REPO_ROOT}/dsp/ptutil/Qnt/Qnt2But.cpp
    ${REPO_ROOT}/dsp/ptutil/Qnt/QntitoBoostCut.cpp
    ${REPO_ROOT}/dsp/ptutil/Qnt/Qntitol.cpp
    ${REPO_ROOT}/dsp/ptutil/Qnt/Qntitor.cpp
    ${REPO_ROOT}/dsp/ptutil/Qnt/Qntitor2.cpp
    ${REPO_ROOT}/dsp/ptutil/Qnt/Qntrtoi.cpp
    ${REPO_ROOT}/dsp/ptutil/Qnt/Qntrtol.cpp
    ${REPO_ROOT}/dsp/ptutil/Qnt/Qntrtor.cpp
    ${REPO_ROOT}/dsp/ptutil/PRELST/Prelst.cpp
    ${REPO_ROOT}/dsp/ptutil/COM/Com.cpp
    ${REPO_ROOT}/dsp/ptutil/COM/ComMem.cpp
    ${REPO_ROOT}/dsp/ptutil/COM/Comeprom.cpp
    ${REPO_ROOT}/dsp/ptutil/COM/Comget.cpp
    ${REPO_ROOT}/dsp/ptutil/COM/Compass.cpp
    ${REPO_ROOT}/dsp/ptutil/COM/Comread.cpp
    ${REPO_ROOT}/dsp/ptutil/COM/Comwave.cpp
    ${REPO_ROOT}/dsp/ptutil/COM/Comwrite.cpp
    ${REPO_ROOT}/dsp/ptutil/Filt/Fil12But.cpp
    ${REPO_ROOT}/dsp/ptutil/Filt/FiltCalcBiqd.cpp
    ${REPO_ROOT}/dsp/ptutil/Filt/FiltCalcFilterResponse.cpp
    ${REPO_ROOT}/dsp/ptutil/Filt/FiltRun.cpp
    ${REPO_ROOT}/dsp/ptutil/Filt/FiltbiqdSos.cpp
    ${REPO_ROOT}/dsp/ptutil/Filt/Filtpoly.cpp
    ${REPO_ROOT}/dsp/ptutil/SOS/Sos.cpp
    ${REPO_ROOT}/dsp/ptutil/SOS/SosGet.cpp
    ${REPO_ROOT}/dsp/ptutil/SOS/SosProcess.cpp
    ${REPO_ROOT}/dsp/ptutil/SOS/SosSet.cpp
    ${REPO_ROOT}/dsp/ptutil/DspUtil/GraphicEq/GraphicEqGet.cpp
    ${REPO_ROOT}/dsp/ptutil/DspUtil/GraphicEq/GraphicEqInit.cpp
    ${REPO_ROOT}/dsp/ptutil/DspUtil/GraphicEq/GraphicEqInitBands.cpp
    ${REPO_ROOT}/dsp/ptutil/DspUtil/GraphicEq/GraphicEqInitSections.cpp
    ${REPO_ROOT}/dsp/ptutil/DspUtil/GraphicEq/GraphicEqProcess.cpp
    ${REPO_ROOT}/dsp/ptutil/DspUtil/GraphicEq/GraphicEqSet.cpp
    ${REPO_ROOT}/dsp/ptutil/DspUtil/BinauralSync/BinauralSynGet.cpp
    ${REPO_ROOT}/dsp/ptutil/DspUtil/BinauralSync/BinauralSynInit.cpp
    ${REPO_ROOT}/dsp/ptutil/DspUtil/BinauralSync/BinauralSynProcess.cpp
    ${REPO_ROOT}/dsp/ptutil/DspUtil/BinauralSync/BinauralSynSet.cpp
    ${REPO_ROOT}/dsp/ptutil/DspUtil/SurroundSyn/SurroundSynInit.cpp
    ${REPO_ROOT}/dsp/ptutil/DspUtil/SurroundSyn/SurroundSynProcess.cpp
    ${REPO_ROOT}/dsp/ptutil/DspUtil/spectrum/spectrumGet.cpp
    ${REPO_ROOT}/dsp/ptutil/DspUtil/spectrum/spectrumInit.cpp
    ${REPO_ROOT}/dsp/ptutil/DspUtil/spectrum/spectrumMessageValues.cpp
    ${REPO_ROOT}/dsp/ptutil/DspUtil/spectrum/spectrumProcess.cpp
    ${REPO_ROOT}/dsp/ptutil/DspUtil/spectrum/spectrumReset.cpp
    ${REPO_ROOT}/dsp/ptutil/DspUtil/spectrum/spectrumSet.cpp
    ${REPO_ROOT}/dsp/ptutil/dfxSharedUtil/dfxSharedUtil.cpp
    ${REPO_ROOT}/dsp/ptutil/realSample/realSampleForceLegalValues.cpp
    ${REPO_ROOT}/FxSoundMac/Source/DSP/pwav_mac_stub.cpp
    ${REPO_ROOT}/dsp/ptComSftDfx/ComsftwrCPP.cpp
    ${REPO_ROOT}/audiopassthru/src/SLOUT/Slout.cpp
    ${REPO_ROOT}/FxSoundMac/Source/DSP/file_mac_stub.cpp
    ${REPO_ROOT}/audiopassthru/src/MTH/MthBuffer.cpp
    ${REPO_ROOT}/audiopassthru/src/MTH/MthUtil.cpp
    ${REPO_ROOT}/audiopassthru/src/MTH/Mthfreq.cpp
    ${REPO_ROOT}/audiopassthru/src/MTH/MthWarp.cpp
    ${REPO_ROOT}/audiopassthru/src/MTH/MthRand.cpp
    ${REPO_ROOT}/audiopassthru/src/MTH/MthStat.cpp
    ${REPO_ROOT}/audiopassthru/src/MTH/MthTrig.cpp
    ${REPO_ROOT}/audiopassthru/src/MTH/MthRegion.cpp
    ${REPO_ROOT}/audiopassthru/src/MTH/MthLinearRegression.cpp
    ${REPO_ROOT}/audiopassthru/src/MTH/Mthcrypt.cpp
    ${REPO_ROOT}/FxSoundMac/Source/DSP/pstrWide_mac.cpp
    ${REPO_ROOT}/FxSoundMac/Source/DSP/pstr_mac_stub.cpp
    ${REPO_ROOT}/FxSoundMac/Source/DSP/reg_mac_stub.cpp
    ${REPO_ROOT}/FxSoundMac/Source/DSP/hrdwr_mac_stub.cpp
    ${REPO_ROOT}/FxSoundMac/Source/DSP/mth_mac_stub.cpp
)

# ---------------------------------------------------------------------------
# Static library target
# ---------------------------------------------------------------------------
add_library(DfxDspMac STATIC
    ${DFXDSP_C_SOURCES}
    ${DFXDSP_CXX_SOURCES}
)

# Force C sources to compile as C (not C++) to preserve C linkage
set_source_files_properties(${DFXDSP_C_SOURCES} PROPERTIES LANGUAGE C)

# Required compile definitions (from probe script)
target_compile_definitions(DfxDspMac PRIVATE
    PT_TURN_OFF_MEM_TRACE
    DSPSOFT_TARGET
    DSPSOFT_32_BIT
    PT_DSP_BUILD=PT_DSP_DFX
)

# Suppress all legacy warnings — this is third-party/legacy DSP code
target_compile_options(DfxDspMac PRIVATE -Wno-everything)

# Include directories — PUBLIC so downstream targets (app, tests) inherit them
target_include_directories(DfxDspMac PUBLIC
    ${REPO_ROOT}/dsp/
    ${REPO_ROOT}/dsp/include/
    ${REPO_ROOT}/dsp/ptutil/include/
    ${REPO_ROOT}/dsp/ptutil/dfxp/
    ${REPO_ROOT}/dsp/ptutil/COM/
    ${REPO_ROOT}/dsp/ptutil/VALS/
    ${REPO_ROOT}/dsp/ptutil/PRELST/
    ${REPO_ROOT}/dsp/ptutil/Filt/
    ${REPO_ROOT}/dsp/ptutil/Qnt/
    ${REPO_ROOT}/dsp/ptutil/realSample/
    ${REPO_ROOT}/dsp/ptutil/SOS/
    ${REPO_ROOT}/dsp/ptutil/DspUtil/
    ${REPO_ROOT}/dsp/ptutil/DspUtil/GraphicEq/
    ${REPO_ROOT}/dsp/ptutil/DspUtil/BinauralSync/
    ${REPO_ROOT}/dsp/ptutil/DspUtil/SurroundSyn/
    ${REPO_ROOT}/dsp/ptutil/DspUtil/spectrum/
    ${REPO_ROOT}/dsp/ptutil/dfxSharedUtil/
    ${REPO_ROOT}/dsp/ptutil/PWAV/
    ${REPO_ROOT}/dsp/ptComSftDfx/
    ${REPO_ROOT}/dsp/ptechDsp/Aural/Aural032/
    ${REPO_ROOT}/dsp/ptechDsp/Lex/Lex32/
    ${REPO_ROOT}/dsp/ptechDsp/Maximizer/Maxi32/
    ${REPO_ROOT}/dsp/ptechDsp/Peq/Peq832/
    ${REPO_ROOT}/dsp/ptechDsp/Play/Play32/
    ${REPO_ROOT}/dsp/ptechDsp/wide/Wide32/
    ${REPO_ROOT}/dsp/ptechDsp/Dly32/Dly832/
    ${REPO_ROOT}/FxSoundMac/Source/DSP/
    ${REPO_ROOT}/audiopassthru/include/
    ${REPO_ROOT}/audiopassthru/src/SLOUT/
    ${REPO_ROOT}/audiopassthru/src/FILE/
    ${REPO_ROOT}/audiopassthru/src/MTH/
    ${REPO_ROOT}/audiopassthru/src/pstr/
    ${REPO_ROOT}/audiopassthru/src/reg/
    ${REPO_ROOT}/audiopassthru/src/ptime/
)
