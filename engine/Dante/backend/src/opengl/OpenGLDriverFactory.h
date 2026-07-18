/*
 * Copyright (C) 2019 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_BACKEND_OPENGL_OPENGLDRIVERFACTORY_H
#define TNT_FILAMENT_BACKEND_OPENGL_OPENGLDRIVERFACTORY_H

#include <backend/Platform.h>

namespace filament::backend {

class OpenGLPlatform;
class Driver;

class OpenGLDriverFactory {
public:
    static Driver* create(OpenGLPlatform* platform, void* sharedGLContext,
            const Platform::DriverConfig& driverConfig) noexcept;
};

} // namespace filament::backend

#endif //TNT_FILAMENT_BACKEND_OPENGL_OPENGLDRIVERFACTORY_H
