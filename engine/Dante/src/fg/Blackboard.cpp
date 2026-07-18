/*
 * Copyright (C) 2021 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "fg/Blackboard.h"

#include <string_view>

namespace filament {

Blackboard::Blackboard() noexcept = default;

Blackboard::~Blackboard() noexcept = default;

FrameGraphHandle Blackboard::getHandle(std::string_view const name) const noexcept {
    auto it = mMap.find(name);
    if (it != mMap.end()) {
        return it->second;
    }
    return {};
}

FrameGraphHandle& Blackboard::operator [](std::string_view const name) noexcept {
    auto[pos, _] = mMap.insert_or_assign(name, FrameGraphHandle{});
    return pos->second;
}

void Blackboard::put(std::string_view const name, FrameGraphHandle const handle) noexcept {
    operator[](name) = handle;
}


void Blackboard::remove(std::string_view const name) noexcept {
    mMap.erase(name);
}

} // namespace filament
