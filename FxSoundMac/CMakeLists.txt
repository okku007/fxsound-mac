cmake_minimum_required(VERSION 3.22)
project(FxSoundMac VERSION 0.1.0 LANGUAGES C CXX)

# Must be called before any add_test() calls, including those in subdirectories.
enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "Minimum macOS version")
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "Universal binary archs")

set(REPO_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/DfxDspLib.cmake)

include(FetchContent)
FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 7.0.12
)
FetchContent_MakeAvailable(juce)

# Patch JUCE 7.0.12 for Xcode 26 / macOS 26 SDK compatibility.
# CGWindowListCreateImage was removed (not just deprecated) in macOS 15 SDK.
set(_juce_windowing_patch
    "${juce_SOURCE_DIR}/modules/juce_gui_basics/native/juce_Windowing_mac.mm")
if(EXISTS "${_juce_windowing_patch}")
    execute_process(
        COMMAND python3
            "${CMAKE_CURRENT_SOURCE_DIR}/patches/patch_juce_windowing.py"
            "${_juce_windowing_patch}"
        RESULT_VARIABLE _patch_result
    )
    if(NOT _patch_result EQUAL 0)
        message(WARNING "JUCE windowing patch returned ${_patch_result} (may already be applied)")
    endif()
endif()

# Bundle SVG assets and Gilroy fonts as compile-time binary data.
juce_add_binary_data(FxSoundMacAssets
    SOURCES
        Resources/Images/power_on.svg
        Resources/Images/power_off.svg
        Resources/Images/Slider_Thumb.svg
        Resources/Images/Slider_Thumb_bw.svg
        Resources/Images/dropdown_arrow_hover.svg
        Resources/Images/dropdown_arrow_bw.svg
        Resources/Images/logo-white.svg
        Resources/Images/settings.svg
        Resources/Fonts/Gilroy-Regular.ttf
        Resources/Fonts/Gilroy-Semibold.ttf
        Resources/Fonts/Gilroy-Bold.ttf
)

juce_add_gui_app(FxSoundMac
    PRODUCT_NAME "FxSound"
    BUNDLE_ID "com.fxsound.mac"
    ICON_BIG "${CMAKE_CURRENT_SOURCE_DIR}/Resources/AppIcon.png"
    MICROPHONE_PERMISSION_ENABLED TRUE
    MICROPHONE_PERMISSION_TEXT "FxSound needs audio input access to capture system audio from the BlackHole virtual device for real-time processing."
)

target_sources(FxSoundMac PRIVATE
    Source/App/FxSoundMacApp.cpp
    Source/App/MainWindow.cpp
    Source/DSP/LegacyDspAdapter.cpp
    Source/Audio/DeviceValidation.cpp
    Source/Audio/MacAudioEngine.cpp
    Source/Support/PresetLibrary.cpp
    Source/Support/FxController.cpp
    Source/UI/FxTheme.cpp
    Source/UI/FxPowerButton.cpp
    Source/UI/FxEffects.cpp
    Source/UI/FxEqualizer.cpp
    Source/UI/MainComponent.cpp
    Source/UI/SetupStatusPanel.cpp
)

target_compile_definitions(FxSoundMac PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:FxSoundMac,JUCE_PRODUCT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:FxSoundMac,JUCE_VERSION>"
)

target_link_libraries(FxSoundMac PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_devices
    juce::juce_audio_basics
    juce::juce_gui_extra
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags
    FxSoundMacAssets
    DfxDspMac
)

target_include_directories(FxSoundMac PRIVATE
    Source/DSP
    Source/Audio
    Source/Support
    Source/UI
)

# Ad-hoc codesign for local development/testing.
set_target_properties(FxSoundMac PROPERTIES
    XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "-"
    XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED YES
    XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED NO
)

juce_add_console_app(FxSoundMacTests PRODUCT_NAME "FxSoundMacTests")

target_sources(FxSoundMacTests PRIVATE
    Tests/TestMain.cpp
    Tests/SmokeTest.cpp
    Tests/DspProbeTest.cpp
    Tests/LegacyDspAdapterTest.cpp
    Tests/DeviceValidationTest.cpp
    Tests/PresetLibraryTest.cpp
    Tests/FxControllerTest.cpp
    Source/DSP/LegacyDspAdapter.cpp
    Source/Audio/DeviceValidation.cpp
    Source/Support/PresetLibrary.cpp
    Source/Support/FxController.cpp
)

target_link_libraries(FxSoundMacTests PRIVATE
    juce::juce_core
    juce::juce_audio_basics
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags
    DfxDspMac
)

target_include_directories(FxSoundMacTests PRIVATE
    ${REPO_ROOT}/dsp/include
    Source/DSP
    Source/Audio
    Source/Support
)

add_test(NAME FxSoundMacTests COMMAND FxSoundMacTests)

# Verify NSMicrophoneUsageDescription is present in the built app bundle.
add_test(NAME InfoPlistMicCheck
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/Tests/check_info_plist.sh
            $<TARGET_BUNDLE_DIR:FxSoundMac>)

# Bundle curated presets into the app at Contents/Resources/Presets/
set(FXMAC_PRESET_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Presets)

add_custom_command(TARGET FxSoundMac POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_BUNDLE_CONTENT_DIR:FxSoundMac>/Resources/Presets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${FXMAC_PRESET_DIR}"
        "$<TARGET_BUNDLE_CONTENT_DIR:FxSoundMac>/Resources/Presets"
    COMMENT "Bundling presets into FxSoundMac.app"
)

# JUCE's ICON_BIG generator emits a single-resolution .icns (only ic08/256px),
# which macOS won't render in the Dock (shows the generic placeholder). Overwrite
# it with a full multi-resolution AppIcon.icns (16…1024 incl. @2x). CFBundleIconFile
# already points at Icon.icns, so replacing the file is enough.
add_custom_command(TARGET FxSoundMac POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/Resources/AppIcon.icns"
        "$<TARGET_BUNDLE_CONTENT_DIR:FxSoundMac>/Resources/Icon.icns"
    COMMENT "Installing full-resolution app icon"
)
