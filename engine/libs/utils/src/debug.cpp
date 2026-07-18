/*
 * Copyright (C) 2021 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <utils/debug.h>
#include <utils/Panic.h>

#include <cstdlib>

namespace utils {

UTILS_NORETURN
void panic(const char *func, const char * file, int line, const char *assertion) noexcept {
    PANIC_LOG("%s:%d: failed assertion '%s'\n", file, line, assertion);
    std::abort();
}

} // namespace filament
