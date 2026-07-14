# Dante

A lean, cross-platform (Windows / Linux / macOS), graphics-first rendering engine.
Built on top of [Filament](https://github.com/google/filament), Google's real-time
physically based rendering engine, trimmed down to just the pieces needed for a
windowed PBR renderer: skybox/IBL, glTF loading with skeletal animation, a movable
camera, and bloom/SSAO post-processing — without the platforms, samples, tests, and
tooling Dante doesn't use.

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

## Status

Early scaffolding — see the project plan for the current milestone.

## Building

Requires CMake 3.22+, Ninja, a C++20 compiler (MSVC on Windows, Clang 17+ on
Linux/macOS), and the Vulkan SDK (Windows/Linux) or Xcode (macOS, for Metal).

```
cmake -B build -G Ninja
cmake --build build
```
