# Dante

Graphics engine. Targets OpenGL on Windows/Linux only. Metal, Vulkan, and WebGPU are
dropped entirely, their source trees don't exist in this repo, not just disabled by a
build flag.

## Prerequisites

| | Windows | Linux |
|---|---|---|
| Compiler | MSVC (Visual Studio 2022, Desktop C++ workload) | Clang 17+ |
| CMake | 3.22+ | 3.22+ |
| Build tool | Ninja | Ninja |

No graphics SDK to install on either platform. OpenGL builds against your existing GPU
driver.

- **Windows**: build from an x64 Native Tools Command Prompt for VS 2022 (or run
  `vcvarsall.bat x64` first). The Ninja generator needs `cl.exe`/`link.exe` on `PATH`;
  a plain terminal that never loaded the VS developer environment will configure fine
  but fail at the final link step. Opening the folder directly in Visual Studio or
  CLion via their built-in CMake integration also works and handles this for you
  automatically.
- **Linux**: install Clang 17 (e.g. via [apt.llvm.org](https://apt.llvm.org/)), Ninja,
  and X11 dev headers: `libglu1-mesa-dev libxi-dev libxcomposite-dev libxxf86vm-dev`.

## Build

```
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

Swap `Release` for `Debug` for an unoptimized build with debug symbols. Cap
parallelism with `--parallel 4` on a multi-core machine if a full-core build makes it
unusable to work on while it runs.

The first build compiles the whole engine and is slow: several minutes. Every build
after that is fast, Ninja only recompiles what changed.

## Run

```
build\Dante.exe        # Windows
build/Dante             # Linux
```

No install step, no arguments. A window opens immediately.

## Day-to-day workflow: rebuilding after changes

1. Edit `src/main.cpp`, add or edit a `.mat` material, or drop in a new asset.
2. `cmake --build build --parallel`. You only need to re-run the `cmake -B build ...`
   configure step if you touch `CMakeLists.txt` or add/remove a source file; routine
   edits never need it.
3. Run the binary again. Changes to `assets/` (models, HDRIs) are picked up on the next
   launch with no rebuild. `DANTE_ASSETS_DIR` points straight at the source tree in dev
   builds, nothing gets copied.
4. Watch stderr. `[Dante] ...` lines (glTF entity/animation counts, bounding boxes on
   load) are Dante's own logging; check there first if a model loads invisibly or
   looks wrong.

| Change | Rebuild needed? | Command |
|---|---|---|
| Edit `src/main.cpp` | Yes | `cmake --build build --parallel` |
| Add or edit a `.mat` shader | Yes (`matc` runs automatically) | `cmake --build build --parallel` |
| Add or swap a model/asset | No, just relaunch | `build\Dante.exe` |
| Touch `CMakeLists.txt` or add/remove a source file | Yes, reconfigure first | `cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release` then `cmake --build build --parallel` |

When building it again use Dev cmd prompt 
x64 Native Tools Command Prompt for VS 2022
cd C:\Users\dommc\Dante
cmake --build build

## Adding your own models

Any static or animated glTF/GLB loads through the same pattern as `CharacterModel` in
`main.cpp`: `gltfio::AssetLoader` + `ResourceLoader`, plus `Animator` if it has
animations. Copy that struct's `create()`/`destroy()` shape for a new model, point it
at `assets/models/<name>/`, and add/remove its entities from the `Scene`.

Only PNG/JPEG textures are wired up (via `stb`) right now; KTX2/Basis compressed
textures aren't hooked up yet.

`gltfio::Animator` plays back whatever clips are embedded in the glTF/GLB. `main.cpp`
currently just loops clip 0.

For Mixamo-style animations retargeted onto your own character rig,
`tools/convert_character.py` automates the retarget in Blender:

```
blender --background --python tools/convert_character.py -- character.fbx animation.fbx output.glb
```

Requires Blender 4.4+ headless. Bakes the animation source's clip onto the character's
armature frame-by-frame by matching bone names, discards the animation source's own
mesh/skeleton, and exports mesh + skin + baked animation as one `.glb` with textures
re-encoded as quality-80 JPEG.

## Building an exe to share

```
cmake --build build --config Release --parallel
```

Then put `Dante.exe` and a copy of the whole `assets/` folder next to each other in one
directory and zip it:

```
Dante/
  Dante.exe
  assets/
    environments/...
    models/...
```

Dante looks for `assets/` next to the executable first, falling back to the
compile-time dev path (which won't exist on someone else's machine) only if that's
missing. This layout is what makes a build portable. On Windows nothing else needs
installing (the MSVC runtime is statically linked); the recipient just needs a GPU
with OpenGL support, which is practically any GPU from the last several years with an
up-to-date driver.

## Credit

Built using some of [Filament](https://github.com/google/filament), Google's real-time
PBR rendering engine, licensed under the Apache License 2.0.

## License

Dante's own code and assets are proprietary, all rights reserved. See
[LICENSE](LICENSE).
