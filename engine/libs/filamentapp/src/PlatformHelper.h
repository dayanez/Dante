/*
 * Copyright (C) 2025 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENTAPP_PLATFORM_HELPER_H
#define TNT_FILAMENTAPP_PLATFORM_HELPER_H

#include <filamentapp/Config.h>

#include <filament/Engine.h>

namespace filament::backend {
class Platform;
} // namespace filament::backend

namespace filament::app {

filament::Engine::Backend resolveBackend(filament::Engine::Backend);

/**
 * Creates a Vulkan platform instance.
 * @param gpuHintCstr A string hint to choose the GPU.
 * @return A pointer to the created Vulkan platform.
 */
filament::backend::Platform* createVulkanPlatform(char const* gpuHintCstr);

/**
 * Destroys a Vulkan platform instance.
 * @param platform The platform to destroy.
 */
void destroyVulkanPlatform(filament::backend::Platform* platform);

/**
 * Creates a WebGPU platform instance.
 * @param forcedBackend The WebGPU backend to force.
 * @return A pointer to the created WebGPU platform.
 */
filament::backend::Platform* createWebGPUPlatform(Config::WebGPUBackend forcedBackend);

/**
 * Destroys a WebGPU platform instance.
 * @param platform The platform to destroy.
 */
void destroyWebGPUPlatform(filament::backend::Platform* platform);

} // namespace filament::app

#endif // TNT_FILAMENTAPP_PLATFORM_HELPER_H
