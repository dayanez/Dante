# Dante

This was a weekend project. Now this is my hobby. 
Its a graphics rendering engine built on top of some of filaments math, and their MATC compiler.

# Progress Pics
On My Google Drive just some pics of what it looks like 
https://drive.google.com/drive/folders/1xEvbrjaccKw1PnfxGb-r7AvfkoVATH2x?usp=sharing

## First-time install & build

Windows: open an **x64 Native Tools Command Prompt for VS 2022** (plain PowerShell/cmd
won't have `cl.exe` on `PATH`). Requires Visual Studio 2022 (Desktop C++ workload),
CMake 3.22+, and Ninja.

Linux: requires Clang 17+, CMake 3.22+, Ninja, and X11 dev headers
(`libglu1-mesa-dev libxi-dev libxcomposite-dev libxxf86vm-dev`).

```
cd C:\Users\dommc\Dante
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

## Every time after (rebuild & run)

```
cmake --build build --parallel
build\Dante.exe        # Windows
build/Dante             # Linux
```

Only re-run the `cmake -B build ...` configure step if you touch `CMakeLists.txt` or
add/remove a source file.

## Building a shareable exe

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
they just need a GPU with an up-to-date OpenGL driver.



Check License
