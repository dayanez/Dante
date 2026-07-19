
#include <utils/compiler.h>
#include <utils/ThreadUtils.h>

namespace utils {

UTILS_NOINLINE
std::thread::id ThreadUtils::getThreadId() noexcept {
    return std::this_thread::get_id();
}

bool ThreadUtils::isThisThread(std::thread::id id) noexcept {
    return getThreadId() == id;
}

} // namespace utils
