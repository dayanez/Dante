
#ifndef TNT_DANTE_DETAILS_SYNC_H
#define TNT_DANTE_DETAILS_SYNC_H

#include "downcast.h"

#include <dante/Sync.h>

#include <backend/CallbackHandler.h>
#include <backend/Handle.h>
#include <backend/Platform.h>

namespace dante {

class FEngine;

class FSync : public Sync {
public:
    FSync(FEngine& engine);

    void terminate(FEngine& engine) noexcept;

    backend::SyncHandle getHwHandle() const noexcept { return mHwSync; }

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
    void getExternalHandle(Sync::CallbackHandler* handler, Sync::Callback callback,
            void* userData) noexcept;

private:
    FEngine& mEngine;
    backend::SyncHandle mHwSync;
};

DANTE_DOWNCAST(Sync)

} // namespace dante

#endif // TNT_DANTE_DETAILS_SYNC_H
