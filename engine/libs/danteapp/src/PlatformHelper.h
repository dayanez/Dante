
#ifndef TNT_DANTEAPP_PLATFORM_HELPER_H
#define TNT_DANTEAPP_PLATFORM_HELPER_H

#include <danteapp/Config.h>

#include <dante/Engine.h>

namespace dante::backend {
class Platform;
} // namespace dante::backend

namespace dante::app {

dante::Engine::Backend resolveBackend(dante::Engine::Backend);

/**
 * Creates a Vulkan platform instance.
 * @param gpuHintCstr A string hint to choose the GPU.
 * @return A pointer to the created Vulkan platform.
 */
dante::backend::Platform* createVulkanPlatform(char const* gpuHintCstr);

/**
 * Destroys a Vulkan platform instance.
 * @param platform The platform to destroy.
 */
void destroyVulkanPlatform(dante::backend::Platform* platform);

/**
 * Creates a WebGPU platform instance.
 * @param forcedBackend The WebGPU backend to force.
 * @return A pointer to the created WebGPU platform.
 */
dante::backend::Platform* createWebGPUPlatform(Config::WebGPUBackend forcedBackend);

/**
 * Destroys a WebGPU platform instance.
 * @param platform The platform to destroy.
 */
void destroyWebGPUPlatform(dante::backend::Platform* platform);

} // namespace dante::app

#endif // TNT_DANTEAPP_PLATFORM_HELPER_H
