#!/usr/bin/env bash
# Verifies NSMicrophoneUsageDescription is present in the built FxSoundMac.app bundle.
# Usage: check_info_plist.sh <path-to.app>
set -euo pipefail

APP="${1:?usage: check_info_plist.sh <path-to.app>}"
PLIST="$APP/Contents/Info.plist"

if /usr/libexec/PlistBuddy -c "Print :NSMicrophoneUsageDescription" "$PLIST" >/dev/null 2>&1; then
    echo "OK: NSMicrophoneUsageDescription present in $PLIST"
    exit 0
else
    echo "FAIL: NSMicrophoneUsageDescription missing from $PLIST"
    exit 1
fi
