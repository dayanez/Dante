# Dante

Dante is a Scene Editor as of right now with Game Engine like features. It's built on top of Google Filaments Graphics Rendering Engine. It features the ability to load sky boxes, models and scenes. It allows user to save and load scenes. Offers ability to plug in play add models and options to actually move the model. There are also options to adjust lighting and post processing effects via a slider. This is a work in progress. Currently doing this as more of a passion project to practice my graphics programming and systems programming. The stack is currently just using Google Filaments backend for OpenGL and dropped Vulkan DirectX and Metal. It compiles and runs as of right now only on Windows and Linux. I do plan on maybe making this into a game engine in the future. Not too sure. It's license is currently closed off on proprietary code but Google Filaments code is Apache so it's free use.  

#Current Perfromance
On a Nvidia 5060Ti it currently runs at 61 fps. Working on optimizations so it isnt capped and can run at more frames on lower systems. 

# Progress Pics
On My Google Drive just some pics of what it looks like 
https://drive.google.com/drive/folders/1xEvbrjaccKw1PnfxGb-r7AvfkoVATH2x?usp=sharing

## First-time install & build

Windows: two ways to build now, pick one.

**Option A - Zig (recommended, lightweight, no Visual Studio needed):**

Install these (all small, no installer/IDE needed):
- Zig: https://ziglang.org/download/ (put it on PATH)
- CMake 3.22+: https://cmake.org/download/
- Ninja: https://github.com/ninja-build/ninja/releases (or `winget install Kitware.CMake Ninja-build.Ninja`)
- Python 3: https://www.python.org/downloads/ (needed at configure time by spirv-tools' codegen)
- Git for Windows (for the `bash` shell used below): https://git-scm.com/download/win

CMake can't call `zig cc`/`zig c++`/`zig ar` directly (they're subcommands, not standalone
exes), so build the small wrapper stubs once first:

```
bash engine/build/windows-gnu/build-wrappers.sh
```

Then configure and build (Release is required - see the note the script prints):

```
cmake -S . -B build -G Ninja ^
    -DCMAKE_C_COMPILER="engine/build/windows-gnu/bin/zigcc.exe" ^
    -DCMAKE_CXX_COMPILER="engine/build/windows-gnu/bin/zigcxx.exe" ^
    -DCMAKE_AR="engine/build/windows-gnu/bin/zigar.exe" ^
    -DCMAKE_RANLIB="engine/build/windows-gnu/bin/zigranlib.exe" ^
    -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

**Option B - MSVC (the original way):** Ideally have Microsoft Visual Studio. I recommend
downloading the full integrated development enviroment, but if you are inclinded not to just
simply download the Visual Studio dev build tools. It can be found here
https://visualstudio.microsoft.com/.

```
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

Linux: Now on linux it requires Clang 17+, CMake 3.22+, Ninja, and X11 dev headers
(`libglu1-mesa-dev libxi-dev libxcomposite-dev libxxf86vm-dev`).

```
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

I would also run the `--build` flag with `--parallel 4` (or whatever number) instead of bare
`--parallel` so it doesn't max out every core on your machine, that's optional though.

## Every time after (rebuild & run)

```
cmake --build build --parallel
build\Dante.exe        # Windows
build/Dante             # Linux
```

## Building a shareable exe
To send this to someone or to share it use this ideally. Build it then add a Release and then follow the below. 
```
cmake --build build --config Release --parallel
```

Put `Dante.exe` and the whole `assets/` folder next to each other and zip:

```
Dante/
  Dante.exe
  assets/
```

Dante looks for `assets/` next to the executable. No install step for the recipient;
they just need a GPU or a Laptop with some sort of Graphics capabilities and with an up-to-date OpenGL driver for better performance.



Check License for more info. But in short Filament code free use not mine. Now Editor and General Dante code is mine you are not allowed to use it unless I give explicit permission. This isn't a open source project and probably wont be. If you are looking for that you wont find it here thank you. 
