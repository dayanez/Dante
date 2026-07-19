
#ifndef TNT_DANTE_DETAILS_ASYNCHELPERS_H
#define TNT_DANTE_DETAILS_ASYNCHELPERS_H

#include <private/backend/Driver.h>

#include <backend/CallbackHandler.h>

#include <functional>

namespace dante {

using namespace utils;

// This acts as an adapter that bridges a user-provided callback with the specific requirements of
// the Dante callback system.
// E.g.
//   The callback that users expect => std::function<void(Texture*, void*)>
//   Dante callback system requirement => CallbackHandler::Callback* callback, void* user
template<typename T>
class CallbackAdapter {
public:
    using UserCallback = std::function<void(T*, void*)>;

    template<typename... Args>
    static CallbackAdapter<T>* make(Args&&... args) {
        return new (std::nothrow) CallbackAdapter<T>(std::forward<Args>(args)...);
    }

    CallbackAdapter(UserCallback&& callback, const T* param, void* custom)
        : mUserCallback(std::move(callback)), mUserParam1(param), mUserParam2(custom) {}

    static void func(void* user) {
        auto* const thisObject = static_cast<CallbackAdapter*>(user);
        thisObject->mUserCallback(const_cast<T*>(thisObject->mUserParam1), thisObject->mUserParam2);
        delete thisObject;
    }

private:
    UserCallback mUserCallback;
    const T* mUserParam1;
    void* mUserParam2;
};

// This specialized class tracks multiple asynchronous operations. Before invoking the user-provided
// callback, it waits for all tasks to complete. Users are required to manually update the countdown
// for each async call they make.
template<typename T>
class CountdownCallbackHandler : public backend::CallbackHandler {
public:
    using UserCallback = std::function<void(T*, void*)>;
    using CountdownCompleteCallback = std::function<void()>;

    template<typename... Args>
    static CountdownCallbackHandler<T>* make(Args&&... args) {
        return new (std::nothrow) CountdownCallbackHandler<T>(std::forward<Args>(args)...);
    }

    CountdownCallbackHandler(CallbackHandler* handler, UserCallback&& userCallback, T* userParam1,
            void* userParam2, CountdownCompleteCallback&& onCountdownComplete, backend::Driver* driver)
            : mUserHandler(handler),
              mUserCallback(std::move(userCallback)),
              mUserParam1(userParam1),
              mUserParam2(userParam2),
              mCountdownCompleteCallback(std::move(onCountdownComplete)),
              mDriver(driver) {
    }

    // 1. This method serves as a custom, intermediate handler that is called by the
    // "ServiceThread" in DriverBase. Its primary purpose is to manage the countdown
    // mechanism before the *REAL* user's callback is executed.
    void post(void* user, Callback callback) override {
        // 2. Calls the static method `countdownCallback(user)` to manage the internal
        // counter for pending asynchronous operations.
        callback(user);
    }

    // 3. Decreases the count of pending asynchronous operations. When the counter hits zero,
    // the final callback is scheduled with the *REAL* user's handler to execute the *REAL*
    // user's callback.
    static void countdownCallback(void* user) {
        auto* thisObject = static_cast<CountdownCallbackHandler*>(user);
        if (thisObject->decreaseCountdown()) {
            if (thisObject->mCountdownCompleteCallback) {
                thisObject->mCountdownCompleteCallback();
            }
            // 4. Schedules the final callback to execute the *REAL* user's callback.
            thisObject->mDriver->scheduleCallback(thisObject->mUserHandler, user,
                    &CountdownCallbackHandler::invokeUserCallback);
        }
    }

    // 5. This method is called via the *REAL* user's handler. Executes the *REAL* user's
    // callback.
    static void invokeUserCallback(void* user) {
        auto* thisObject = static_cast<CountdownCallbackHandler*>(user);
        // 6. Executes the *REAL* user's callback.
        if (thisObject->mUserCallback) {
            thisObject->mUserCallback(thisObject->mUserParam1, thisObject->mUserParam2);
        }
        delete thisObject;
    }

    // Increase the countdown. Users are required to call this for each async call they make.
    uint32_t increaseCountdown() {
        // `std::memory_order_relaxed` should be sufficient because no other variables need to be
        // visible to other threads in a strict sequence.
        auto prev = mCount.fetch_add(1, std::memory_order_relaxed);
        return prev + 1;
    }

private:
    // Reduces the countdown and returns true upon reaching zero.
    bool decreaseCountdown() {
        // `std::memory_order_acq_rel` is recommended here to ensure all member fields (mUserHandler,
        // mCountdownCompleteCallback, and others) are visible to this `ServiceThread` before using
        // them.
        auto prev = mCount.fetch_sub(1, std::memory_order_acq_rel);
        return prev == 1;
    }

    std::atomic_uint32_t mCount = 0;
    CallbackHandler* mUserHandler = nullptr;
    UserCallback mUserCallback;
    T* mUserParam1 = nullptr;
    void* mUserParam2 = nullptr;
    CountdownCompleteCallback mCountdownCompleteCallback;
    backend::Driver* mDriver = nullptr;
};

} // namespace dante

#endif // TNT_DANTE_DETAILS_ASYNCHELPERS_H
