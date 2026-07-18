/*
 * Copyright (C) 2025 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */
#include <utils/ostream.h>
#include <utils/Status.h>

namespace utils {

utils::io::ostream& operator<<(utils::io::ostream& os, const Status& status) {
    os << "Status: ";
    switch (status.getCode()) {
        case StatusCode::OK: os << "Ok";
            break;
        case StatusCode::INVALID_ARGUMENT: os << "Invalid argument";
            break;
        case StatusCode::INTERNAL: os << "Internal error";
            break;
        case StatusCode::UNSUPPORTED: os << "Unsupported error";
            break;
    }
    os << ", with a message: " << status.getMessage();
    return os;
}
} // namespace utils
