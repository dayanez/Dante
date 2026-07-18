/*
 * Copyright (C) 2022 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef UBERZ_ARCHIVE_ENUMS_H
#define UBERZ_ARCHIVE_ENUMS_H

#include <stdint.h>

namespace filament::uberz {

    enum class ArchiveFeature : uint64_t {
        UNSUPPORTED,
        OPTIONAL,
        REQUIRED,
    };

} // namespace filament::uberz

#endif // UBERZ_ARCHIVE_ENUMS_H
