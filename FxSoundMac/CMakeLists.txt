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

juce_add_gui_app(FxSoundMac
    PRODUCT_NAME "FxSoundMac"
    BUNDLE_ID "com.fxsound.mac"
    MICROPHONE_PERMISSION_ENABLED TRUE
    MICROPHONE_PERMISSION_TEXT "FxSound needs audio input access to capture system audio from the BlackHole virtual device for real-time processing."
)

target_sources(FxSoundMac PRIVATE
    Source/App/FxSoundMacApp.cpp
    Source/App/MainWindow.cpp
    Source/DSP/LegacyDspAdapter.cpp
    Source/Audio/DeviceValidation.cpp
    Source/Support/PresetLibrary.cpp
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
    DfxDspMac
)

target_include_directories(FxSoundMac PRIVATE
    Source/DSP
    Source/Audio
    Source/Support
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
    Source/DSP/LegacyDspAdapter.cpp
    Source/Audio/DeviceValidation.cpp
    Source/Support/PresetLibrary.cpp
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
