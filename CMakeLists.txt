cmake_minimum_required(VERSION 3.22)
project(Dante CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Filament backend selection -------------------------------------------------------
# Dante targets Windows/Linux via Vulkan and macOS via Metal. OpenGL and WebGPU are
# dropped entirely (see README for rationale).
set(FILAMENT_SUPPORTS_VULKAN ON CACHE BOOL "" FORCE)
set(FILAMENT_SUPPORTS_OPENGL OFF CACHE BOOL "" FORCE)
set(FILAMENT_SUPPORTS_WEBGPU OFF CACHE BOOL "" FORCE)

# Skip Filament's own ~20 sample programs (materials/, gltf_viewer/, etc.) - Dante only
# needs the filamentapp *library* they share, which is patched to still build (see
# third_party/filament/libs/filamentapp/CMakeLists.txt).
set(FILAMENT_SKIP_SAMPLES ON CACHE BOOL "" FORCE)

add_subdirectory(third_party/filament)

add_executable(Dante src/main.cpp)
target_link_libraries(Dante PRIVATE filamentapp)
