
#ifndef TNT_UTILS_FIXEDCIRCULARBUFFER_H
#define TNT_UTILS_FIXEDCIRCULARBUFFER_H

#include <memory>
#include <optional>
#include <type_traits>

#include <assert.h>
#include <stddef.h>

namespace utils {

template<typename T>
class FixedCircularBuffer {
public:
    explicit FixedCircularBuffer(size_t const capacity)
        : mData(std::make_unique<T[]>(capacity)), mCapacity(capacity) {}

    size_t size() const noexcept { return mSize; }
    size_t capacity() const noexcept { return mCapacity; }
    bool full() const noexcept { return mCapacity > 0 && mSize == mCapacity; }
    bool empty() const noexcept { return mSize == 0; }

    /**
     * Push v into the buffer. If the buffer is already full, removes the oldest item and returns
     * it. If this buffer has no capacity, simply returns v.
     * @param v the new value to push into the buffer
     * @return if the buffer was full, the oldest value which was displaced
     */
    std::optional<T> push(T v) noexcept {
        if (mCapacity == 0) {
            return v;
        }
        std::optional<T> displaced = full() ? pop() : std::optional<T>{};
        mData[mEnd] = v;
        mEnd = (mEnd + 1) % mCapacity;
        mSize++;
        return displaced;
    }

    T pop() noexcept {
        assert(mSize > 0);
        T result = mData[mBegin];
        mBegin = (mBegin + 1) % mCapacity;
        mSize--;
        return result;
    }

private:
    std::unique_ptr<T[]> mData;

    size_t mBegin = 0;
    size_t mEnd = 0;
    size_t mSize = 0;
    size_t mCapacity;
};

} // namespace utils

#endif  // TNT_UTILS_FIXEDCIRCULARBUFFER_H
