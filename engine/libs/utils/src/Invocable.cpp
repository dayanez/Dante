/*
 * Copyright (C) 2025 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <utils/Invocable.h>
#include <utils/ostream.h>

namespace utils {
io::ostream& InvocableBase::printInvocable(io::ostream& out, const char* name) {
    return out << name;
}
} // namespace utils
