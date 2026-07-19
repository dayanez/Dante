
#ifndef TNT_DANTE_BACKEND_OPENGL_OPENGLDRIVERFACTORY_H
#define TNT_DANTE_BACKEND_OPENGL_OPENGLDRIVERFACTORY_H

#include <backend/Platform.h>

namespace dante::backend {

class OpenGLPlatform;
class Driver;

class OpenGLDriverFactory {
public:
    static Driver* create(OpenGLPlatform* platform, void* sharedGLContext,
            const Platform::DriverConfig& driverConfig) noexcept;
};

} // namespace dante::backend

#endif //TNT_DANTE_BACKEND_OPENGL_OPENGLDRIVERFACTORY_H
