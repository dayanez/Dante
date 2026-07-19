
#ifndef TNT_UTILS_THREADUTILS_H
#define TNT_UTILS_THREADUTILS_H

#include <thread>

namespace utils {

class ThreadUtils {
public:
    static std::thread::id getThreadId() noexcept;
    static bool isThisThread(std::thread::id id) noexcept;
};

} // namespace utils

#endif // TNT_UTILS_THREADUTILS_H
