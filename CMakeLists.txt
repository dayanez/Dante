cmake_minimum_required(VERSION 3.22)
cmake_policy(SET CMP0091 NEW)
project(Dante CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

# NOTE: Dante.exe is not marked DPI-aware. Embedding the DPI-awareness manifest
# (src/app.manifest) used to be done here via MSVC's mt.exe post-build step, now removed
# along with MSVC support - zig cc's GNU-style linker has no equivalent built in. Until
# that's replaced with a toolchain-agnostic way to embed the manifest (e.g. compiling
# app.manifest into a .res with a GNU resource compiler and linking it in), Windows will
# render the window at a virtualized resolution and stretch it to fit display scaling,
# blurring both the 3D content and the UI.
