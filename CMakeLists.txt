cmake_minimum_required(VERSION 3.22)
cmake_policy(SET CMP0091 NEW)
project(Dante CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (WIN32)
    # Dante links its own libraries against the static CRT (see its add_compile_options
    # use of /MT, /MTd) and, for Debug builds, disables STL iterator debug checks for
    # compile-time reasons (/D_ITERATOR_DEBUG_LEVEL=0, see DANTE_SHORTEN_MSVC_COMPILATION
    # in engine/CMakeLists.txt). Both settings are scoped to Dante's own
    # subdirectory tree and don't reach targets created in this parent scope, so they must
    # be matched here or the two ABI-relevant settings disagree at link time (LNK2038/1319).
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    add_compile_definitions($<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=0>)
endif()

# --- Dante backend selection -------------------------------------------------------
# Dante targets Windows/Linux via OpenGL only. Metal, Vulkan, and WebGPU are dropped
# entirely - their source trees are gone from engine/Dante/backend, not just disabled
# here, so this isn't just a build option: there is nothing left to fall back to on
# another platform.
set(DANTE_SUPPORTS_OPENGL ON CACHE BOOL "" FORCE)
set(DANTE_SUPPORTS_METAL OFF CACHE BOOL "" FORCE)
set(DANTE_SUPPORTS_VULKAN OFF CACHE BOOL "" FORCE)
set(DANTE_SUPPORTS_WEBGPU OFF CACHE BOOL "" FORCE)

# Skip Dante's own ~20 sample programs (materials/, gltf_viewer/, etc.) - Dante only
# needs the danteapp *library* they share, which is patched to still build (see
# engine/libs/danteapp/CMakeLists.txt).
set(DANTE_SKIP_SAMPLES ON CACHE BOOL "" FORCE)

# Dante's own gtest unit tests and micro-benchmarks aren't relevant to this project
# and add real compile time on every build; skip them.
set(DANTE_BUILD_TESTING OFF CACHE BOOL "" FORCE)

add_subdirectory(engine)

add_executable(Dante src/dante.cpp)
target_link_libraries(Dante PRIVATE danteapp gltfio)
# Left unquoted and stringized at the use site (see DANTE_ASSETS_DIR in src/dante.cpp) rather
# than passed as a pre-quoted string literal - CMake/Ninja on Windows with a non-MSVC compiler
# (e.g. zig cc) doesn't reliably preserve the escaped inner quotes of a `-DFOO="bar"`-style
# definition through to the compiler.
target_compile_definitions(Dante PRIVATE DANTE_ASSETS_DIR=${CMAKE_SOURCE_DIR}/assets)

if (MSVC)
    # CMake's own Ninja+MSVC link rule always runs its own mt.exe merge pass after
    # linking, which conflicts with a manually-specified /MANIFESTINPUT (mt.exe ends up
    # invoked with no manifest to merge and fails with c10100a7). /MANIFEST:NO turns off
    # CMake's automatic pass so we can embed the DPI-awareness manifest ourselves below.
    # Without this, Dante.exe isn't marked DPI-aware, so Windows renders it at a
    # virtualized resolution and stretches the whole window to fit the display's scaling -
    # blurring both the 3D content and the UI uniformly.
    #
    # /MANIFEST:NO and mt.exe are MSVC-toolchain-only (mt.exe doesn't exist for a non-MSVC
    # Windows toolchain like zig cc, and its GNU-style linker driver doesn't understand
    # /MANIFEST:NO's syntax either) - Dante.exe just isn't marked DPI-aware there for now.
    target_link_options(Dante PRIVATE "/MANIFEST:NO")
    add_custom_command(TARGET Dante POST_BUILD
        COMMAND "${CMAKE_MT}" -nologo -manifest "${CMAKE_SOURCE_DIR}/src/app.manifest"
                "-outputresource:$<TARGET_FILE:Dante>;#1"
        COMMENT "Embedding DPI-awareness manifest into Dante.exe")
endif()
