
#include "futex.h"

#include <utils/linux/Mutex.h>

namespace utils {
namespace linuxutil {

void Mutex::wait() noexcept {
    while (UTILS_UNLIKELY(mState.exchange(LOCKED_CONTENDED, std::memory_order_acquire) != UNLOCKED)) {
        linuxutil::futex_wait_ex(&mState, false, LOCKED_CONTENDED, false, nullptr);
    }
}

void Mutex::wake() noexcept {
    linuxutil::futex_wake_ex(&mState, false, LOCKED);
}

} // namespace linuxutil
} // namespace utils

