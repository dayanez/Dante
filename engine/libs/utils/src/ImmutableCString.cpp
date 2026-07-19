
#include <utils/compiler.h>
#include <utils/ImmutableCString.h>
#include <utils/Logger.h>
#include <utils/ostream.h>
#include <utils/StaticString.h>

#include <atomic>
#include <cstdlib>
#include <cstring>

namespace utils {

namespace {
struct ImmutableCStringStats {
    std::atomic_int32_t alive = { 0 };
    std::atomic_int32_t ctor = { 0 };
    std::atomic_int32_t staticStrings = { 0 };
    std::atomic_int32_t heapStrings = { 0 };
};

ImmutableCStringStats gImmutableCStringStats{};
constexpr size_t IMMUTABLECSTRING_LOG_INTERVAL = 10000;
}

void ImmutableCString::do_tracking(bool ctor, bool is_static) {
    if (ctor) {
        gImmutableCStringStats.ctor.fetch_add(1, std::memory_order_relaxed);
        gImmutableCStringStats.alive.fetch_add(1, std::memory_order_relaxed);
        if (is_static) {
            gImmutableCStringStats.staticStrings.fetch_add(1, std::memory_order_relaxed);
        } else {
            gImmutableCStringStats.heapStrings.fetch_add(1, std::memory_order_relaxed);
        }
    } else {
        gImmutableCStringStats.alive.fetch_sub(1, std::memory_order_relaxed);
        if (is_static) {
            gImmutableCStringStats.staticStrings.fetch_sub(1, std::memory_order_relaxed);
        } else {
            gImmutableCStringStats.heapStrings.fetch_sub(1, std::memory_order_relaxed);
        }
    }
    static std::atomic_int32_t sCtorSinceLastLog = { 0 };
    if (UTILS_UNLIKELY(sCtorSinceLastLog.fetch_add(1, std::memory_order_relaxed) == IMMUTABLECSTRING_LOG_INTERVAL)) {
        LOG(INFO) << "ImmutableCString stats: "
                  << gImmutableCStringStats.alive.load(std::memory_order_relaxed) << " alive, "
                  << gImmutableCStringStats.ctor.load(std::memory_order_relaxed) << " ctor, "
                  << gImmutableCStringStats.staticStrings.load(std::memory_order_relaxed) << " static, "
                  << gImmutableCStringStats.heapStrings.load(std::memory_order_relaxed) << " heap";
        sCtorSinceLastLog.store(0, std::memory_order_relaxed);
    }
}

ImmutableCString& ImmutableCString::operator=(const ImmutableCString& other) {
    if (this != &other) {
        ImmutableCString(other).swap(*this);
    }
    return *this;
}

ImmutableCString& ImmutableCString::operator=(ImmutableCString&& other) noexcept {
    this->swap(other);
    return *this;
}


void ImmutableCString::initializeFrom(const char* cstr, size_t const length) {
    if (length > 0 && cstr) {
        char* buffer = static_cast<char*>(malloc(length + 1));
        if (UTILS_LIKELY(buffer)) {
            memcpy(buffer, cstr, length);
            buffer[length] = '\0';
            mData = buffer;
            mSize = length;
            mIsStatic = false;
            return;
        }
    }
    mData = "";
    mSize = 0;
    mIsStatic = true;
}

#if !defined(NDEBUG)
io::ostream& operator<<(io::ostream& out, const ImmutableCString& rhs) {
    return out << rhs.c_str();
}

io::ostream& operator<<(io::ostream& out, const StaticString& rhs) {
    return out << rhs.c_str();
}
#endif

} // namespace utils
