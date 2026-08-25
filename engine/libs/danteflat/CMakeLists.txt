cmake_minimum_required(VERSION 3.19)
project(danteflat)

set(TARGET         danteflat)
set(PUBLIC_HDR_DIR include)

# ==================================================================================================
# Sources and headers
# ==================================================================================================
file(GLOB_RECURSE HDRS include/danteflat/*.h)

set(SRCS
        src/ChunkContainer.cpp
        src/DictionaryReader.cpp
        src/MaterialChunk.cpp
        src/Unflattener.cpp)

# ==================================================================================================
# Include and target definitions
# ==================================================================================================
include_directories(${PUBLIC_HDR_DIR})

add_library(${TARGET} ${HDRS} ${SRCS})
target_include_directories(${TARGET} PUBLIC ${PUBLIC_HDR_DIR})
set_target_properties(${TARGET} PROPERTIES FOLDER Libs)

target_link_libraries(${TARGET} dantebridge utils)

if (DANTE_SUPPORTS_VULKAN)
    target_link_libraries(${TARGET} smol-v)
endif()

# ==================================================================================================
# Installation
# ==================================================================================================
# We do not need danteflat headers in the install directory
# install(DIRECTORY ${PUBLIC_HDR_DIR}/danteflat DESTINATION include)
install(TARGETS ${TARGET} ARCHIVE DESTINATION lib/${DIST_DIR})

# ==================================================================================================
# Tests
# ==================================================================================================
if (DANTE_BUILD_TESTING AND IS_HOST_PLATFORM)
    project(test_danteflat)
    set(TARGET test_danteflat)
    set(SRCS
            tests/test_danteflat.cpp)

    add_executable(${TARGET} ${SRCS})
    target_include_directories(${TARGET} PRIVATE src)
    target_link_libraries(${TARGET} danteflat gtest)
    set_target_properties(${TARGET} PROPERTIES FOLDER Tests)
endif()
