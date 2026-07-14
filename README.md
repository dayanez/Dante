# Dante

A lean, cross-platform (Windows / Linux / macOS), graphics-first rendering engine.
Built on top of [Filament](https://github.com/google/filament), Google's real-time
physically based rendering engine, trimmed down to just the pieces needed for a
windowed PBR renderer: skybox/IBL, glTF loading with skeletal animation, a movable
camera, and bloom/SSAO post-processing — without the platforms, samples, tests, and
tooling Dante doesn't use.

## Status

Early scaffolding — see the project plan for the current milestone. There's no
data-driven asset list, editor, or scene format yet: `src/main.cpp` is one hand-written
file that sets up the camera, skybox, ground plane, and a single animated character.
Adding content today means editing that file, not dropping files into a folder (see
[Adding content](#adding-content) below).

## Building

### Prerequisites

| | Windows | Linux | macOS |
|---|---|---|---|
| Compiler | MSVC (Visual Studio 2022, Desktop C++ workload) | Clang 17+ | Xcode + command line tools |
| CMake | 3.22+ | 3.22+ | 3.22+ |
| Build tool | Ninja | Ninja | Ninja |
| Graphics API | Vulkan SDK | Vulkan SDK | Metal (bundled with Xcode) |

- **Windows**: install the [Vulkan SDK](https://vulkan.lunarg.com/) (its installer sets
  `VULKAN_SDK` for you), then build from an **x64 Native Tools Command Prompt for VS
  2022** (or run `vcvarsall.bat x64` first) — the Ninja generator needs `cl.exe` on
  `PATH`, it doesn't discover MSVC on its own the way the Visual Studio generator does.
  Opening the folder directly in Visual Studio or CLion via their built-in CMake
  integration also works and manages this for you automatically.
- **Linux**: install Clang 17 (e.g. via [apt.llvm.org](https://apt.llvm.org/)), Ninja,
  the Vulkan SDK, and X11 dev headers: `libglu1-mesa-dev libxi-dev libxcomposite-dev
  libxxf86vm-dev`. See `.github/workflows/build.yml` for the exact package list CI uses.
- **macOS**: `xcode-select --install` and `brew install ninja`. No Vulkan needed —
  Dante builds against Metal here.

### Compiling

```
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

This produces `build/Dante.exe` (Windows) or `build/Dante` (Linux/macOS). Run it
directly — there's no install step. Swap `Release` for `Debug` for an unoptimized
build with debug symbols; everything else about the command is identical.

## Daily workflow

The first configure+build is slow (Filament is a lot of C++). Every build after that
is fast, because Ninja only recompiles what changed:

1. Edit `src/main.cpp` (or whatever you're working on).
2. `cmake --build build --parallel` — seconds, not minutes, once the first build is done.
3. Run `build/Dante.exe` (or `build/Dante` on Linux/macOS) straight from the build
   folder. Assets load from the source tree, not a copy in `build/` — `DANTE_ASSETS_DIR`
   points straight at `assets/` in dev builds, so editing a model or HDRI and
   relaunching picks it up immediately, no rebuild, no copying.
4. Watch stderr. `[Dante] ...` lines (glTF entity/animation counts, bounding boxes on
   load) are Dante's own logging — check there first if a model loads invisibly or
   looks wrong. Filament/gltfio print their own diagnostics the same way.
5. Only reconfigure (step 0, re-run the `cmake -B build ...` command) when you touch
   `CMakeLists.txt` or add/remove a source file — routine edits never need it.

You never need to reconfigure just to test a change to `assets/` or `src/main.cpp`.
Add `-DCMAKE_EXPORT_COMPILE_COMMANDS=ON` to the configure step once if your editor
wants a `compile_commands.json` for clangd/IntelliSense.

## Sharing a build

To hand someone a working copy of Dante without them building it themselves:

1. `cmake --build build --config Release --parallel` (or reconfigure with
   `-DCMAKE_BUILD_TYPE=Release` first if your existing `build/` is a Debug config).
2. Make a folder containing `build/Dante.exe` **and** a copy of the whole `assets/`
   folder next to it (same directory, not nested):
   ```
   Dante/
     Dante.exe
     assets/
       environments/...
       models/...
   ```
   Dante checks for an `assets/` folder next to the executable first, and only falls
   back to the compile-time dev path (your machine's source tree, which won't exist on
   theirs) if that's missing — so this layout is what makes it portable.
3. Zip that folder and send it. On Windows nothing else needs installing — the MSVC
   runtime is statically linked in. The recipient does need a GPU with Vulkan support
   (Windows/Linux) or a Mac (Metal) — practically any GPU from the last several years
   with an up-to-date driver already has the Vulkan loader, so this is rarely an issue
   in practice, but it's the one real requirement.

## Adding content

There's no asset registry yet — everything below means editing `src/main.cpp`.

### Environments (skybox / IBL)

Drop an equirectangular `.hdr` or `.exr` into `assets/environments/` and point
`Config::iblDirectory` (set in `main()`) at it. `FilamentApp` filters a single
equirect image into a real-time IBL at load — no offline baking step required. (It
also accepts a pre-baked `cmgen` output directory instead, if you want faster
startup for a fixed environment, but that's not necessary to get going.)

Note the *visible* sky and the *lighting* are independent: the lit background is a
flat-color `Skybox` built in `main()` (`Skybox::Builder().color(...)`), while
`iblDirectory` only drives ambient light/reflections. Swap in
`Skybox::Builder().environment(...)` if you want the HDRI itself visible as the
backdrop instead of a flat color.

### Models

Any static or animated glTF/GLB asset loads through the same pattern as
`CharacterModel` in `main.cpp` (`gltfio::AssetLoader` + `ResourceLoader` +, if the
asset has animations, `Animator`). To add another model, copy that struct's
`create()`/`destroy()` shape, point it at a new file under `assets/models/<name>/`,
and add/remove its entities from the `Scene` the same way. Only PNG/JPEG textures are
wired up via `stb` right now — KTX2/Basis-compressed textures aren't hooked up.

### Animations

`gltfio::Animator` plays back whatever animation clips are embedded in the glTF/GLB —
`main.cpp` currently just loops clip 0. For Mixamo-style humanoid animations retargeted
onto your own character rig, `tools/convert_character.py` automates the retarget in
Blender:

```
blender --background --python tools/convert_character.py -- character.fbx animation.fbx output.glb
```

Requires Blender 4.4+ headless. It imports both FBX files, bakes the animation
source's clip onto the character's armature frame-by-frame by matching bone names
(plain Action reassignment doesn't reliably rebind across armatures in 4.4+'s Action
Slots system), discards the animation source's own mesh/skeleton, and exports the
character's mesh + skin + baked animation as one `.glb` with textures re-encoded as
quality-80 JPEG (keeps file size sane — the source Mixamo PNGs are often 4K and
lossless).

## Project layout

- `src/` — Dante's own code.
- `engine/` — vendored, trimmed [Filament](https://github.com/google/filament)
  (see [Credit](#credit)).
- `assets/` — runtime content, referenced directly from the source tree.
- `tools/` — offline content-pipeline scripts (currently: the Blender retargeting
  script above).
- `.github/workflows/` — CI matrix that verifies the CMake/compiler setup builds
  cleanly on Windows, Linux, and macOS.

## Credit

Dante vendors a trimmed copy of [google/filament](https://github.com/google/filament)
(pinned to `v1.74.0`) under `engine/`, licensed under the
[Apache License 2.0](engine/LICENSE). All credit for the rendering core,
`gltfio`, `camutils`, `filagui`, and `filamentapp` goes to Google and the Filament
contributors. See `engine/AUTHORS` and the original project for details.
This is not a fork of Filament — Dante is a separate project with its own history that
imports Filament's source as a vendored, trimmed dependency.

## License

Dante's own code and assets are proprietary — all rights reserved, see
[LICENSE](LICENSE). The vendored copy of Filament under `engine/`
remains under its own Apache License 2.0 regardless (see Credit above); that
license can't be narrowed or superseded by Dante's.
