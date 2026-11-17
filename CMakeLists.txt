cmake_minimum_required(VERSION 3.22)

project(MAKOSIZER VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if (APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13")
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Build architectures for macOS" FORCE)
endif()

include(FetchContent)

set(JUCE_ENABLE_MODULE_SOURCE_GROUPS ON CACHE BOOL "" FORCE)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.2
)
FetchContent_MakeAvailable(JUCE)

set_property(GLOBAL PROPERTY USE_FOLDERS YES)

if (APPLE)
    set(MAKOSIZER_FORMATS VST3 AU)
else()
    set(MAKOSIZER_FORMATS VST3)
endif()

juce_add_plugin(Makosizer
    COMPANY_NAME "MAKOSIZER"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Mksz
    PLUGIN_CODE MkSz
    FORMATS ${MAKOSIZER_FORMATS}
    PRODUCT_NAME "Makosizer"
)

target_compile_definitions(Makosizer PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
)

set(MAKOSIZER_SOURCES
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/LookAndFeel.cpp
    Source/LedMeter.cpp
    Source/DestroyKnob.cpp
    Components/SharkLogo.cpp
    DSP/Saturator.cpp
    DSP/VinylSim.cpp
    DSP/BitCrusher.cpp
    DSP/WowFlutter.cpp
    DSP/NoiseGenerator.cpp
    DSP/StereoWidth.cpp
    DSP/SoftClipper.cpp
    DSP/Limiter.cpp
)

target_sources(Makosizer PRIVATE ${MAKOSIZER_SOURCES})

target_include_directories(Makosizer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
    ${CMAKE_CURRENT_SOURCE_DIR}/DSP
    ${CMAKE_CURRENT_SOURCE_DIR}/Components
)

target_link_libraries(Makosizer PRIVATE juce::juce_audio_utils)

juce_generate_juce_header(Makosizer)

if (MSVC)
    target_compile_options(Makosizer PRIVATE /W4 /permissive-)
else()
    target_compile_options(Makosizer PRIVATE -Wall -Wextra -Wpedantic)
endif()
