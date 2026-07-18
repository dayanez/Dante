/*
 * Copyright (C) 2022 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <backend/platforms/PlatformEGLHeadless.h>

#include <utils/compiler.h>
#include <utils/Logger.h>
#include <utils/Panic.h>

#include <bluegl/BlueGL.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>

using namespace utils;

namespace filament {
using namespace backend;

PlatformEGLHeadless::PlatformEGLHeadless() noexcept
        : PlatformEGL() {
}

bool PlatformEGLHeadless::isOpenGL() const noexcept {
#if defined(BACKEND_OPENGL_VERSION_GL)
    return true;
#else
    return false;
#endif  // defined(BACKEND_OPENGL_VERSION_GL)
}

backend::Driver* PlatformEGLHeadless::createDriver(void* sharedContext,
        const Platform::DriverConfig& driverConfig) {
    auto bindApiHelper = [](EGLenum api, const char* errorString) -> bool {
        EGLBoolean bindAPI = eglBindAPI(api);
        if (UTILS_UNLIKELY(bindAPI == EGL_FALSE || bindAPI == EGL_BAD_PARAMETER)) {
            logEglError(errorString);
            eglReleaseThread();
            return false;
        };
        return true;
    };

    EGLenum api = isOpenGL() ? EGL_OPENGL_API : EGL_OPENGL_ES_API;
    const char* apiString = isOpenGL() ? "eglBindAPI EGL_OPENGL_API" : "eglBindAPI EGL_OPENGL_ES_API";
    if (!bindApiHelper(api, apiString)) {
        return nullptr;
    }

    int bindBlueGL = bluegl::bind();
    if (UTILS_UNLIKELY(bindBlueGL != 0)) {
        LOG(ERROR) << "bluegl bind failed";
        return nullptr;
    }

    return PlatformEGL::createDriver(sharedContext, driverConfig);
}

} // namespace filament
