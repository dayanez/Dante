
#ifndef TNT_UTILS_GENERIC_CONDITION_H
#define TNT_UTILS_GENERIC_CONDITION_H

#include <condition_variable>

#include <stddef.h>

namespace utils {

#if defined(UTILS_DEBUG_MUTEX) || defined(DANTE_DEBUG_MUTEX)
class Condition : public std::condition_variable_any {
public:
    using std::condition_variable_any::condition_variable_any;
#else
class Condition : public std::condition_variable {
public:
    using std::condition_variable::condition_variable;
#endif

    inline void notify_n(size_t const n) noexcept {
        if (n == 1) {
            notify_one();
        } else if (n > 1) {
            notify_all();
        }
    }
};

} // namespace utils

#endif // TNT_UTILS_GENERIC_CONDITION_H
