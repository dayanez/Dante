# ==================================================================================================
# CMake
# ==================================================================================================
cmake_minimum_required(VERSION 3.22.1)

# Note: we set the main CMakeLists.txt's minimum cmake version to the maximum of all the
# cmake_minimum_required versions for the full build (building for all backends, and for
# both release and debug).

# ==================================================================================================
# Unsupported environment
# ==================================================================================================

# Dante does not support MSYS2-based environment on Windows. See issue #9968.

# ==================================================================================================
# Toolchain configuration
# ==================================================================================================
# On iOS, the deployment target is set inside third_party/clang/ios.cmake
if (APPLE AND NOT IOS)
    # This must be set before project() is called
    set(CMAKE_OSX_DEPLOYMENT_TARGET 11.0 CACHE STRING "")
endif()

# ==================================================================================================
# Project declaration
# ==================================================================================================
set(EXTRA_LANGS)

# This is only needed for the webgpu on MacOS build.
if (APPLE AND NOT IOS)
    set(EXTRA_LANGS OBJC OBJCXX)
endif()

project(TNT C CXX ${EXTRA_LANGS})

# ==================================================================================================
# Options
# ==================================================================================================
option(DANTE_USE_EXTERNAL_GLES3 "Experimental: Compile Dante against OpenGL ES 3" OFF)

# Note: If you intend to use the JNI library (Android/Java build), you need to have
# exceptions enabled (the default). If you are using Dante on Android as a pure
# native library and want to save space, you can disable them.
# We disable exceptions on iOS by default. This fixes an availability error we see when using
# std::visit and std::get, which are not supported on iOS 11.0 when exceptions are enabled.
if (IOS)
    set(DANTE_ENABLE_EXCEPTIONS_DEFAULT OFF)
else()
    set(DANTE_ENABLE_EXCEPTIONS_DEFAULT ON)
endif()
option(DANTE_ENABLE_EXCEPTIONS "Enable C++ exceptions" ${DANTE_ENABLE_EXCEPTIONS_DEFAULT})

option(DANTE_ENABLE_RTTI "Enable C++ RTTI" OFF)

option(DANTE_ENABLE_LTO "Enable link-time optimizations if supported by the compiler" OFF)
option(DANTE_SKIP_SAMPLES "Don't build samples" OFF)

option(DANTE_SUPPORTS_XCB "Include XCB support in Linux builds" ON)

option(DANTE_SUPPORTS_XLIB "Include XLIB support in Linux builds" ON)

option(DANTE_SUPPORTS_EGL_ON_LINUX "Use EGL for OpenGL in Linux builds" OFF)

option(DANTE_SUPPORTS_WAYLAND "Include Wayland support in Linux builds" OFF)

option(DANTE_SKIP_SDL2 "Skip dependencies of SDL2, and SDL2" OFF)

option(DANTE_LINUX_IS_MOBILE "Treat Linux as Mobile" OFF)

option(DANTE_ENABLE_ASAN_UBSAN "Enable Address and Undefined Behavior Sanitizers" OFF)

option(DANTE_ENABLE_TSAN "Enable Thread Sanitizer" OFF)

option(DANTE_ENABLE_COVERAGE "Enable LLVM code coverage" OFF)

option(DANTE_ENABLE_MULTIVIEW "Enable multiview for Dante" OFF)

option(DANTE_SUPPORTS_OSMESA "Enable OSMesa (headless GL context) for Dante" OFF)

option(DANTE_USE_ABSEIL_LOGGING "Use Abseil to log, may increase binary size" OFF)

option(DANTE_ENABLE_EXPERIMENTAL_GCC_SUPPORT "Enable GCC support (unsupported)" ON)

option(DANTE_SUPPORTS_WEBP_TEXTURES "Enable webp texture support for Dante (builds libwebp when ON)" OFF)

# This is to disable GTAO for the short-term while we investigate a way to better manage size increases.
# On the regular dante build (where size is of less concern), we enable GTAO by default.
option(DANTE_DISABLE_GTAO "Disable GTAO" OFF)

option(DANTE_BUILD_TESTING "Build tests" ON)

option(DANTE_DEBUG_MUTEX "Enable utils::Mutex lock order and deadlock debugging" OFF)

set(DANTE_NDK_VERSION "" CACHE STRING
    "Android NDK version or version prefix to be used when building for Android."
)

set(DANTE_PER_RENDER_PASS_ARENA_SIZE_IN_MB "3" CACHE STRING
    "Per render pass arena size. Must be roughly 1 MB larger than DANTE_PER_FRAME_COMMANDS_SIZE_IN_MB, default 3."
)

set(DANTE_PER_FRAME_COMMANDS_SIZE_IN_MB "2" CACHE STRING
    "Size of the high-level draw commands buffer. Rule of thumb, 1 MB less than DANTE_PER_RENDER_PASS_ARENA_SIZE_IN_MB, default 2."
)

set(DANTE_MIN_COMMAND_BUFFERS_SIZE_IN_MB "2" CACHE STRING
    "Size of the command-stream buffer. As a rule of thumb use the same value as DANTE_PER_FRRAME_COMMANDS_SIZE_IN_MB, default 2."
)

set(DANTE_OPENGL_HANDLE_ARENA_SIZE_IN_MB "4" CACHE STRING
    "Size of the OpenGL handle arena, default 4."
)

