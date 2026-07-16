# Dante

A personal, code-first graphics engine and learning playground. Dante is a thin app
(`src/main.cpp`) on top of a trimmed vendor copy of Google's [Filament](https://github.com/google/filament)
(see [Credit](#credit)) — a real production-grade PBR renderer, not a toy. There's no
editor: scenes, materials, and post-FX are all written in C++ and Filament's own
material shading language, by hand. That's the point — this is a place to learn
graphics programming, not a packaged game engine.

Targets OpenGL on Windows/Linux and Metal on macOS. No Vulkan, no WebGPU, no
Android/iOS — dropped deliberately to keep this a solo-dev-sized project.

## Prerequisites

| | Windows | Linux | macOS |
|---|---|---|---|
| Compiler | MSVC (Visual Studio 2022, Desktop C++ workload) | Clang 17+ | Xcode + command line tools |
| CMake | 3.22+ | 3.22+ | 3.22+ |
| Build tool | Ninja | Ninja | Ninja |
| Graphics API | OpenGL (GPU driver only) | OpenGL (GPU driver only) | Metal (bundled with Xcode) |

No graphics SDK to install on any platform — OpenGL builds against your existing GPU
driver, Metal ships with Xcode.

- **Windows**: build from an **x64 Native Tools Command Prompt for VS 2022** (or run
  `vcvarsall.bat x64` first) — the Ninja generator needs `cl.exe`/`link.exe` on `PATH`
  and the SDK `LIB`/`INCLUDE` environment variables set; it doesn't discover MSVC on
  its own the way the Visual Studio generator does. A plain terminal that never loaded
  the VS developer environment will configure fine but fail at the final link step.
  Opening the folder directly in Visual Studio or CLion via their built-in CMake
  integration also works and handles this for you automatically.
- **Linux**: install Clang 17 (e.g. via [apt.llvm.org](https://apt.llvm.org/)), Ninja,
  and X11 dev headers: `libglu1-mesa-dev libxi-dev libxcomposite-dev libxxf86vm-dev`.
- **macOS**: `xcode-select --install` and `brew install ninja`.

## Building & running

```
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

Swap `Release` for `Debug` for an unoptimized build with debug symbols. On a
multi-core machine you can cap parallelism (`--parallel 4`) if a full-core build makes
the machine unusable to work on while it runs.

The first build compiles all of Filament and is slow (minutes, not seconds). Every
build after that is fast — Ninja only recompiles what changed.

Run the binary straight out of `build/`: `build/Dante.exe` (Windows) or `build/Dante`
(Linux/macOS). No install step, no arguments, a window opens immediately. (If you
instead configured a multi-config generator like `-G "Visual Studio 17 2022"`, the
binary lands one level deeper, in `build/Release/` or `build/Debug/`.)

## Day-to-day workflow

1. Edit `src/main.cpp`, add a `.mat` material, or drop in a new asset.
2. `cmake --build build --parallel` — seconds once the first build is done. You only
   need to re-run the `cmake -B build ...` configure step if you touch `CMakeLists.txt`
   or add/remove a source file; routine edits never need it.
3. Run the binary from `build/`. Changes to `assets/` (models, HDRIs) are picked up on
   the next launch with no rebuild — `DANTE_ASSETS_DIR` points straight at the source
   tree in dev builds, nothing gets copied.
4. Watch stderr. `[Dante] ...` lines (glTF entity/animation counts, bounding boxes on
   load) are Dante's own logging — check there first if a model loads invisibly or
   looks wrong. Filament/gltfio print their own diagnostics the same way.

**As this grows:** `main.cpp` is a single file today; once you're juggling more than a
couple of objects/lights it's worth splitting into a few files (a scene wrapper, an
input/camera controller) rather than letting one file keep growing — a normal,
small refactor, not something to plan for up front.

**Materials**: writing a `.mat` file is real shader work — Filament's material shading
language is GLSL embedded in a small config wrapper, compiled by `matc` (already part
of the build, runs automatically). See [`engine/libs/filamentapp/materials/`](engine/libs/filamentapp/materials/)
for small, real examples to copy from.

**Debugging visuals**: [RenderDoc](https://renderdoc.org/) works well against the
OpenGL backend for capturing and stepping through a single frame draw-call by
draw-call — worth reaching for once "why does this look wrong" stops being obvious
from staring at the result.

**Version control**: commit in small checkpoints as you go; `engine/` almost never
needs touching day-to-day, so diffs should mostly be `src/`, `assets/`, and new
material files.

Add `-DCMAKE_EXPORT_COMPILE_COMMANDS=ON` to the configure step once if your editor
wants a `compile_commands.json` for clangd/IntelliSense.

## Adding content

There's no asset registry — everything below means editing `src/main.cpp` directly.

**Environments (skybox / IBL)**: drop an equirectangular `.hdr` or `.exr` into
`assets/environments/` and point `Config::iblDirectory` (set in `main()`) at it.
The *visible* sky and the *lighting* are independent — the backdrop is a flat-color
`Skybox` built with `Skybox::Builder().color(...)`, while `iblDirectory` only drives
ambient light/reflections. Swap in `Skybox::Builder().environment(...)` if you want
the HDRI itself visible as the backdrop instead.

**Models**: any static or animated glTF/GLB loads through the same pattern as
`CharacterModel` in `main.cpp` (`gltfio::AssetLoader` + `ResourceLoader`, plus
`Animator` if it has animations). Copy that struct's `create()`/`destroy()` shape for
a new model, point it at `assets/models/<name>/`, and add/remove its entities from the
`Scene`. Only PNG/JPEG textures are wired up (via `stb`) right now — KTX2/Basis
compressed textures aren't hooked up yet.

**Animations**: `gltfio::Animator` plays back whatever clips are embedded in the
glTF/GLB — `main.cpp` currently just loops clip 0. For Mixamo-style animations
retargeted onto your own character rig, `tools/convert_character.py` automates the
retarget in Blender:

```
blender --background --python tools/convert_character.py -- character.fbx animation.fbx output.glb
```

Requires Blender 4.4+ headless. Bakes the animation source's clip onto the character's
armature frame-by-frame by matching bone names (plain Action reassignment doesn't
reliably rebind across armatures in 4.4+'s Action Slots system), discards the
animation source's own mesh/skeleton, and exports mesh + skin + baked animation as one
`.glb` with textures re-encoded as quality-80 JPEG.

## Sharing a build

`cmake --build build --config Release --parallel`, then put `Dante.exe` and a copy of
the whole `assets/` folder next to each other in one directory and zip it:

```
Dante/
  Dante.exe
  assets/
    environments/...
    models/...
```

Dante looks for `assets/` next to the executable first, falling back to the
compile-time dev path (which won't exist on someone else's machine) only if that's
missing — this layout is what makes a build portable. On Windows nothing else needs
installing (the MSVC runtime is statically linked); the recipient just needs a GPU
with OpenGL support (or a Mac, for Metal), which is practically any GPU from the last
several years with an up-to-date driver.

## Project layout

- `src/` — Dante's own code.
- `engine/` — vendored, trimmed [Filament](https://github.com/google/filament) (see
  [Credit](#credit)).
- `assets/` — runtime content, referenced directly from the source tree.
- `tools/` — offline content-pipeline scripts (currently: the Blender retargeting
  script above).

## Credit

Built on [Filament](https://github.com/google/filament), Google's real-time PBR
rendering engine, licensed under the Apache License 2.0.

## License

Dante's own code and assets are proprietary, all rights reserved — see
[LICENSE](LICENSE).
