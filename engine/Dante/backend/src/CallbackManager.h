
#ifndef TNT_DANTE_BACKEND_CALLBACKMANAGER_H
#define TNT_DANTE_BACKEND_CALLBACKMANAGER_H

#include <backend/CallbackHandler.h>

#include <utils/compiler.h>
#include <utils/Mutex.h>

#include <atomic>
#include <list>
#include <mutex>

namespace dante::backend {

class DriverBase;
class CallbackHandler;

/*
 * CallbackManager schedules user callbacks once all previous conditions are met.
 * A "Condition" is created by calling "get" and is met by calling "put". These
 * are typically called from different threads.
 * The callback is specified with "setCallback", which atomically creates a new set of
 * conditions to be met.
 */
class CallbackManager {
    struct Callback {
        mutable std::atomic_int count{};
        CallbackHandler* handler = nullptr;
        CallbackHandler::Callback func = {};
        void* user = nullptr;
    };

    using Container = std::list<Callback>;

public:
    using Handle = Container::const_iterator;

    explicit CallbackManager(DriverBase& driver);

    ~CallbackManager() noexcept;

    // Calls all the pending callbacks regardless of remaining conditions to be met. This is to
    // avoid leaking resources for instance. It also doesn't matter if the conditions are met
    // because we're shutting down.
    void terminate() noexcept;

    // creates a condition and get a handle for it
    Handle get() const noexcept;

    // Announces the specified condition is met. If a callback was specified and all conditions
    // prior to setting the callback are met, the callback is scheduled.
    void put(Handle& curr) noexcept;

    // Sets a callback to be called when all previously created (get) conditions are met (put).
    // If there were no conditions created, or they're all already met, the callback is scheduled
    // immediately.
    void setCallback(CallbackHandler* handler, CallbackHandler::Callback func, void* user);

private:
    Container::const_iterator getCurrent() const noexcept {
        utils::LockGuard const lock(mLock);
        return --mCallbacks.end();
    }

    Container::iterator allocateNewSlot() {
        utils::LockGuard const lock(mLock);
        auto curr = --mCallbacks.end();
        mCallbacks.emplace_back();
        return curr;
    }
    void destroySlot(Container::const_iterator curr) noexcept {
        utils::LockGuard const lock(mLock);
        mCallbacks.erase(curr);
    }

    DriverBase& mDriver;
    mutable utils::Mutex mLock;
    Container mCallbacks UTILS_GUARDED_BY(mLock);
};

} // namespace dante::backend

#endif // TNT_DANTE_BACKEND_CALLBACKMANAGER_H
