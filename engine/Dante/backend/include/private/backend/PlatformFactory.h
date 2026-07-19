
//! \file

#ifndef TNT_DANTE_BACKEND_PLATFORM_FACTORY_H
#define TNT_DANTE_BACKEND_PLATFORM_FACTORY_H

#include <backend/DriverEnums.h>

#include <utils/compiler.h>

namespace dante::backend {

class Platform;

class UTILS_PUBLIC PlatformFactory  {
public:

    /**
     * Creates a Platform configured for the requested backend if available
     *
     * @param backendHint Preferred backend, if not available the backend most suitable for the
     *                    underlying platform is returned and \p backendHint is updated
     *                    accordingly. Can't be nullptr.
     *
     * @return A pointer to the Platform object.
     *
     * @see destroy
     */
    static Platform* create(backend::Backend* backendHint) noexcept;

    /**
     * Destroys a Platform object returned by create()
     *
     * @param platform a reference (as a pointer) to the Platform pointer to destroy.
     *                 \p platform is cleared upon return.
     *
     * @see create
     */
    static void destroy(Platform** platform) noexcept;
};

} // namespace dante

#endif // TNT_DANTE_BACKEND_PLATFORM_FACTORY_H
