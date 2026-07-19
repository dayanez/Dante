
#ifndef TNT_UTILS_DEBUG_MUTEX_H
#define TNT_UTILS_DEBUG_MUTEX_H

#include <utils/CallStack.h>
#include <utils/compiler.h>

#include <atomic>

#if defined(__ANDROID__)
#include <utils/linux/Mutex.h>
#else
#include <utils/generic/Mutex.h>
#endif

namespace utils::debug {

class UTILS_CAPABILITY("mutex") Mutex {
public:
    constexpr Mutex() noexcept = default;
    ~Mutex() noexcept;

    Mutex(const Mutex&) = delete;
    Mutex& operator=(const Mutex&) = delete;

    void lock() UTILS_ACQUIRE();
    bool try_lock() UTILS_TRY_ACQUIRE(true);
    void unlock() UTILS_RELEASE();

    CallStack const& getCreationStack() const noexcept;
    void ensureCreationStackCaptured() const noexcept;

#if defined(__ANDROID__)
    linuxutil::Mutex& getUnderlyingMutex() noexcept { return mUnderlying; }
    linuxutil::Mutex const& getUnderlyingMutex() const noexcept { return mUnderlying; }
#else
    generic::Mutex& getUnderlyingMutex() noexcept { return mUnderlying; }
    generic::Mutex const& getUnderlyingMutex() const noexcept { return mUnderlying; }
#endif

private:
    friend class Condition;

#if defined(__ANDROID__)
    linuxutil::Mutex mUnderlying;
#else
    generic::Mutex mUnderlying;
#endif

    mutable CallStack mCreationStack;
    mutable std::atomic<bool> mCreationCaptured = { false };
};

}

#endif // TNT_UTILS_DEBUG_MUTEX_H
