cmake_minimum_required(VERSION 3.22)

# CMake 3.29+ introduced new policies that JUCE 8.0.2 does not yet handle correctly
# when a newer toolchain is installed in GitHub Actions. Guard the newer policies while
# keeping compatibility with the older JUCE CMake scripts used by this project.
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()
if(POLICY CMP0148)
    cmake_policy(SET CMP0148 NEW)
endif()
if(POLICY CMP0175)
    cmake_policy(SET CMP0175 NEW)
endif()

project(MAKOSIZER VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if (APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Minimum macOS deployment target")
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "macOS architectures")
endif()

include(FetchContent)
FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.2
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)

if (APPLE)
    set(MAKOSIZER_FORMATS VST3 AU)
else()
    set(MAKOSIZER_FORMATS VST3)
endif()

juce_add_plugin(Makosizer
    COMPANY_NAME "MAKOSIZER"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Mksz
    PLUGIN_CODE MkSz
    FORMATS ${MAKOSIZER_FORMATS}
    PRODUCT_NAME "Makosizer")

target_sources(Makosizer PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/LookAndFeel.cpp
    Source/LedMeter.cpp
    Source/DestroyKnob.cpp
    Source/PresetManager.cpp
    Components/SharkLogo.cpp
    DSP/Saturator.cpp
    DSP/VinylSim.cpp
    DSP/BitCrusher.cpp
    DSP/WowFlutter.cpp
    DSP/NoiseGenerator.cpp
    DSP/StereoWidth.cpp
    DSP/SoftClipper.cpp
    DSP/Limiter.cpp)

target_include_directories(Makosizer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
    ${CMAKE_CURRENT_SOURCE_DIR}/DSP
    ${CMAKE_CURRENT_SOURCE_DIR}/Components)

target_compile_definitions(Makosizer PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0)

target_link_libraries(Makosizer PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags)

juce_generate_juce_header(Makosizer)
