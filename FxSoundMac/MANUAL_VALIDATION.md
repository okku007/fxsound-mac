# FxSoundMac Manual Validation Checklist

This checklist covers audio-device and DSP behaviour that cannot be verified in CI.
It requires BlackHole 2ch, a physical output device (speakers or headphones), and an
audio source (browser, music player, or streaming video).

## Prerequisites

- [ ] BlackHole 2ch installed: `brew install --cask blackhole-2ch`
- [ ] A Multi-Output Device created in Audio MIDI Setup routing system audio to both
      BlackHole 2ch and your physical output (so you can hear audio while FxSoundMac
      processes it).
- [ ] FxSoundMac.app built and launched from
      `FxSoundMac/build/FxSoundMac_artefacts/Debug/FxSoundMac.app`
      (or the Release artefact).

## Setup and detection

- [ ] App launches without crashing.
- [ ] Status panel shows BlackHole guidance on first launch.
- [ ] If BlackHole is not installed, status panel shows the install command.
- [ ] Output device selector lists physical devices (BlackHole is excluded from the list).

## Audio path

- [ ] Select a physical output device and click **Start Audio**.
- [ ] Status panel shows "Audio is running."
- [ ] Audio from the system (browser, music) is audible through the selected output.
- [ ] No crackling, dropout, or distortion at default settings.

## DSP controls

- [ ] Selecting a different preset audibly changes the sound character.
- [ ] Each of the five effect sliders (Fidelity, Ambience, Surround, Dynamic Boost, Bass)
      audibly changes the output when moved.
- [ ] EQ sliders audibly affect the frequency balance.
- [ ] Output gain slider changes the playback level.
- [ ] Power off (toggle Power button) produces clean passthrough with no DSP processing.
- [ ] Bypass toggle produces clean passthrough.

## Device switching and recovery

- [ ] Switching the output device selector and clicking Start Audio restarts audio cleanly
      on the new device.
- [ ] Clicking **Stop Audio** stops processing.
- [ ] After stopping, the status panel shows recovery guidance mentioning BlackHole routing.
- [ ] If macOS system output is still routed to BlackHole after stopping, the user can
      recover by switching system output back to speakers in System Settings → Sound.

## Stability

- [ ] Quit and relaunch works without leaving the system in a broken audio state.
- [ ] No lip-sync drift or pitch change during video playback.
- [ ] No memory warnings or crashes after extended use (5+ minutes of audio).

## Automated checks (run before manual testing)

```bash
# From FxSoundMac/
cmake --build build --config Debug --target FxSoundMacTests
./build/FxSoundMacTests_artefacts/Debug/FxSoundMacTests   # must exit 0

bash Tests/check_info_plist.sh build/FxSoundMac_artefacts/Debug/FxSoundMac.app
# must print: OK: NSMicrophoneUsageDescription present

lipo -archs build/FxSoundMac_artefacts/Release/FxSoundMac.app/Contents/MacOS/FxSoundMac
# must print: x86_64 arm64
```
