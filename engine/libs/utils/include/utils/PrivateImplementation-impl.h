/*
 * Copyright (C) 2022 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef UTILS_PRIVATEIMPLEMENTATION_IMPL_H
#define UTILS_PRIVATEIMPLEMENTATION_IMPL_H

/*
 * This looks like a header file, but really it acts as a .cpp, because it's used to
 * explicitly instantiate the methods of PrivateImplementation<>
 */

#include <utils/PrivateImplementation.h>

#include <utility>

namespace utils {

template<typename T>
PrivateImplementation<T>::PrivateImplementation() noexcept
        : mImpl(new T) {
}

template<typename T>
template<typename ... ARGS>
PrivateImplementation<T>::PrivateImplementation(ARGS&& ... args) noexcept
        : mImpl(new T(std::forward<ARGS>(args)...)) {
}

template<typename T>
PrivateImplementation<T>::~PrivateImplementation() noexcept {
    delete mImpl;
}

#ifndef UTILS_PRIVATE_IMPLEMENTATION_NON_COPYABLE

template<typename T>
PrivateImplementation<T>::PrivateImplementation(PrivateImplementation const& rhs) noexcept
        : mImpl(new T(*rhs.mImpl)) {
}

template<typename T>
PrivateImplementation<T>& PrivateImplementation<T>::operator=(PrivateImplementation<T> const& rhs) noexcept {
    if (this != &rhs) {
        *mImpl = *rhs.mImpl;
    }
    return *this;
}

#endif

} // namespace utils

#endif // UTILS_PRIVATEIMPLEMENTATION_IMPL_H
