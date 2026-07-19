
#include <utils/CountDownLatch.h>

namespace utils {

CountDownLatch::CountDownLatch(size_t count)  noexcept
    : m_initial_count(static_cast<uint32_t>(count)),
     m_remaining_count(static_cast<uint32_t>(count)) {
}

void CountDownLatch::reset(size_t count) noexcept {
    LockGuard<Mutex> guard(m_lock);
    m_initial_count = static_cast<uint32_t>(count);
    m_remaining_count = static_cast<uint32_t>(count);
    if (count == 0) {
        m_cv.notify_all();
    }
}

void CountDownLatch::await() noexcept {
    UniqueLock<Mutex> guard(m_lock);
    while (m_remaining_count != 0) {
        m_cv.wait(guard);
    }
}

void CountDownLatch::latch() noexcept {
    LockGuard<Mutex> guard(m_lock);
    if (m_remaining_count > 0) {
        if (--m_remaining_count == 0) {
            m_cv.notify_all();
        }
    }
}

size_t CountDownLatch::getCount() const noexcept {
    LockGuard<Mutex> guard(m_lock);
    return m_initial_count - m_remaining_count;
}

} // namespace utils
