# VST3Effekt+ — for YungOS by 29yuro

Native macOS Audio Unit (AU), VST3 and standalone effect, built with JUCE 8.0.12.

## What works in v1

Real-time drive, lowpass, delay, feedback, reverb, compressor, dry/wet and output gain; parameter automation; preset/state recall; mono/stereo support; custom dark-purple UI with **for YungOS by 29yuro** in the header.

**Not a byte-for-byte port** of the original Windows DLL: the original source code is unavailable. Original presets, proprietary effects and exact old GUI are not recovered. No external AI, no reference upload. This is a fresh native implementation.

## GitHub build

1. Create a GitHub repository named `VST3EffektPlus`.
2. Upload the **contents** of this directory, including `.github/workflows/build-macos.yml`.
3. Commit to `main` or open **Actions → Build macOS AU and VST3 → Run workflow**.
4. Download `VST3EffektPlus-macOS-AU-VST3` from the successful workflow run's Artifacts section.
5. On a Mac, copy the `.component` bundle to `~/Library/Audio/Plug-Ins/Components/` and restart Logic Pro; use Logic Pro → Settings → Plug-in Manager to rescan if needed.

Unsigned GitHub builds may trigger macOS security warnings. A production release requires signing and notarization with an Apple Developer ID.

## Local build

Requires macOS, Xcode, CMake 3.22+ and internet access to download JUCE:

```sh
cmake -S . -B build -G Xcode
cmake --build build --config Release --parallel 3
```

## Verification limitations

The source archive is statically checked before delivery, but macOS AU/VST3 compilation and Logic Pro loading must be validated by GitHub Actions or a real Mac. The workflow fails if either bundle is missing. No promise of zero bugs without those runtime tests.
