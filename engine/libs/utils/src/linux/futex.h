
#ifndef UTILS_LINUX_FUTEX_H
#define UTILS_LINUX_FUTEX_H

#include <utils/compiler.h>

#include <linux/futex.h>

#include <sys/syscall.h>
#include <unistd.h>

#include <errno.h>
#include <stdbool.h>

struct timespec;

namespace utils {
namespace linuxutil {

inline int futex(volatile void* ftx, int op, int value,
        const struct timespec* timeout, int bitset) {
    // from futex man pages:
    // In the event of an error (and assuming that futex() was invoked via syscall(2)),
    // all operations return -1 and set errno to indicate the cause of the error.
    int err = (int)syscall(__NR_futex, ftx, op, value, timeout, nullptr, bitset);
    if (UTILS_UNLIKELY(err == -1)) {
        err = -errno;
    }
    return err;
}

inline int futex_wake(volatile void* ftx, int count) {
    return futex(ftx, FUTEX_WAKE, count, nullptr, 0);
}

inline int futex_wake_ex(volatile void* ftx, bool shared, int count) {
    return futex(ftx, shared ? FUTEX_WAKE : FUTEX_WAKE_PRIVATE, count, nullptr, 0);
}

inline int futex_wait(volatile void* ftx, int value, const struct timespec* timeout) {
    return futex(ftx, FUTEX_WAIT, value, timeout, 0);
}

inline int futex_wait_ex(volatile void* ftx, bool shared, int value,
                  bool use_realtime_clock, const struct timespec* abs_timeout) {
    return futex(ftx, (shared ? FUTEX_WAIT_BITSET : FUTEX_WAIT_BITSET_PRIVATE) |
                      (use_realtime_clock ? FUTEX_CLOCK_REALTIME : 0), value, abs_timeout,
            FUTEX_BITSET_MATCH_ANY);
}

} // namespace linuxutil
} // namespace utils


#endif // UTILS_LINUX_FUTEX_H
