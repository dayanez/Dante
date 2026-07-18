/*
 * Copyright (C) 2016 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_UTILS_GENERIC_MUTEX_H
#define TNT_UTILS_GENERIC_MUTEX_H

#include <mutex>

namespace utils {
namespace generic {

using Mutex = std::mutex;

} // namespace generic

#if !defined(UTILS_DEBUG_MUTEX) && !defined(FILAMENT_DEBUG_MUTEX)
using Mutex = generic::Mutex;
#endif

} // namespace utils

#endif // TNT_UTILS_GENERIC_MUTEX_H