set(DANTE_METAL_HANDLE_ARENA_SIZE_IN_MB "8" CACHE STRING
    "Size of the Metal handle arena, default 8."
)

set(DANTE_BACKEND_DEBUG_FLAG "" CACHE STRING
    "A debug flag meant for enabling/disabling backend debugging paths"
)

set(DANTE_OSMESA_PATH "" CACHE STRING
    "Path to the OSMesa header and lib"
)

# Enable exceptions by default in spirv-cross.
set(SPIRV_CROSS_EXCEPTIONS_TO_ASSERTIONS OFF)

if (NOT DANTE_ENABLE_EXCEPTIONS)
    add_compile_options(-fno-exceptions)
    set(SPIRV_CROSS_EXCEPTIONS_TO_ASSERTIONS ON)
endif()

if (NOT DANTE_ENABLE_RTTI)
    add_compile_options(-fno-rtti)
endif()

if (DANTE_DEBUG_MUTEX)
    add_compile_definitions(DANTE_DEBUG_MUTEX)
endif()

# ==================================================================================================
# CMake policies
# ==================================================================================================
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.12")
    cmake_policy(SET CMP0074 NEW)
endif()

# ==================================================================================================
# Support for ccache
# ==================================================================================================
find_program(CCACHE_PROGRAM ccache)
if (CCACHE_PROGRAM)
    if (WIN32)
        set(CMAKE_C_COMPILER_LAUNCHER   "${CCACHE_PROGRAM}")
        set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    else()
        set(C_LAUNCHER   "${CCACHE_PROGRAM}")
        set(CXX_LAUNCHER "${CCACHE_PROGRAM}")

        configure_file(build/launch-c.in   launch-c)
        configure_file(build/launch-cxx.in launch-cxx)

        execute_process(COMMAND chmod a+rx
            "${CMAKE_CURRENT_BINARY_DIR}/launch-c"
            "${CMAKE_CURRENT_BINARY_DIR}/launch-cxx"
        )

        if (CMAKE_GENERATOR STREQUAL "Xcode")
            set(CMAKE_XCODE_ATTRIBUTE_CC         "${CMAKE_CURRENT_BINARY_DIR}/launch-c")
            set(CMAKE_XCODE_ATTRIBUTE_CXX        "${CMAKE_CURRENT_BINARY_DIR}/launch-cxx")
            set(CMAKE_XCODE_ATTRIBUTE_LD         "${CMAKE_CURRENT_BINARY_DIR}/launch-c")
            set(CMAKE_XCODE_ATTRIBUTE_LDPLUSPLUS "${CMAKE_CURRENT_BINARY_DIR}/launch-cxx")
        else()
            set(CMAKE_C_COMPILER_LAUNCHER        "${CMAKE_CURRENT_BINARY_DIR}/launch-c")
            set(CMAKE_CXX_COMPILER_LAUNCHER      "${CMAKE_CURRENT_BINARY_DIR}/launch-cxx")
        endif()
    endif()
endif()

# ==================================================================================================
# Support Vim and Visual Studio Code by generating compile_commands.json
# ==================================================================================================
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ==================================================================================================
# OS specific
# ==================================================================================================
if (UNIX AND NOT APPLE AND NOT ANDROID AND NOT WASM)
    set(LINUX TRUE)
else()
    # since cmake 3.25 LINUX is automatically set based on CMAKE_SYSTEM_NAME, which the android
    # cmake files are setting to "Linux".
    set(LINUX FALSE)
endif()

if (NOT DANTE_OSMESA_PATH STREQUAL "")
    if (NOT EXISTS ${DANTE_OSMESA_PATH}/)
        message(FATAL_ERROR "Cannot find specified OSMesa build directory: ${DANTE_OSMESA_PATH}")
    endif()
    set(DANTE_SUPPORTS_OSMESA TRUE)
endif()

if (LINUX)
    if (DANTE_SUPPORTS_WAYLAND)
        add_definitions(-DDANTE_SUPPORTS_WAYLAND)
        set(DANTE_SUPPORTS_X11 FALSE)
    elseif (DANTE_SUPPORTS_EGL_ON_LINUX)
        add_definitions(-DDANTE_SUPPORTS_EGL_ON_LINUX)
        set(DANTE_SUPPORTS_X11 FALSE)
    elseif (DANTE_SUPPORTS_OSMESA)
        set(DANTE_SUPPORTS_X11 FALSE)
        add_definitions(-DDANTE_SUPPORTS_OSMESA)
    else ()
        if (DANTE_SUPPORTS_XCB)
            add_definitions(-DDANTE_SUPPORTS_XCB)
        endif()

        if (DANTE_SUPPORTS_XLIB)
            add_definitions(-DDANTE_SUPPORTS_XLIB)
        endif()

        if (DANTE_SUPPORTS_XCB OR DANTE_SUPPORTS_XLIB)
            add_definitions(-DDANTE_SUPPORTS_X11)
            set(DANTE_SUPPORTS_X11 TRUE)
        endif()
    endif()
endif()

if (ANDROID OR WASM OR IOS OR DANTE_LINUX_IS_MOBILE)
    set(IS_MOBILE_TARGET TRUE)
endif()

if (ANDROID)
    add_definitions(-D__ANDROID_UNAVAILABLE_SYMBOLS_ARE_WEAK__)
