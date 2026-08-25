cmake_minimum_required(VERSION 3.19)
project(dante C ASM)

set(TARGET backend)
set(PUBLIC_HDR_DIR include)
set(GENERATION_ROOT ${CMAKE_CURRENT_BINARY_DIR})

# ==================================================================================================
# Sources and headers
# ==================================================================================================
set(PUBLIC_HDRS
        include/backend/AcquiredImage.h
        include/backend/BufferDescriptor.h
        include/backend/CallbackHandler.h
        include/backend/DescriptorSetOffsetArray.h
        include/backend/DriverApiForward.h
        include/backend/DriverEnums.h
        include/backend/Handle.h
        include/backend/PipelineState.h
        include/backend/PixelBufferDescriptor.h
        include/backend/Platform.h
        include/backend/Program.h
        include/backend/SamplerDescriptor.h
        include/backend/TargetBufferInfo.h
)

set(SRCS
        src/BackendUtils.cpp
        src/BlobCacheKey.cpp
        src/Callable.cpp
        src/CallbackManager.cpp
        src/CircularBuffer.cpp
        src/CommandBufferQueue.cpp
        src/CommandStream.cpp
        src/CompilerThreadPool.cpp
        src/Driver.cpp
        src/Handle.cpp
        src/HandleAllocator.cpp
        src/JobQueue.cpp
        src/ostream.cpp
        src/noop/NoopDriver.cpp
        src/noop/PlatformNoop.cpp
        src/Platform.cpp
        src/PlatformFactory.cpp
        src/Program.cpp
)

set(PRIVATE_HDRS
        include/private/backend/CircularBuffer.h
        include/private/backend/CommandBufferQueue.h
        include/private/backend/CommandStream.h
        include/private/backend/Dispatcher.h
        include/private/backend/Driver.h
        include/private/backend/DriverApi.h
        include/private/backend/DriverAPI.inc
        include/private/backend/HandleAllocator.h
        include/private/backend/PlatformFactory.h
        src/CallbackManager.h
        src/CommandStreamDispatcher.h
        src/CompilerThreadPool.h
        src/DataReshaper.h
        src/DriverBase.h
        src/JobQueue.h
)

# ==================================================================================================
# OpenGL Sources (Windows / Linux only)
# ==================================================================================================

if (DANTE_SUPPORTS_OPENGL)
    list(APPEND SRCS
            include/backend/platforms/OpenGLPlatform.h
            src/opengl/BindingMap.h
            src/opengl/gl_headers.cpp
            src/opengl/gl_headers.h
            src/opengl/GLBufferObject.h
            src/opengl/GLDescriptorSet.cpp
            src/opengl/GLDescriptorSet.h
            src/opengl/GLDescriptorSetLayout.h
            src/opengl/GLTexture.h
            src/opengl/GLUtils.cpp
            src/opengl/GLUtils.h
            src/opengl/GLMemoryMappedBuffer.cpp
            src/opengl/GLMemoryMappedBuffer.h
            src/opengl/OpenGLBlobCache.cpp
            src/opengl/OpenGLBlobCache.h
            src/opengl/OpenGLContext.cpp
            src/opengl/OpenGLContext.h
            src/opengl/OpenGLState.cpp
            src/opengl/OpenGLState.h
            src/opengl/OpenGLDriver.cpp
            src/opengl/OpenGLDriver.h
            src/opengl/OpenGLDriverFactory.h
            src/opengl/OpenGLProgram.cpp
            src/opengl/OpenGLProgram.h
            src/opengl/OpenGLPlatform.cpp
            src/opengl/OpenGLTimerQuery.cpp
            src/opengl/OpenGLTimerQuery.h
            src/opengl/ShaderCompilerService.cpp
            src/opengl/ShaderCompilerService.h
    )
    if (EGL)
        list(APPEND SRCS src/opengl/platforms/PlatformEGL.cpp)
    endif()
    if (LINUX)
        if (DANTE_SUPPORTS_X11)
            list(APPEND SRCS src/opengl/platforms/PlatformGLX.cpp)
        elseif (DANTE_SUPPORTS_EGL_ON_LINUX)
            list(APPEND SRCS src/opengl/platforms/PlatformEGLHeadless.cpp)
        endif()
    elseif (WIN32)
        list(APPEND SRCS src/opengl/platforms/PlatformWGL.cpp)
    endif()
