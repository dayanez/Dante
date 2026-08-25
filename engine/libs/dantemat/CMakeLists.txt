cmake_minimum_required(VERSION 3.19)

project(dantemat)

set(TARGET         dantemat)
set(PUBLIC_HDR_DIR include)

# ==================================================================================================
# Sources and headers
# ==================================================================================================
set(HDRS
        include/dantemat/Enums.h
        include/dantemat/MaterialBuilder.h
        include/dantemat/Package.h
)

set(COMMON_PRIVATE_HDRS
        src/eiff/Chunk.h
        src/eiff/ChunkContainer.h
        src/eiff/CompressedStringChunk.h
        src/eiff/DictionaryTextChunk.h
        src/eiff/Flattener.h
        src/eiff/LineDictionary.h
        src/eiff/MaterialTextChunk.h
        src/eiff/MaterialInterfaceBlockChunk.h
        src/eiff/ShaderEntry.h
        src/eiff/SimpleFieldChunk.h
        src/PushConstantDefinitions.h)

set(COMMON_SRCS
        src/eiff/Chunk.cpp
        src/eiff/ChunkContainer.cpp
        src/eiff/CompressedStringChunk.cpp
        src/eiff/DictionaryTextChunk.cpp
        src/eiff/LineDictionary.cpp
        src/eiff/MaterialTextChunk.cpp
        src/eiff/MaterialInterfaceBlockChunk.cpp
        src/eiff/SimpleFieldChunk.cpp
        src/shaders/CodeGenerator.cpp
        src/shaders/ShaderGenerator.cpp
        src/shaders/SibGenerator.cpp
        src/shaders/UibGenerator.cpp
        src/Enums.cpp
        src/MaterialBuilder.cpp
        src/MaterialVariants.cpp
)

# Sources and headers for dantemat

set(PRIVATE_HDRS
        ${COMMON_PRIVATE_HDRS}
        src/eiff/BlobDictionary.h
        src/eiff/DictionarySpirvChunk.h
        src/eiff/DictionaryMetalLibraryChunk.h
        src/eiff/MaterialBinaryChunk.h
        src/GLSLPostProcessor.h
        src/MetalArgumentBuffer.h
        src/ShaderMinifier.h
        src/SpirvFixup.h
        src/sca/ASTHelpers.h
        src/sca/GLSLTools.h
        src/sca/builtinResource.h)

set(SRCS
        ${COMMON_SRCS}
        src/eiff/BlobDictionary.cpp
        src/eiff/DictionarySpirvChunk.cpp
        src/eiff/DictionaryMetalLibraryChunk.cpp
        src/eiff/MaterialBinaryChunk.cpp
        src/MetalArgumentBuffer.cpp
        src/sca/ASTHelpers.cpp
        src/sca/GLSLTools.cpp
        src/GLSLPostProcessor.cpp
        src/ShaderMinifier.cpp
        src/SpirvFixup.cpp)

# ==================================================================================================
# Include and target definitions
# ==================================================================================================
include_directories(${PUBLIC_HDR_DIR})
include_directories(${CMAKE_BINARY_DIR})

# Dantemat
add_library(${TARGET} STATIC ${HDRS} ${PRIVATE_HDRS} ${SRCS})
target_include_directories(${TARGET} PUBLIC ${PUBLIC_HDR_DIR})
set_target_properties(${TARGET} PROPERTIES FOLDER Libs)
target_link_libraries(${TARGET} backend_headers shaders dantebridge utils smol-v zstd)

if (DANTE_SUPPORTS_WEBGPU)
    target_link_libraries(${TARGET} libtint)
endif ()

# We are being naughty and accessing private headers here
# For spirv-tools, we're just following glslang's example
target_include_directories(${TARGET} PRIVATE ${spirv-tools_SOURCE_DIR}/include)

# glslang libraries have circular dependencies. To make sure the proper object are part of the link
# we need to force archive re-scan on new symbol dependencies via start/end-group.
# Read more about this here https://eli.thegreenplace.net/2013/07/09/library-order-in-static-linking
if (MSVC)
    target_link_libraries(${TARGET} glslang SPIRV SPIRV-Tools-opt spirv-cross-glsl)
else()
    target_link_libraries(${TARGET}
            -Wl,--start-group glslang SPIRV SPIRV-Tools-opt spirv-cross-glsl -Wl,--end-group)
endif()

# ==================================================================================================
# Compiler flags
# ==================================================================================================
# this must match options enabled in glslang's CMakeLists.txt
target_compile_options(${TARGET} PRIVATE -DAMD_EXTENSIONS -DNV_EXTENSIONS )

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W0 /Zc:__cplusplus")
endif()

if (DANTE_ENABLE_MATDBG)
    add_definitions(-DDANTE_ENABLE_MATDBG)
endif()

# ==================================================================================================
# Installation
# ==================================================================================================

# Dantemat has dependencies on a bunch of SPIRV-related libraries. To make things simpler, we bundle
# them together into a single shared library and copy this into the installation folder. This
# requires us to explicitly list the dependencies below, as CMake doesn't have a way to recursively
# query dependencies.
set(DANTEMAT_DEPS
        OSDependent
        SPIRV
        SPIRV-Tools
        SPIRV-Tools-opt
        dantemat
        glslang
        spirv-cross-core
        spirv-cross-glsl
        spirv-cross-msl
        zstd
        )

set(DANTEMAT_COMBINED_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/libdantemat_combined.a")
combine_static_libs(dantemat "${DANTEMAT_COMBINED_OUTPUT}" "${DANTEMAT_DEPS}")

set(DANTEMAT_LIB_NAME ${CMAKE_STATIC_LIBRARY_PREFIX}dantemat${CMAKE_STATIC_LIBRARY_SUFFIX})
install(FILES "${DANTEMAT_COMBINED_OUTPUT}" DESTINATION lib/${DIST_DIR} RENAME ${DANTEMAT_LIB_NAME})
install(DIRECTORY ${PUBLIC_HDR_DIR}/dantemat DESTINATION include)

# ==================================================================================================
# Tests
# ==================================================================================================
if (DANTE_BUILD_TESTING AND IS_HOST_PLATFORM)
    project(test_dantemat)
    set(TARGET test_dantemat)
    set(SRCS
            tests/test_dantemat.cpp
            tests/test_argBufferFixup.cpp
            tests/test_clipDistanceFixup.cpp
            tests/test_line_dictionary.cpp)

    add_executable(${TARGET} ${SRCS})
    target_include_directories(${TARGET} PRIVATE src)
    target_link_libraries(${TARGET} dantemat gtest)
    set_target_properties(${TARGET} PROPERTIES FOLDER Tests)
endif()
