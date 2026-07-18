/*
 * Copyright (C) 2019 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ostream_.h"

#include <utils/ostream.h>
#include <utils/sstream.h>

namespace utils::io {

ostream& sstream::flush() noexcept {
    // no-op.
    return *this;
}

const char* sstream::c_str() const noexcept {
    char const* buffer = getBuffer().get();
    return buffer ? buffer : "";
}

size_t sstream::length() const noexcept {
    return getBuffer().length();
}

} // namespace utils::io
