/*
 * Copyright (C) 2019 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <backend/PresentCallable.h>

#include <utils/debug.h>
#include <utils/Panic.h>

namespace filament::backend {

PresentCallable::PresentCallable(PresentFn fn, void* user) noexcept
    : mPresentFn(fn), mUser(user) {
    assert_invariant(fn != nullptr);
}

void PresentCallable::operator()(bool presentFrame) noexcept {
    FILAMENT_CHECK_PRECONDITION(mPresentFn) << "This PresentCallable was already called. "
                                               "PresentCallables should be called exactly once.";
    mPresentFn(presentFrame, mUser);
    // Set mPresentFn to nullptr to denote that the callable has been called.
    mPresentFn = nullptr;
}

} // namespace filament::backend