endif()

# ==================================================================================================
# Includes & target definition
# ==================================================================================================
# specify where our headers are
include_directories(${PUBLIC_HDR_DIR})
include_directories(src)
include_directories(${GENERATION_ROOT})

# we're building a library
add_library(${TARGET} STATIC ${PRIVATE_HDRS} ${PUBLIC_HDRS} ${SRCS})

# specify where the public headers of this library are
target_include_directories(${TARGET} PUBLIC ${PUBLIC_HDR_DIR})

# add this subproject to the Dante folder
set_target_properties(${TARGET} PROPERTIES FOLDER Dante)

# ==================================================================================================
# Expose a header-only target to minimize dependencies.
# ==================================================================================================

add_library(${TARGET}_headers INTERFACE)
target_include_directories(${TARGET}_headers INTERFACE ${PUBLIC_HDR_DIR})

# ==================================================================================================
# Dependencies
# ==================================================================================================

target_link_libraries(${TARGET} PUBLIC math)
target_link_libraries(${TARGET} PUBLIC utils)

if (DANTE_USE_ABSEIL_LOGGING)
    target_link_libraries(${TARGET} PRIVATE absl::log)
endif()

if (DANTE_SUPPORTS_OPENGL)
    target_link_libraries(${TARGET} PRIVATE bluegl)
endif()

if (LINUX)
    target_link_libraries(${TARGET} PRIVATE dl)
    if(DANTE_SUPPORTS_EGL_ON_LINUX)
       target_link_libraries(${TARGET} PUBLIC EGL)
    endif()
endif()

# ==================================================================================================
# Compiler flags
# ==================================================================================================
if(DANTE_USING_GCC)
    set(OPTIMIZATION_FLAGS
        -ffast-math
        -fno-finite-math-only
        -ffp-contract=fast
        -fvisibility-inlines-hidden
    )
else()
    set(OPTIMIZATION_FLAGS
        -ffast-math
        -fno-finite-math-only
        -ffp-contract=fast
        -fvisibility-inlines-hidden
        -fstrict-vtable-pointers
    )
endif()

set(LINUX_LINKER_OPTIMIZATION_FLAGS
        -Wl,--exclude-libs,bluegl
)

if (DANTE_USING_GCC)
    # GCC is not officially supported, so use a weaker set of warnings (incl. not using -Werror)
    set(DANTE_WARNINGS
            -Wall -Wno-unused-parameter -Wno-missing-field-initializers
            -Wdeprecated -Wnon-virtual-dtor -Wno-strict-aliasing
            -Wno-invalid-constexpr -Wno-parentheses -Wno-unused-variable
    )
else()
    set(DANTE_WARNINGS
            -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers
            -Wextra-semi -Wnewline-eof -Wdeprecated -Wundef
            -Wgnu-conditional-omitted-operand
            -Wweak-vtables -Wnon-virtual-dtor -Wclass-varargs -Wimplicit-fallthrough
            -Wover-aligned -Wreorder-init-list
            -Werror
    )
endif()

target_compile_options(${TARGET} PRIVATE
        ${DANTE_WARNINGS}
        $<$<CONFIG:Release>:${OPTIMIZATION_FLAGS}>
)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(${TARGET} PRIVATE -Wthread-safety)
endif()

# ==================================================================================================
# Linker flags
# ==================================================================================================

target_link_libraries(${TARGET} PRIVATE
        $<$<AND:$<PLATFORM_ID:Linux>,$<CONFIG:Release>>:${LINUX_LINKER_OPTIMIZATION_FLAGS}>
)

# ==================================================================================================
# Installation
# ==================================================================================================
set(INSTALL_TYPE ARCHIVE)
install(TARGETS ${TARGET} ${INSTALL_TYPE} DESTINATION lib/${DIST_DIR})
install(TARGETS ${INSTALL_TYPE} DESTINATION lib/${DIST_DIR})
install(DIRECTORY ${PUBLIC_HDR_DIR}/backend DESTINATION include)