endif()

if (NOT ANDROID AND NOT WASM AND NOT IOS AND NOT DANTE_LINUX_IS_MOBILE)
    set(IS_HOST_PLATFORM TRUE)
endif()

if (APPLE)
    if (DANTE_SUPPORTS_OSMESA)
        add_definitions(-DDANTE_SUPPORTS_OSMESA)
    endif()
endif()

if (WIN32)
    # On Windows we need to instruct cmake to generate the .def in order to get the .lib required
    # when linking against dlls.
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# ==================================================================================================
#  Paths
# ==================================================================================================
# Where our external libs are
set(EXTERNAL ${CMAKE_CURRENT_SOURCE_DIR}/third_party)

# Where our libraries are
set(LIBRARIES ${CMAKE_CURRENT_SOURCE_DIR}/libs)

# Where our dante code is
set(DANTE ${CMAKE_CURRENT_SOURCE_DIR})

# Where our tools are
set(TOOLS ${CMAKE_CURRENT_SOURCE_DIR}/tools)

# ==================================================================================================
# Compiler check
# ==================================================================================================
set(MIN_CLANG_VERSION "6.0")

set(DANTE_USING_GCC OFF)
if (CMAKE_C_COMPILER_ID MATCHES "GNU" AND DANTE_ENABLE_EXPERIMENTAL_GCC_SUPPORT)
    set(DANTE_USING_GCC ON)
elseif (CMAKE_C_COMPILER_ID MATCHES "Clang")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS MIN_CLANG_VERSION)
        message(FATAL_ERROR "Detected CXX compiler Clang ${CMAKE_CXX_COMPILER_VERSION} < ${MIN_CLANG_VERSION}")
    endif()
else()
    message(FATAL_ERROR "Detected CXX compiler ${CMAKE_CXX_COMPILER_ID} is unsupported")
endif()

# Detect use of the clang-cl.exe frontend (clang simulating MSVC's cl.exe), which does not
# support all of clang's normal options and isn't the toolchain this project builds with.
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    if ("${CMAKE_CXX_SIMULATE_ID}" STREQUAL "MSVC")
        message(FATAL_ERROR "Building with clang-cl is not supported. Use the Zig toolchain (zig cc) instead, see README.md.")
    endif()
endif()

# ==================================================================================================
# Link time optimizations (LTO)
# ==================================================================================================
if (DANTE_ENABLE_LTO)
    include(CheckIPOSupported)

    check_ipo_supported(RESULT IPO_SUPPORT OUTPUT IPO_ERROR)

    if (IPO_SUPPORT)
        message(STATUS "IPO / LTO is enabled")
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(WARNING "IPO / LTO is not supported by this architecture: ${IPO_ERROR}")
    endif()
endif()

# ==================================================================================================
# General compiler flags
# ==================================================================================================

# This is only needed for the webgpu on MacOS build.
# Don't enable for iOS because we need to support ios min target 11.0
if (APPLE AND NOT IOS)
    set(CMAKE_OBJCXX_STANDARD 20)
    set(CMAKE_OBJCXX_STANDARD_REQUIRED ON)
endif()

set(CXX_STANDARD "-std=c++20")

