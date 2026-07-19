
#ifndef TNT_DANTE_SYNC_H
#define TNT_DANTE_SYNC_H

#include <dante/DanteAPI.h>

#include <backend/CallbackHandler.h>
#include <backend/DriverEnums.h>
#include <backend/Platform.h>

namespace dante {

class UTILS_PUBLIC Sync : public DanteAPI {
public:
    using CallbackHandler = backend::CallbackHandler;
    using Callback = backend::Platform::SyncCallback;

    /**
     * Fetches a handle to the external, platform-specific representation of
     * this sync object.
     *
     * @param handler A handler for the callback that will receive the handle
     * @param callback A callback that will receive the handle when ready
     * @param userData Data to be passed to the callback so that the application
     *                 can identify what frame the sync is relevant to.
     * @return The external handle for the Sync. This is valid destroy() is
     *         called on this Sync object.
     */
    void getExternalHandle(CallbackHandler* handler, Callback callback, void* userData) noexcept;

protected:
    // prevent heap allocation
    ~Sync() = default;
};

} // namespace dante

#endif // TNT_DANTE_SYNC_H
