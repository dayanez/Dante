/*
 * Copyright (C) 2024 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_BACKEND_SYSTRACEPROFILE_H
#define TNT_FILAMENT_BACKEND_SYSTRACEPROFILE_H

#include <private/utils/Tracing.h>

#define PROFILE_SCOPE(marker)       SYSTRACE_NAME(marker)

#define PROFILE_NAME_BEGINFRAME    "backend::beginFrame"
#define PROFILE_NAME_ENDFRAME      "backend::endFrame"

#endif // TNT_FILAMENT_BACKEND_SYSTRACEPROFILE_H

