/*
 * Copyright (C) 2016 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_UTILS_ARCHITECTURE_H
#define TNT_UTILS_ARCHITECTURE_H

#include <stddef.h>

namespace utils {

constexpr size_t CACHELINE_SIZE = 64;

namespace arch {

size_t getPageSize() noexcept;

} // namespace arch
} // namespace utils

#endif // TNT_UTILS_ARCHITECTURE_H
