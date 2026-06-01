# PORTING_NOTES.md

## Purpose
This file tracks the Windows-to-macOS port of FxSound. It exists so the agent and the developer have one shared source of truth for what is Windows-specific, what has already been ported, what is blocked, and what still needs design decisions.

This document should be updated every time a Windows-specific dependency is identified, replaced, stubbed, or deferred.

## Port Status Summary

| Area | Status | Notes |
|---|---|---|
| Repo audit | Not started | Audit Win32, WASAPI/WDM, build flags, binary deps, JUCE modules |
| Build system | In progress | CMake scaffold created; JUCE 7.0.12 fetched and patched for Xcode 26/macOS 26 SDK; Debug build succeeds with ad-hoc signature |
| UI layer | Not started | Replace Win32-only UI/platform APIs with JUCE/macOS-safe code |
| Audio backend | Not started | Move to JUCE/CoreAudio + BlackHole Phase 1 path |
| Permissions | Not started | Add `NSMicrophoneUsageDescription` and review entitlements |
| Latency/sync | Not started | Validate sample rate, buffer size, drift, and output sync |
| Exit recovery | Not started | Prevent silent-audio trap on quit/crash |
| Packaging | Not started | Sign, notarize, then Homebrew Cask |

## Audit Checklist

### 1. Windows-only headers
Record every header that prevents a clean macOS build.

- [ ] `windows.h`
- [ ] `mmsystem.h`
- [ ] `mmdeviceapi.h`
- [ ] `audioclient.h`
- [ ] `functiondiscoverykeys_devpkey.h`
- [ ] Other:

### 2. Windows-only APIs and concepts
Record direct API calls, structs, handles, and assumptions.

| File | Symbol / API | Purpose | macOS/JUCE replacement | Status |
|---|---|---|---|---|
| example.cpp | `MessageBoxW` | Error dialog | `juce::AlertWindow::showMessageBoxAsync` | Open |
| example.cpp | `HWND` | Native window handle | Prefer JUCE `Component` / conditional native code | Open |
| example.cpp | WASAPI device enumeration | Audio device selection | `juce::AudioDeviceManager` | Open |

### 3. Build-system blockers
Track everything preventing a clean CMake-based macOS build.

| Item | Problem | Resolution path | Status |
|---|---|---|---|
| Visual Studio solution only | No portable build entrypoint | Create root `CMakeLists.txt` | Open |
| MSVC flags | Compiler-specific | Wrap in `if(MSVC)` | Open |
| Windows `.lib` dependencies | Not linkable on macOS | Replace from source / build universal `.a` | Open |

### 4. Binary dependency audit
Universal builds fail if linked libraries are not available for both `arm64` and `x86_64` [web:85][web:90]. Audit every static or dynamic library before wiring the universal build.

| Dependency | Current form | Source available? | macOS build available? | Universal-ready? | Status |
|---|---|---|---|---|---|
| DfxDsp | Unknown | Unknown | Unknown | Unknown | Open |
| AudioPassthru | Unknown | Unknown | Unknown | Unknown | Open |
| Other third-party lib |  |  |  |  |  |

### 5. JUCE module audit
If the repo uses custom modules, forked JUCE code, or old module names, note them before editing CMake.

| Module / fork | Where used | JUCE 7/8 equivalent | Action | Status |
|---|---|---|---|---|
| Unknown custom module |  |  | Audit first | Open |

## macOS-specific implementation notes

### BlackHole Phase 1 assumptions
- BlackHole is the interception layer in Phase 1.
- DSP remains in the JUCE app.
- Do **not** start custom HAL or IPC work in Phase 1.

### Permissions
- [ ] Add `NSMicrophoneUsageDescription` to `Info.plist`.
- [ ] Review whether `com.apple.security.device.audio-input` is needed for the chosen distribution model [web:73][web:79].

### Audio behavior risks
- [ ] Validate sample rate match between BlackHole and output device [web:74].
- [ ] Validate buffer size / latency window for media playback [web:83][web:86].
- [ ] Add app-level output gain because system volume may not work in multi-output routing [web:74][web:80].
- [ ] Add quit-time recovery or clear user messaging for silent-routing failures [web:87].

## File-by-file migration log
Use one section per edited file.

### Task 1 — CMake scaffold (FxSoundMac/)

#### `FxSoundMac/.gitignore`
- Created: ignores `build/` and `*.app` artefacts.

#### `FxSoundMac/CMakeLists.txt`
- Created: CMake 3.22+ project; JUCE 7.0.12 via FetchContent; `juce_add_gui_app` with `MICROPHONE_PERMISSION_ENABLED`; universal `arm64;x86_64`; macOS 13 deployment target; ad-hoc codesign attributes; post-fetch patch for Xcode 26 SDK compatibility.

#### `FxSoundMac/Resources/Info.plist.in`
- Created: contains `NSMicrophoneUsageDescription` for BlackHole audio input access.

#### `FxSoundMac/Source/App/FxSoundMacApp.h` / `.cpp`
- Created: `JUCEApplication` subclass; single-instance; creates `MainWindow` on `initialise`.

#### `FxSoundMac/Source/App/MainWindow.h` / `.cpp`
- Created: `DocumentWindow` subclass; 480×320 placeholder label; native title bar; delegates quit to `JUCEApplication`.

#### `FxSoundMac/patches/patch_juce_windowing.py`
- Created: Python patch script that guards `CGWindowListCreateImage` behind `MAC_OS_X_VERSION_MAX_ALLOWED < 150000`. Required because JUCE 7.0.12 uses this API which was removed (not just deprecated) in the macOS 15 / Xcode 26 SDK. Applied automatically at CMake configure time via `execute_process`.

**Build status:** `cmake -B build -G Xcode` + `cmake --build build --config Debug --target FxSoundMac` — **SUCCEEDED**
**Signature:** `codesign -dv` shows `Signature=adhoc`, `flags=0x2(adhoc)` ✓
**Concern:** Build produces `arm64` only (not fat binary) because Xcode 26 does not support `x86_64` targets on Apple Silicon — the universal binary goal will need revisiting when an Intel Mac or cross-compile toolchain is available.

### `path/to/file.cpp`
- Original Windows-specific behavior:
- Replacement on macOS:
- Guard used: `#if JUCE_WINDOWS` / `#if JUCE_MAC`
- Build status:
- Runtime status:
- Follow-up work:

## Decisions log
Record architecture decisions so the agent does not re-litigate them.

- Phase 1 uses BlackHole, not a custom HAL plug-in.
- Phase 2 may replace BlackHole with an AudioServerPlugIn.
- JUCE abstractions are preferred over raw CoreAudio in Phase 1.
- Windows build must continue to work while the macOS port is developed.

## Current blockers
- No audited list of Windows-only dependencies yet.
- No validated list of third-party binary dependencies yet.
- Universal binary (arm64+x86_64) not yet verified — Xcode 26 on Apple Silicon only produced arm64 in the Task 1 build; needs validation on Intel or with a cross-compile setup.

## Next actions
1. Audit the repo for Windows-only headers, APIs, and build flags.
2. Audit all third-party libraries and JUCE modules.
3. Create the initial CMake target structure.
4. Stub platform-specific files behind JUCE platform guards.
5. Wire the BlackHole input path only after the app builds.
