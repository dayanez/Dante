/*
 * Copyright (C) 2025 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_DETAILS_SYNC_H
#define TNT_FILAMENT_DETAILS_SYNC_H

#include "downcast.h"

#include <filament/Sync.h>

#include <backend/CallbackHandler.h>
#include <backend/Handle.h>
#include <backend/Platform.h>

namespace filament {

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

FILAMENT_DOWNCAST(Sync)

} // namespace filament

#endif // TNT_FILAMENT_DETAILS_SYNC_H