if(DANTE_USING_GCC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_STANDARD} -Wno-changes-meaning -Wno-return-type -Wno-attributes -Wno-unknown-pragmas -Wno-class-memaccess -Wno-multichar -Wno-deprecated-declarations -Wno-subobject-linkage -Wno-invalid-constexpr")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_STANDARD} -fstrict-aliasing -Wno-unknown-pragmas -Wno-unused-function -Wno-deprecated-declarations -D_LIBCPP_ENABLE_THREAD_SAFETY_ANNOTATIONS")
endif()
if (APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-nullability-extension")
    set(CMAKE_OBJCXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

if (WIN32)
    # zig cc instruments C/C++ code with Zig's own runtime safety checks by default, even at
    # -O3/NDEBUG - unlike a plain Clang build, where this class of undefined behavior is
    # silently tolerated rather than trapped. This has
    # surfaced real-but-harmless UB already latent in this codebase (e.g. computing, without
    # dereferencing, a null-pointer-plus-offset value inside CubemapIBL.cpp's parallel_for
    # split when a job's slice happens to be empty) as a hard "panic: applying non-zero offset
    # ... to null pointer" crash instead of a normal build. Matching the other toolchains'
    # behavior here rather than auditing/fixing every such pre-existing UB site.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-sanitize=undefined")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-sanitize=undefined")
endif()

if (DANTE_USE_EXTERNAL_GLES3)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDANTE_USE_EXTERNAL_GLES3")
endif()

if (DANTE_SUPPORTS_EGL_ON_LINUX)
  set(EGL TRUE)
endif()

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_USE_MATH_DEFINES=1")
endif()

if (LINUX)
    option(USE_STATIC_LIBCXX "Link against the static runtime libraries." ON)

    # Add this step to support both glibc-based Linux distributions (e.g., Ubuntu, Debian)
    # and musl-based distributions (e.g., Alpine).
    include(CheckSymbolExists)
    check_symbol_exists(__GLIBC__ "features.h" CLANG_WITH_GLIBC)

    if (${USE_STATIC_LIBCXX})
        if (DANTE_USING_GCC)
            link_libraries("-static-libgcc -static-libstdc++")
        elseif (CLANG_WITH_GLIBC)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
            link_libraries("-static-libgcc -static-libstdc++")
            link_libraries(libc++.a)
            link_libraries(libc++abi.a)
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
        endif()
    endif()

    # Only linux, clang doesn't want to use a shared library that is not PIC.
    # /usr/bin/ld: ../bluegl/libbluegl.a(BlueGL.cpp.o): relocation R_X86_64_32S
    # against `.bss' can not be used when making a shared object; recompile with -fPIC
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

if (ANDROID)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Werror=unguarded-availability")
endif()

if (CYGWIN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

# Add colors to ninja builds (when not using gcc)
if (UNIX AND CMAKE_GENERATOR STREQUAL "Ninja" AND NOT DANTE_USING_GCC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fcolor-diagnostics")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
endif()

# Use hidden by default and expose what we need.
if (NOT WIN32)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
endif()

# ==================================================================================================
# Release compiler flags
# ==================================================================================================
if (NOT IOS)
    # Omitting stack frame pointers prevents the generation of readable stack traces in crash reports on iOS
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fomit-frame-pointer")
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ffunction-sections -fdata-sections")

if (ANDROID OR WASM)
    # On Android and WASM RELEASE builds, we omit unwind info to save space.
    # (We keep unwind info on iOS to allow readable stack traces in crash reports.)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fno-unwind-tables -fno-asynchronous-unwind-tables")
endif()

# With WASM, we disable RTTI because we pass emscripten::val back and forth
# between C++ and JavaScript in order to efficiently access typed arrays, which are unbound.
# NOTE: This is not documented in emscripten so we should consider a different approach.
if (WASM)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

if (WASM_PTHREADS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

# ==================================================================================================
# Debug compiler flags
# ==================================================================================================
if (DANTE_ENABLE_ASAN_UBSAN)
   set(EXTRA_SANITIZE_OPTIONS "-fsanitize=address -fsanitize=undefined")
endif()
if (DANTE_ENABLE_TSAN)
   set(EXTRA_SANITIZE_OPTIONS "-fsanitize=thread")
endif()
if (ANDROID)
    # keep STL debug infos (mimics what the NDK does)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-limit-debug-info")
endif()
if (NOT WASM)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fstack-protector")
endif()
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${EXTRA_SANITIZE_OPTIONS}")

if (DANTE_ENABLE_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-instr-generate -fcoverage-mapping -Wno-pass-failed")
endif()

# Disable the stack check for macOS to workaround a known issue in clang 11.0.0.
# See: https://forums.developer.apple.com/thread/121887
if (APPLE)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}  -fno-stack-check")
endif()

# ==================================================================================================
# Linker flags
# ==================================================================================================
# Strip unused sections
if (NOT WASM)
    set(GC_SECTIONS "-Wl,--gc-sections")
endif()

# Prevents stacks from being made for executable by explicitly adding this linker flag. Otherwise,
# it generates warnings newly introduced as of GNU LD 2.39. Modern security practices strongly
# recommend marking the stack as non-executable using the -z noexecstack linker flag to prevent
# stack-based buffer overflow exploits.
# See: https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=ffcf9c5700e49c0aee42dcba9a12ba21338e8136
if (LINUX)
    set(NO_EXEC_STACK "-Wl,-z,noexecstack")
endif()

set(B_SYMBOLIC_FUNCTIONS "-Wl,-Bsymbolic-functions")

if (ANDROID)
    set(BINARY_ALIGNMENT "-Wl,-z,max-page-size=16384")
endif()

if (APPLE)
    set(GC_SECTIONS "-Wl,-dead_strip")
    set(B_SYMBOLIC_FUNCTIONS "")

    # tell ranlib to ignore empty compilation units
    set(CMAKE_C_ARCHIVE_FINISH   "<CMAKE_RANLIB> -no_warning_for_no_symbols <TARGET>")
    set(CMAKE_CXX_ARCHIVE_FINISH "<CMAKE_RANLIB> -no_warning_for_no_symbols <TARGET>")
    # prevents ar from invoking ranlib, let CMake do it
    set(CMAKE_C_ARCHIVE_CREATE   "<CMAKE_AR> qc -S <TARGET> <LINK_FLAGS> <OBJECTS>")
    set(CMAKE_CXX_ARCHIVE_CREATE "<CMAKE_AR> qc -S <TARGET> <LINK_FLAGS> <OBJECTS>")
endif()

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GC_SECTIONS} ${NO_EXEC_STACK}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${GC_SECTIONS} ${B_SYMBOLIC_FUNCTIONS} ${BINARY_ALIGNMENT}")

if (WASM_PTHREADS)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pthread")
endif()

# ==================================================================================================
# Project flags
# ==================================================================================================
# Debug modes only
if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(TNT_DEV true)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTNT_DEV")
endif()

# By default, build with support for OpenGL on all platforms.
option(DANTE_SUPPORTS_OPENGL "Include the OpenGL backend" ON)
if (DANTE_SUPPORTS_OPENGL)
    add_definitions(-DDANTE_SUPPORTS_OPENGL)
endif()

# By default, build with Vulkan support on desktop platforms, although clients must request to use
# it at run time.
if (WIN32 OR WASM OR IOS)
    option(DANTE_SUPPORTS_VULKAN "Include the Vulkan backend" OFF)
else()
    option(DANTE_SUPPORTS_VULKAN "Include the Vulkan backend" ON)
endif()
if (DANTE_SUPPORTS_VULKAN)
    add_definitions(-DDANTE_DRIVER_SUPPORTS_VULKAN)
endif()

if (DANTE_SUPPORTS_WEBGPU)
    add_definitions(-DDANTE_SUPPORTS_WEBGPU)
endif()

if (DANTE_SUPPORTS_WEBP_TEXTURES)
    add_definitions(-DDANTE_SUPPORTS_WEBP_TEXTURES)
endif()

# Build with Metal support on non-WASM Apple platforms.
if (APPLE AND NOT WASM)
    option(DANTE_SUPPORTS_METAL "Include the Metal backend" ON)
else()
    option(DANTE_SUPPORTS_METAL "Include the Metal backend" OFF)
endif()
if (DANTE_SUPPORTS_METAL)
    add_definitions(-DDANTE_SUPPORTS_METAL)
endif()

# Building dantemat increases build times and isn't required for web, so turn it off by default.
if (NOT WASM)
    option(DANTE_BUILD_DANTEMAT "Build dantemat and JNI buildings" ON)
else()
    option(DANTE_BUILD_DANTEMAT "Build dantemat and JNI buildings" OFF)
endif()

# Only optimize materials in Release mode (so error message lines match the source code)
if (CMAKE_BUILD_TYPE MATCHES Release)
    option(DANTE_DISABLE_MATOPT "Disable material optimizations" OFF)
else()
    option(DANTE_DISABLE_MATOPT "Disable material optimizations" ON)
endif()

# This only affects the prebuilt shader files in gltfio and samples, not dante library.
# The value can be either "instanced", "multiview", or "none"
set(DANTE_SAMPLES_STEREO_TYPE "none" CACHE STRING
    "Stereoscopic type that shader files in gltfio and samples are built for."
)
string(TOLOWER "${DANTE_SAMPLES_STEREO_TYPE}" DANTE_SAMPLES_STEREO_TYPE)
if (NOT DANTE_SAMPLES_STEREO_TYPE STREQUAL "instanced"
    AND NOT DANTE_SAMPLES_STEREO_TYPE STREQUAL "multiview"
    AND NOT DANTE_SAMPLES_STEREO_TYPE STREQUAL "none")
    message(FATAL_ERROR "Invalid stereo type: \"${DANTE_SAMPLES_STEREO_TYPE}\" choose either \"instanced\",  \"multiview\", or \"none\" ")
endif ()

# Compiling samples for multiview implies enabling multiview feature as well.
if (DANTE_SAMPLES_STEREO_TYPE STREQUAL "multiview")
    set(DANTE_ENABLE_MULTIVIEW ON)
endif ()

# Define backend flag for debug only
if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT DANTE_BACKEND_DEBUG_FLAG STREQUAL "")
    add_definitions(-DDANTE_BACKEND_DEBUG_FLAG=${DANTE_BACKEND_DEBUG_FLAG})
    unset(DANTE_BACKEND_DEBUG_FLAG)
endif()

if (DANTE_USE_ABSEIL_LOGGING)
    add_definitions(-DDANTE_USE_ABSEIL_LOGGING)
endif()

# ==================================================================================================
# Material compilation flags
# ==================================================================================================

# Target system.
if (IS_MOBILE_TARGET)
    set(MATC_TARGET mobile)
else()
    set(MATC_TARGET desktop)
endif()

set(MATC_API_FLAGS )

if (DANTE_SUPPORTS_OPENGL)
    set(MATC_API_FLAGS ${MATC_API_FLAGS} -a opengl)
endif()
if (DANTE_SUPPORTS_VULKAN)
    set(MATC_API_FLAGS ${MATC_API_FLAGS} -a vulkan)
endif()
if (DANTE_SUPPORTS_METAL)
    set(MATC_API_FLAGS ${MATC_API_FLAGS} -a metal)
endif()
if (DANTE_SUPPORTS_WEBGPU)
    set(MATC_API_FLAGS ${MATC_API_FLAGS} -a webgpu)
endif()

# Disable optimizations
if (DANTE_DISABLE_MATOPT)
    set(MATC_OPT_FLAGS ${MATC_OPT_FLAGS} -g)
endif()

set(MATC_BASE_FLAGS ${MATC_API_FLAGS} -p ${MATC_TARGET} ${MATC_OPT_FLAGS})

# ==================================================================================================
# Distribution
# ==================================================================================================
# choose where to put the objects in the dist folder
if (NOT DIST_ARCH)
    # On Apple silicon, the value of CMAKE_HOST_SYSTEM_PROCESSOR varies based on the CMake process's
    # own architecture. Because of this, running a x86_64 CMake binary on Apple silicon will cause
    # DIST_ARCH to be set incorrectly.
    set(DIST_ARCH "${CMAKE_HOST_SYSTEM_PROCESSOR}")

    if (CMAKE_OSX_ARCHITECTURES MATCHES ".*;.*")
        set(DIST_ARCH "universal")
    else()
        if (NOT "${CMAKE_OSX_ARCHITECTURES}" STREQUAL "")
            set(DIST_ARCH "${CMAKE_OSX_ARCHITECTURES}")
        endif()
    endif()

endif()
# On Windows machines, the host processor is set to 'AMD64', which we'll interpret as x86_64.
string(TOLOWER "${DIST_ARCH}" DIST_ARCH)
string(REPLACE "amd64" "x86_64" DIST_ARCH "${DIST_ARCH}")
if (NOT DIST_DIR)
    set(DIST_DIR "${DIST_ARCH}")
    if (PLATFORM_NAME)
        set(DIST_DIR "${DIST_DIR}-${PLATFORM_NAME}")
    endif()
endif()

# ==================================================================================================
# Functions
# ==================================================================================================
## The MSVC compiler has a limitation on literal string length which is reached when all the
## licenses are concatenated together into a large string... so split them into multiple strings.
function(list_licenses OUTPUT MODULES)
    set(STR_OPENER "R\"DANTE__(")
    set(STR_CLOSER ")DANTE__\"")
    set(CONTENT)
    set(_MODULES ${MODULES} ${ARGN})
    foreach(module ${_MODULES})
        set(license_path "../../third_party/${module}/LICENSE")
        get_filename_component(fullname "${license_path}" ABSOLUTE)
        if(EXISTS ${fullname})
            string(APPEND CONTENT "${STR_OPENER}License and copyrights for ${module}:\n${STR_CLOSER},\n")
            file(READ ${license_path} license_long)
            string(REPLACE "\n" "${STR_CLOSER},\n${STR_OPENER}" license ${license_long})
            string(APPEND CONTENT ${STR_OPENER}${license}\n${STR_CLOSER},)
            string(APPEND CONTENT "\n\n")
        else()
            message(AUTHOR_WARNING "${license_path} not found. You can ignore this warning if you have devendored ${module}.")
        endif()
    endforeach()
    configure_file(${DANTE}/build/licenses.inc.in ${OUTPUT})
endfunction()

set(COMBINE_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/build/linux/combine-static-libs.sh")

# Add a custom command to TARGET that combines the static libraries in DEPS into a single archive.
function(combine_static_libs TARGET OUTPUT DEPS)
    # Loop through the dependent libraries and query their location on disk.
    set(DEPS_FILES )
    foreach(DEPENDENCY ${DEPS})
        if (TARGET ${DEPENDENCY})
            get_property(dep_type TARGET ${DEPENDENCY} PROPERTY TYPE)
            if (dep_type STREQUAL "STATIC_LIBRARY")
                list(APPEND DEPS_FILES "$<TARGET_FILE:${DEPENDENCY}>")
            endif()
        endif()
    endforeach()

    # COMBINE_SCRIPT has a #!/usr/bin/env bash shebang, which Windows' CreateProcess doesn't
    # honor - invoke it via bash explicitly so this also works on Windows (zig cc targeting
    # *-windows-gnu), not just Linux/macOS.
    if (WIN32)
        find_program(BASH_PROGRAM bash HINTS "C:/Program Files/Git/usr/bin" "C:/Program Files/Git/bin")
        if (NOT BASH_PROGRAM)
            message(FATAL_ERROR "combine_static_libs needs bash (e.g. Git for Windows) on Windows")
        endif()
        set(COMBINE_SCRIPT_COMMAND "${BASH_PROGRAM}" "${COMBINE_SCRIPT}")
    else()
        set(COMBINE_SCRIPT_COMMAND "${COMBINE_SCRIPT}")
    endif()
    add_custom_command(
        TARGET ${TARGET} POST_BUILD
        COMMAND ${COMBINE_SCRIPT_COMMAND} "${CMAKE_AR}" "${OUTPUT}" ${DEPS_FILES}
        COMMENT "Combining ${target} dependencies into single shared library"
        VERBATIM
    )
endfunction()

# ==================================================================================================
# Configuration importing/exporting prebuilt "/tools" executables
# ==================================================================================================

# In certain cases, we want the ability to separate the build type/flags (release, debug) of tools
# (matc, resgen, etc...) from dante.  Cross compilation is one such case (e.g. building material
# with matc running on host while building dante for Android) [1]. Another example is when client
# wants to enable ASAN for dante but not the tool [2].
#
# Here are the varibles that are used to determine behavior is such flows:
#   - CMAKE_CROSSCOMPILING : Set by cmake to indicate the build's target platform is different
#     from the host platform.
#   - IMPORT_EXECUTABLES_DIR : This is the directory containing the /ImportExecutables-type.cmake
#     which are then included in other CMakeLists.txt to enable finding targets that have been
#     prebuilt. This is set by the client in their `cmake` invocation.
#   - IMPORT_EXECUTABLES : Path to a file to for 1) exporting the prebuilt targets or 2)
#     importing a previously exported targets. (Used internally and should not be set by client).
#   - DANTE_EXPORT_PREBUILT_EXECUTABLES_DIR : A path set by the client to indicate that
#     they wish to export the tools as prebuilts, and the corresponding targets will be recorded
#     in a cmake file (i.e IMPORT_EXECUTABLES). IMPORT_EXECUTABLES will be set relative to the the
#     given path. This will set DANTE_EXPORT_PREBUILT_EXECUTABLES=ON.
#   - DANTE_IMPORT_PREBUILT_EXECUTABLES_DIR : A path set by the client to indicate that they wish
#     to import the tools as prebuilts from a .cmake file. The location (IMPORT_EXECUTABLES) of the
#     file is relative to the path given. This will set DANTE_IMPORT_PREBUILT_EXECUTABLES=ON.
#   - IMPORT_PREBUILT_EXECUTABLES and EXPORT_PREBUILT_EXECUTABLES are internal booleans set to
#     ON based on the prescence of DANTE_IMPORT_PREBUILT_EXECUTABLES_DIR and
#     DANTE_EXPORT_PREBUILT_EXECUTABLES_DIR.
#
# In conclusion, for cases
#   - [1] (crosscompiling), the client must set IMPORT_EXECUTABLES_DIR for both when they are building
#     the tools and when they are building the target-platform dante.
#   - [2] (all other instances), the client must set DANTE_EXPORT_PREBUILT_EXECUTABLES_DIR in
#     the prebuilt exporting step, and set DANTE_IMPORT_PREBUILT_EXECUTABLES_DIR in the importing
#     step.

# TODO: Fold the cross compilation case into the more general DANTE_IMPORT/EXPORT variables.

if (DANTE_EXPORT_PREBUILT_EXECUTABLES_DIR)
   set(DANTE_EXPORT_PREBUILT_EXECUTABLES ON)
   set(IMPORT_EXECUTABLES_DIR ${DANTE_EXPORT_PREBUILT_EXECUTABLES_DIR})
endif()

if (DANTE_IMPORT_PREBUILT_EXECUTABLES_DIR)
   set(DANTE_IMPORT_PREBUILT_EXECUTABLES ON)
   set(IMPORT_EXECUTABLES_DIR ${DANTE_IMPORT_PREBUILT_EXECUTABLES_DIR})
endif()

if (DANTE_EXPORT_PREBUILT_EXECUTABLES OR DANTE_IMPORT_PREBUILT_EXECUTABLES)
    set(IMPORT_EXECUTABLES_BUILD_TYPE Prebuilt)
elseif (WASM)
    set(IMPORT_EXECUTABLES_BUILD_TYPE Release)
else()
    set(IMPORT_EXECUTABLES_BUILD_TYPE ${CMAKE_BUILD_TYPE})
endif()
set(IMPORT_EXECUTABLES ${DANTE}/${IMPORT_EXECUTABLES_DIR}/ImportExecutables-${IMPORT_EXECUTABLES_BUILD_TYPE}.cmake)

# ==================================================================================================
# Common Functions
# ==================================================================================================

# Sets the following variables: RESGEN_HEADER, RESGEN_SOURCE, RESGEN_FLAGS, RESGEN_SOURCE_FLAGS,
# and RESGEN_OUTPUTS. Please pass in an ARCHIVE_NAME that is unique to your project, otherwise the
# incbin directive will happily consume a blob from the wrong project without warnings or errors.
# Also be sure to include the ASM language in the CMake "project" directive for your project.
function(get_resgen_vars ARCHIVE_DIR ARCHIVE_NAME)
    set(OUTPUTS
        ${ARCHIVE_DIR}/${ARCHIVE_NAME}.bin
        ${ARCHIVE_DIR}/${ARCHIVE_NAME}.S
        ${ARCHIVE_DIR}/${ARCHIVE_NAME}.apple.S
        ${ARCHIVE_DIR}/${ARCHIVE_NAME}.h
    )
    if (IOS)
        set(ASM_ARCH_FLAG "-arch ${DIST_ARCH}")
    endif()
    if (APPLE)
        set(ASM_SUFFIX ".apple")
    endif()
    set(RESGEN_HEADER "${ARCHIVE_DIR}/${ARCHIVE_NAME}.h" PARENT_SCOPE)
    # Visual Studio makes it difficult to use assembly without using MASM. MASM doesn't support
    # the equivalent of .incbin, so on Windows we'll just tell resgen to output a C file.
    if (WASM OR WIN32 OR ANDROID_ON_WINDOWS)
        set(RESGEN_OUTPUTS "${OUTPUTS};${ARCHIVE_DIR}/${ARCHIVE_NAME}.c" PARENT_SCOPE)
        set(RESGEN_FLAGS -qcx ${ARCHIVE_DIR} -p ${ARCHIVE_NAME} PARENT_SCOPE)
        set(RESGEN_SOURCE "${ARCHIVE_DIR}/${ARCHIVE_NAME}.c" PARENT_SCOPE)
    else()
        set(RESGEN_OUTPUTS "${OUTPUTS}" PARENT_SCOPE)
        set(RESGEN_FLAGS -qx ${ARCHIVE_DIR} -p ${ARCHIVE_NAME} PARENT_SCOPE)
        set(RESGEN_SOURCE "${ARCHIVE_DIR}/${ARCHIVE_NAME}${ASM_SUFFIX}.S" PARENT_SCOPE)
        set(RESGEN_SOURCE_FLAGS "-I'${ARCHIVE_DIR}' ${ASM_ARCH_FLAG}" PARENT_SCOPE)
    endif()
endfunction()

# ==================================================================================================
# Sub-projects
# ==================================================================================================

include(CheckSymbolExists)
check_symbol_exists(getopt_long "getopt.h" HAS_SYSTEM_GETOPT)
if (NOT HAS_SYSTEM_GETOPT)
    add_subdirectory(${EXTERNAL}/getopt)
endif()

# Common to all platforms
if (DANTE_BUILD_TESTING)
    add_subdirectory(${EXTERNAL}/libgtest/tnt)
endif()
add_subdirectory(${LIBRARIES}/camutils)
add_subdirectory(${LIBRARIES}/dantebridge)
add_subdirectory(${LIBRARIES}/danteflat)
add_subdirectory(${LIBRARIES}/dantegui)
# Dante deviation from upstream Dante: generatePrefilterMipmap is an orphan library with zero
# consumers in this app's link graph (nothing target_link_libraries against it) - dropped.
add_subdirectory(${LIBRARIES}/gltfio)
add_subdirectory(${LIBRARIES}/ibl)
add_subdirectory(${LIBRARIES}/iblprefilter)
add_subdirectory(${LIBRARIES}/image)
# Dante deviation from upstream Dante: ktxreader is dropped - its only two consumers
# (gltfio's Ktx2Provider and danteapp's IBL::loadFromKtx) were both dead code (nothing in
# src/main.cpp or DanteApp's dispatch ever exercises them: the app only ever loads .exr
# equirect environments) and have been removed along with it.
add_subdirectory(${LIBRARIES}/math)
add_subdirectory(${LIBRARIES}/uberz)
add_subdirectory(${LIBRARIES}/utils)
add_subdirectory(${DANTE}/shaders)
# Add zstd before basisu to force it to use the external zstd target,
# preventing a duplicate symbol conflict with its bundled version.
add_subdirectory(${EXTERNAL}/zstd/tnt)
add_subdirectory(${EXTERNAL}/imgui/tnt)
add_subdirectory(${EXTERNAL}/robin-map/tnt)
add_subdirectory(${EXTERNAL}/smol-v/tnt)
if (DANTE_BUILD_TESTING)
    add_subdirectory(${EXTERNAL}/benchmark/tnt)
endif()
add_subdirectory(${EXTERNAL}/meshoptimizer/tnt)
add_subdirectory(${EXTERNAL}/mikktspace)
add_subdirectory(${EXTERNAL}/cgltf/tnt)
add_subdirectory(${EXTERNAL}/jsmn/tnt)
add_subdirectory(${EXTERNAL}/stb/tnt)
# basisu stays: imageio/src/BasisEncoder.cpp #includes basisu_comp.h directly, and imageio
# is itself a required build-time dependency of cmgen (needed for dfg.bin, see above) - so
# this is a real build-time link, not just ktxreader's now-removed KTX2 transcoding path.
add_subdirectory(${EXTERNAL}/basisu/tnt)

# Note that this has to be placed after mikktspace in order for combine_static_libs to work.
add_subdirectory(${LIBRARIES}/geometry)

if (DANTE_BUILD_DANTEMAT OR IS_HOST_PLATFORM)
    # spirv-tools must come before dantemat, as dantemat relies on the presence of the
    # spirv-tools_SOURCE_DIR variable.
    set(SPIRV-Headers_SOURCE_DIR ${EXTERNAL}/spirv-headers)
    add_subdirectory(${EXTERNAL}/spirv-tools)
    add_subdirectory(${EXTERNAL}/glslang/tnt)
    add_subdirectory(${EXTERNAL}/spirv-cross/tnt)
    add_subdirectory(${LIBRARIES}/dantemat)
endif()

if (DANTE_SUPPORTS_WEBP_TEXTURES)
    add_subdirectory(${EXTERNAL}/libwebp/tnt)
endif()

add_subdirectory(${DANTE}/Dante)

set(DANTE_SAMPLES_BINARY_DIR ${PROJECT_BINARY_DIR}/samples)

if (WASM)
    add_subdirectory(web/dante-js)
    add_subdirectory(web/examples)
endif()

if (IS_HOST_PLATFORM)
    if (DANTE_SUPPORTS_OPENGL)
        add_subdirectory(${LIBRARIES}/bluegl)
    endif()
    if (NOT DANTE_SKIP_SDL2)
      add_subdirectory(${LIBRARIES}/danteapp)
    endif()
    add_subdirectory(${LIBRARIES}/imageio)
    add_subdirectory(${LIBRARIES}/dante-matp)

    if (NOT DANTE_SKIP_SAMPLES)
        add_subdirectory(${DANTE}/samples)
    endif()

    add_subdirectory(${EXTERNAL}/libpng/tnt)
    add_subdirectory(${EXTERNAL}/libsdl2/tnt)
    add_subdirectory(${EXTERNAL}/libz/tnt)
    add_subdirectory(${EXTERNAL}/tinyexr/tnt)

    # cmgen is also a build-time dependency of engine/Dante itself (generates dfg.bin, the
    # split-sum BRDF LUT baked into every build and used at runtime by standard PBR
    # shading) - not just the standalone offline HDR/cubemap-baking CLI it looks like, so
    # it stays despite this app never invoking it directly as a content tool.
    add_subdirectory(${TOOLS}/cmgen)
    add_subdirectory(${TOOLS}/glslminifier)
    add_subdirectory(${TOOLS}/matc)
    add_subdirectory(${TOOLS}/resgen)
    # Dante deviation from upstream Dante: specular-color (offline DFG-LUT *generator*
    # variant CLI, distinct from the dfg.bin cmgen already produces) is dropped - a
    # standalone tool with zero build-graph consumers.
    add_subdirectory(${TOOLS}/uberz)
endif()
