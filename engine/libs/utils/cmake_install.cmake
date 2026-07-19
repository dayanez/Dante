# Install script for directory: C:/Users/dommc/Dante/engine/libs/utils

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/Dante")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "CMAKE_OBJDUMP-NOTFOUND")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/x86_64" TYPE STATIC_LIBRARY FILES "C:/Users/dommc/Dante/engine/libs/utils/utils.lib")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/utils" TYPE FILE FILES
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/getopt.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/algorithm.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/bitset.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/CallStack.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/debug.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/Allocator.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/BitmaskEnum.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/compiler.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/compressed_pair.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/CString.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/tribool.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/ImmutableCString.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/Entity.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/EntityInstance.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/EntityManager.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/FixedCapacityVector.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/Hash.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/InternPool.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/Invocable.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/Log.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/Logger.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/LruCache.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/memalign.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/MonotonicRingMap.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/Mutex.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/NameComponentManager.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/ostream.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/PagedArenaBitset.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/PagedArenaBitsetPool.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/Panic.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/Path.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/PrivateImplementation.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/PrivateImplementation-impl.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/RefCountedMap.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/SingleInstanceComponentManager.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/Slice.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/StaticString.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/Status.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/StructureOfArrays.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/Systrace.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/sstream.h"
    "C:/Users/dommc/Dante/engine/libs/utils/include/utils/unwindows.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/utils/generic" TYPE FILE FILES "C:/Users/dommc/Dante/engine/libs/utils/include/utils/generic/Mutex.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/utils/debug" TYPE FILE FILES "C:/Users/dommc/Dante/engine/libs/utils/include/utils/debug/Mutex.h")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/Users/dommc/Dante/engine/libs/utils/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
