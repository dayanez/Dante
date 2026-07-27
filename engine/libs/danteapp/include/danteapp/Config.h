
#ifndef TNT_DANTE_SAMPLE_CONFIG_H
#define TNT_DANTE_SAMPLE_CONFIG_H

#include <dante/Engine.h>

#include <camutils/Manipulator.h>

#include <string>

struct Config {
    std::string title;
    std::string iblDirectory;
    std::string dirt;
    float scale = 1.0f;
    bool splitView = false;
    mutable dante::Engine::Backend backend = dante::Engine::Backend::DEFAULT;
    mutable dante::backend::FeatureLevel featureLevel = dante::backend::FeatureLevel::FEATURE_LEVEL_3;
    dante::camutils::Mode cameraMode = dante::camutils::Mode::ORBIT;
    // FREE_FLIGHT camera max speed in world units/second - camutils::Manipulator's own
    // default (10) if left unset.
    float cameraMoveSpeed = 10.0f;
    bool resizeable = true;
    bool headless = false;
    int stereoscopicEyeCount = 2;
    uint8_t samples = 1;

    // Indicate GPU preference for vulkan
    std::string vulkanGPUHint;


    // Note that WebGPU has its own enums for backends, but to avoid leaking webgpu headers to
    // consumers of DanteApp, we just overload the Engine::Backend enum.
    using WebGPUBackend = dante::Engine::Backend;
    // Force a backend for webgpu.
    WebGPUBackend forcedWebGPUBackend = WebGPUBackend::DEFAULT;

    // Asynchronous mode for Engine
    dante::backend::AsynchronousMode asynchronousMode = dante::backend::AsynchronousMode::NONE;
};

#endif // TNT_DANTE_SAMPLE_CONFIG_H
