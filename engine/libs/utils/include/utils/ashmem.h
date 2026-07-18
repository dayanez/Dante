/*
 * Copyright (C) 2016 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_UTILS_ASHMEM_H
#define TNT_UTILS_ASHMEM_H

#include <stddef.h>

namespace utils {

int ashmem_create_region(const char *name, size_t size);

} // namespace utils

#endif // TNT_UTILS_ASHMEM_H
