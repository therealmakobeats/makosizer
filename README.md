# Makosizer

A production-ready JUCE 8 audio plugin suite focused on vintage lo-fi degradation.

## Overview

Makosizer is a dark, SP-303 inspired mastering/processor plugin featuring:
- drive and destroy macros
- vinyl simulation
- wow/flutter modulation
- noise generation
- bit crushing
- stereo width control
- transparent limiting
- retro LCD interface

## Building

### Prerequisites

- CMake 3.22+
- C++20 compiler
- JUCE 8 (downloaded automatically via CMake)
- Windows 10+ or macOS 10.13+

### Configure

#### Windows

```bash
cmake -B build -S . -G "Visual Studio 17 2022" -A x64
```

#### macOS

```bash
cmake -B build -S . -G Xcode -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64"
```

### Build

```bash
cmake --build build --config Release
```

### Output

- Windows: `build/Makosizer_artefacts/Release/VST3/Makosizer.vst3`
- macOS: `build/Makosizer_artefacts/Release/VST3/Makosizer.vst3`
- macOS AU: `build/Makosizer_artefacts/Release/AU/Makosizer.component`

## GitHub Actions

The repository includes `.github/workflows/build.yml` to build:
- Windows VST3
- macOS VST3
- macOS AU

These jobs upload build artifacts automatically as part of CI.

## Installer Creation

### Windows

Use Inno Setup with `Installer/Windows/Makosizer.iss`.

```bash
"C:\Program Files (x86)\Inno Setup 6\ISCC.exe" Installer\Windows\Makosizer.iss
```

This produces `Makosizer_Setup.exe`.

### macOS Packaging

```bash
chmod +x Installer/macOS/build-pkg.sh
./Installer/macOS/build-pkg.sh
```

This produces `Makosizer.pkg` in the project root.

## Release Workflow

- Tag a release: `v1.0.0`
- GitHub Actions builds all plugin formats.
- Upload the generated archives as release artifacts.
- Publish the installer packages to the release.

## Notes

This project intentionally uses a minimal but production-ready plugin architecture with
the JUCE AudioProcessorValueTreeState and a custom retro look-and-feel.
