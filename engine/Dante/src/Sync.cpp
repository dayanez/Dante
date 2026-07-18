/*
 * Copyright (C) 2025 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "details/Sync.h"

#include <backend/Platform.h>

namespace filament {

void Sync::getExternalHandle(Sync::CallbackHandler* handler, Sync::Callback callback,
        void* userData) noexcept {
    downcast(this)->getExternalHandle(handler, callback, userData);
}

} // namespace filament
