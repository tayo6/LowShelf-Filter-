# Low Shelf Filter — VST3 Plugin

A JUCE 7.0.12 VST3 plugin with a custom rotary knob for low-shelf filter control, built for Windows.

## Features

| Feature | Description |
|---------|-------------|
| **Custom Rotary Knob** | `LookAndFeel`-based implementation |
| **Background** | 360 × 360 px white canvas |
| **Arc Size** | 200 × 174 px effective area |
| **Arc Span** | ~5 o'clock → ~7 o'clock (bottom arc via 6 o'clock) |
| **Gradient** | Green (0) → Yellow (50) → Red (100) |
| **Stroke Style** | Rounded caps, matching reference SVG |
| **Interaction** | Velocity-based drag, double-click reset |
| **DSP** | IIR low-shelf at 100 Hz, 0 dB → –12 dB |

## Building Locally (Windows)

**Prerequisites:**
- Visual Studio 2022 (C++ workload)
- CMake ≥ 3.15
- Git

```powershell
# Clone
git clone --recursive https://github.com/yourname/LowShelfFilter.git
cd LowShelfFilter

# Configure
cmake -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release

# Build VST3
cmake --build build --config Release --target LowShelfFilter_VST3
```

**Output:**
```
build/LowShelfFilter_artefacts/Release/VST3/LowShelfFilter.vst3
```

## GitHub Actions CI

The included workflow (`.github/workflows/build.yml`) compiles the Windows VST3 automatically on every push to `main` or `master`.

## Project Structure

```
LowShelfFilter/
├── CMakeLists.txt                    # CMake build
├── Source/
│   ├── CustomLookAndFeel.h/.cpp      # Custom rotary knob
│   ├── PluginProcessor.h/.cpp        # DSP (low-shelf filter)
│   └── PluginEditor.h/.cpp           # 360×360 UI
└── .github/workflows/build.yml       # CI for Windows VST3
```

## License

MIT
