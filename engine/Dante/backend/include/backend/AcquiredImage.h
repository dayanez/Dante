/*
 * Copyright (C) 2019 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_BACKEND_PRIVATE_ACQUIREDIMAGE_H
#define TNT_FILAMENT_BACKEND_PRIVATE_ACQUIREDIMAGE_H

#include <backend/DriverEnums.h>

#include <math/mat3.h>

namespace filament::backend {

class CallbackHandler;

// This lightweight POD allows us to bundle the state required to process an ACQUIRED stream.
// Since these types of external images need to be moved around and queued up, an encapsulation is
// very useful.

struct AcquiredImage {
    void* image = nullptr;
    backend::StreamCallback callback = nullptr;
    void* userData = nullptr;
    CallbackHandler* handler = nullptr;
};

} // namespace filament::backend

#endif // TNT_FILAMENT_BACKEND_PRIVATE_ACQUIREDIMAGE_H
