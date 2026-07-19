
#ifndef UTILS_PRIVATEIMPLEMENTATION_H
#define UTILS_PRIVATEIMPLEMENTATION_H

#include <stddef.h>

namespace utils {

/**
 * \privatesection
 * PrivateImplementation is used to hide the implementation details of a class and ensure a higher
 * level of backward binary compatibility.
 * The actual implementation is in src/PrivateImplementation-impl.h"
 */
template<typename T>
class PrivateImplementation {
public:
    // none of these methods must be implemented inline because it's important that their
    // implementation be hidden from the public headers.
    template<typename ... ARGS>
    explicit PrivateImplementation(ARGS&& ...) noexcept;
    PrivateImplementation() noexcept;
    ~PrivateImplementation() noexcept;
    PrivateImplementation(PrivateImplementation const& rhs) noexcept;
    PrivateImplementation& operator = (PrivateImplementation const& rhs) noexcept;

    // move ctor and copy operator can be implemented inline and don't need to be exported
    PrivateImplementation(PrivateImplementation&& rhs) noexcept : mImpl(rhs.mImpl) { rhs.mImpl = nullptr; }
    PrivateImplementation& operator = (PrivateImplementation&& rhs) noexcept {
        auto temp = mImpl;
        mImpl = rhs.mImpl;
        rhs.mImpl = temp;
        return *this;
    }

protected:
    T* mImpl = nullptr;
    inline T* operator->() noexcept { return mImpl; }
    inline T const* operator->() const noexcept { return mImpl; }
};

} // namespace utils

#endif // UTILS_PRIVATEIMPLEMENTATION_H
