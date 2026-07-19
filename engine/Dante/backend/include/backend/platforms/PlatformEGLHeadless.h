
#ifndef TNT_DANTE_DRIVER_OPENGL_PLATFORM_EGL_HEADLESS_H
#define TNT_DANTE_DRIVER_OPENGL_PLATFORM_EGL_HEADLESS_H

#include "PlatformEGL.h"

namespace dante::backend {

/**
 * A concrete implementation of OpenGLPlatform that supports EGL with only headless swapchains.
 */
class PlatformEGLHeadless : public PlatformEGL {
public:
    PlatformEGLHeadless() noexcept;

    Driver* createDriver(void* sharedContext,
            const Platform::DriverConfig& driverConfig) override;

protected:
    bool isOpenGL() const noexcept override;
};

} // namespace dante

#endif // TNT_DANTE_DRIVER_OPENGL_PLATFORM_EGL_HEADLESS_H
