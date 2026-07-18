/*
 * Copyright (C) 2019 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_UTILS_API_H
#define TNT_UTILS_API_H

#include <utils/api_level.h>

#ifdef __ANDROID__
#include <mutex>
#include <sys/system_properties.h>
#endif

namespace utils {

#ifdef __ANDROID__

uint32_t sApiLevel = 0;
std::once_flag sApiLevelOnceFlag;

int api_level() {
    std::call_once(sApiLevelOnceFlag, []() {
        char sdkVersion[PROP_VALUE_MAX];
        __system_property_get("ro.build.version.sdk", sdkVersion);
        sApiLevel = atoi(sdkVersion);
    });
    return sApiLevel;
}

#else

int api_level() {
    return 0; // API level is only supported on Android currently
}

#endif

} // namespace utils

#endif // TNT_UTILS_ARCHITECTURE_H
