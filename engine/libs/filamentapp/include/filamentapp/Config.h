/*
 * Copyright (C) 2016 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_SAMPLE_CONFIG_H
#define TNT_FILAMENT_SAMPLE_CONFIG_H

#include <filament/Engine.h>

#include <camutils/Manipulator.h>

#include <string>

struct Config {
    std::string title;
    std::string iblDirectory;
    std::string dirt;
    float scale = 1.0f;
    bool splitView = false;
    mutable filament::Engine::Backend backend = filament::Engine::Backend::DEFAULT;
    mutable filament::backend::FeatureLevel featureLevel = filament::backend::FeatureLevel::FEATURE_LEVEL_3;
    filament::camutils::Mode cameraMode = filament::camutils::Mode::ORBIT;
    bool resizeable = true;
    bool headless = false;
    int stereoscopicEyeCount = 2;
    uint8_t samples = 1;

    // Indicate GPU preference for vulkan
    std::string vulkanGPUHint;


    // Note that WebGPU has its own enums for backends, but to avoid leaking webgpu headers to
    // consumers of FilamentApp, we just overload the Engine::Backend enum.
    using WebGPUBackend = filament::Engine::Backend;
    // Force a backend for webgpu.
    WebGPUBackend forcedWebGPUBackend = WebGPUBackend::DEFAULT;

    // Asynchronous mode for Engine
    filament::backend::AsynchronousMode asynchronousMode = filament::backend::AsynchronousMode::NONE;
};

#endif // TNT_FILAMENT_SAMPLE_CONFIG_H
