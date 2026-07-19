
#include "noop/PlatformNoop.h"

#include "noop/NoopDriver.h"

namespace dante::backend {

Driver* PlatformNoop::createDriver(void* sharedGLContext, const Platform::DriverConfig& driverConfig) {
    return NoopDriver::create();
}

} // namespace dante
