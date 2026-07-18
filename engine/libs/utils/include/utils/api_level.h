/*
 * Copyright (C) 2019 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_UTILS_APILEVEL_H
#define TNT_UTILS_APILEVEL_H

#include <utils/compiler.h>

namespace utils {

/**
 * Returns this platform's API level. On Android this function will return
 * the API level as defined by the SDK API level version. If a platform does
 * not have an API level, this function returns 0.
 */
UTILS_PUBLIC
int api_level();

} // namespace utils

#endif // TNT_UTILS_APILEVEL_H
