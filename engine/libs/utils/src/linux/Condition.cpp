/*
 * Copyright (C) 2018 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "futex.h"

#include <utils/linux/Condition.h>

namespace utils {

int Condition::futex_wait(uint32_t old_state, bool realtimeClock, struct timespec* ts) noexcept {
    return linuxutil::futex_wait_ex(&mState, false, old_state, realtimeClock, ts);
}

std::cv_status Condition::wait_until(linuxutil::Mutex* lock,
        bool realtimeClock, struct timespec* ts) noexcept UTILS_NO_THREAD_SAFETY_ANALYSIS {
    if (ts && ts->tv_sec < 0) {
        return std::cv_status::timeout;
    }
    uint32_t old_state = mState.load(std::memory_order_relaxed);
    lock->unlock();
    int status = linuxutil::futex_wait_ex(&mState, false, old_state, realtimeClock, ts);
    lock->lock();
    return (status == -ETIMEDOUT) ? std::cv_status::timeout : std::cv_status::no_timeout;
}

void Condition::pulse(int threadCount) noexcept {
    mState.fetch_add(1, std::memory_order_relaxed);
    linuxutil::futex_wake_ex(&mState, false, threadCount);
}

} // namespace utils
