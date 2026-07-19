
#ifndef TNT_DANTE_BACKEND_PRIVATE_DRIVERAPI_H
#define TNT_DANTE_BACKEND_PRIVATE_DRIVERAPI_H

#include <private/backend/CommandStream.h>

#include <backend/DriverApiForward.h>

#include <stddef.h>

namespace dante::backend {

inline void* allocateFromCommandStream(DriverApi& driver, size_t size, size_t alignment) noexcept {
    return driver.allocate(size, alignment);
}

} // namespace dante::backend

#endif // TNT_DANTE_BACKEND_PRIVATE_DRIVERAPI_H
