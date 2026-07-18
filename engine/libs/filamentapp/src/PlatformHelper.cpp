/*
 * Copyright (C) 2025 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "PlatformHelper.h"

#include <backend/Platform.h>
#if defined(FILAMENT_DRIVER_SUPPORTS_VULKAN)
#include <backend/platforms/VulkanPlatform.h>
#endif
#if defined(FILAMENT_SUPPORTS_WEBGPU)
#include <backend/platforms/WebGPUPlatform.h>
#endif

#include <utils/CString.h>

#include <algorithm>
#include <cstdlib>

namespace filament::app {

using namespace filament::backend;

filament::Engine::Backend resolveBackend(filament::Engine::Backend backend) {
    if (backend == Engine::Backend::DEFAULT) {
        // This mirrors the logic for choosing a backend given compile-time flags and client having
        // provided DEFAULT as the backend (see PlatformFactory.cpp)
#if defined(FILAMENT_IOS) || defined(__APPLE__)
        backend = Engine::Backend::METAL;
#elif defined(__EMSCRIPTEN__) || defined(__ANDROID__)
        backend = Engine::Backend::OPENGL;
#elif defined(FILAMENT_DRIVER_SUPPORTS_VULKAN)
        backend = Engine::Backend::VULKAN;
#elif defined(FILAMENT_DRIVER_SUPPORTS_WEBGPU)
        backend = Engine::Backend::WEBGPU;
#endif
    }
    return backend;
}


#if defined(FILAMENT_DRIVER_SUPPORTS_VULKAN)
void destroyVulkanPlatform(filament::backend::Platform* platform) {
    delete static_cast<VulkanPlatform*>(platform);
}
#else
filament::backend::Platform* createVulkanPlatform(char const* gpuHintCstr) { return nullptr; }
void destroyVulkanPlatform(filament::backend::Platform* platform) {}
#endif

#if defined(FILAMENT_SUPPORTS_WEBGPU)
void destroyWebGPUPlatform(filament::backend::Platform* platform) {
    delete static_cast<WebGPUPlatform*>(platform);
}
#else
filament::backend::Platform* createWebGPUPlatform(Config::WebGPUBackend forcedBackend) {
    return nullptr;
}
void destroyWebGPUPlatform(filament::backend::Platform* platform) {}
#endif

} // namespace filament::app
