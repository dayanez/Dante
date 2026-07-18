/*
 * Copyright (C) 2025 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_ASYNCJOBQUEUE_H
#define TNT_ASYNCJOBQUEUE_H

#include <utils/Condition.h> // NOLINT(*-include-cleaner)
#include <utils/Invocable.h>
#include <utils/JobSystem.h>
#include <utils/Mutex.h> // NOLINT(*-include-cleaner)

#include <thread>
#include <vector>

namespace utils {

/**
 * Simple asynchronous job queue. This manages a *single*thread that executes jobs submitted
 * to it in order.
 */
class AsyncJobQueue {
public:
    using Job = Invocable<void()>;
    using Priority = JobSystem::Priority;

    // create the job queue with a name and desired priority
    AsyncJobQueue(const char* name, Priority priority);

    // drainAndExit()  must be called first
    ~AsyncJobQueue() noexcept;

    // cancel all pending jobs, but doesn't exist the thread
    void cancelAll() noexcept;

    // blocks until all jobs are executed and quits the thread
    void drainAndExit();

    // blocks until all currently queued jobs are executed
    void drain();

    // adds a job to the queue. no-op if drainAndExit() was called.
    void push(Job&& job);

    bool isValid() const noexcept;

private:
#if !defined(__EMSCRIPTEN__)
    void workerThreadLoop(const char* name, Priority priority);
    using Container = std::vector<Job>;
    std::thread mThread;
    Mutex mLock; // NOLINT(*-include-cleaner)
    Condition mCondition; // NOLINT(*-include-cleaner)
    Container mQueue UTILS_GUARDED_BY(mLock);
    bool mExitRequested UTILS_GUARDED_BY(mLock) = false;
#endif
};

} // namespace utils

#endif //TNT_ASYNCJOBQUEUE_H
