cmake_minimum_required(VERSION 3.22)
cmake_policy(SET CMP0091 NEW)
project(Dante CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (WIN32)
    # Filament links its own libraries against the static CRT (see its add_compile_options
    # use of /MT, /MTd) and, for Debug builds, disables STL iterator debug checks for
    # compile-time reasons (/D_ITERATOR_DEBUG_LEVEL=0, see FILAMENT_SHORTEN_MSVC_COMPILATION
    # in engine/CMakeLists.txt). Both settings are scoped to Filament's own
    # subdirectory tree and don't reach targets created in this parent scope, so they must
    # be matched here or the two ABI-relevant settings disagree at link time (LNK2038/1319).
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    add_compile_definitions($<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=0>)
endif()

# --- Filament backend selection -------------------------------------------------------
# Dante targets Windows/Linux via Vulkan and macOS via Metal. OpenGL and WebGPU are
# dropped entirely (see README for rationale). This was previously forcing Vulkan ON
# unconditionally, including on Apple - macOS has no native Vulkan (would need the
# LunarG SDK's MoltenVK translation layer, which we don't want), so that would have
# broken the very first macOS CI build this file exists to support.
if (APPLE)
    set(FILAMENT_SUPPORTS_VULKAN OFF CACHE BOOL "" FORCE)
    set(FILAMENT_SUPPORTS_METAL ON CACHE BOOL "" FORCE)
else()
    set(FILAMENT_SUPPORTS_VULKAN ON CACHE BOOL "" FORCE)
    set(FILAMENT_SUPPORTS_METAL OFF CACHE BOOL "" FORCE)
endif()
set(FILAMENT_SUPPORTS_OPENGL OFF CACHE BOOL "" FORCE)
set(FILAMENT_SUPPORTS_WEBGPU OFF CACHE BOOL "" FORCE)

# Skip Filament's own ~20 sample programs (materials/, gltf_viewer/, etc.) - Dante only
# needs the filamentapp *library* they share, which is patched to still build (see
# engine/libs/filamentapp/CMakeLists.txt).
set(FILAMENT_SKIP_SAMPLES ON CACHE BOOL "" FORCE)

add_subdirectory(engine)

add_executable(Dante src/main.cpp)
target_link_libraries(Dante PRIVATE filamentapp gltfio)
target_compile_definitions(Dante PRIVATE DANTE_ASSETS_DIR="${CMAKE_SOURCE_DIR}/assets")

if (WIN32)
    # MSVC already auto-generates and embeds its own manifest resource for the exe;
    # /MANIFESTINPUT merges our DPI-awareness declaration into that instead of adding a
    # second MANIFEST resource (which fails to link with a duplicate-resource error).
    # Without this, Dante.exe isn't marked DPI-aware, so Windows renders it at a
    # virtualized resolution and stretches the whole window to fit the display's scaling -
    # blurring both the 3D content and the UI uniformly.
    target_link_options(Dante PRIVATE "/MANIFESTINPUT:${CMAKE_SOURCE_DIR}/src/app.manifest")
endif()
