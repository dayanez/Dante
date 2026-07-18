/*
 * Copyright (C) 2024 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_BACKEND_COMMANDSTREAMVECTOR_H
#define TNT_FILAMENT_BACKEND_COMMANDSTREAMVECTOR_H

#include <backend/DriverApiForward.h>

#include <initializer_list>
#include <memory>

#include <stddef.h>
#include <stdint.h>


namespace utils::io {
class ostream;
} // namespace utils::io

namespace filament::backend {

void* allocateFromCommandStream(DriverApi& driver, size_t size, size_t alignment) noexcept;

class DescriptorSetOffsetArray {
public:
    using value_type = uint32_t;
    using reference = value_type&;
    using const_reference = value_type const&;
    using size_type = uint32_t;
    using difference_type = int32_t;
    using pointer = value_type*;
    using const_pointer = value_type const*;
    using iterator = pointer;
    using const_iterator = const_pointer;

    DescriptorSetOffsetArray() noexcept = default;

    ~DescriptorSetOffsetArray() noexcept = default;

    DescriptorSetOffsetArray(size_type size, DriverApi& driver) noexcept {
        mOffsets = (value_type *)allocateFromCommandStream(driver,
                size * sizeof(value_type), alignof(value_type));
        std::uninitialized_fill_n(mOffsets, size, 0);
    }

    DescriptorSetOffsetArray(std::initializer_list<uint32_t> list, DriverApi& driver) noexcept {
        mOffsets = (value_type *)allocateFromCommandStream(driver,
                list.size() * sizeof(value_type), alignof(value_type));
        std::uninitialized_copy(list.begin(), list.end(), mOffsets);
    }

    DescriptorSetOffsetArray(DescriptorSetOffsetArray const&) = delete;
    DescriptorSetOffsetArray& operator=(DescriptorSetOffsetArray const&) = delete;

    DescriptorSetOffsetArray(DescriptorSetOffsetArray&& rhs) noexcept
            : mOffsets(rhs.mOffsets) {
        rhs.mOffsets = nullptr;
    }

    DescriptorSetOffsetArray& operator=(DescriptorSetOffsetArray&& rhs) noexcept {
        if (this != &rhs) {
            mOffsets = rhs.mOffsets;
            rhs.mOffsets = nullptr;
        }
        return *this;
    }

    bool empty() const noexcept { return mOffsets == nullptr; }

    value_type* data() noexcept { return mOffsets; }
    const value_type* data() const noexcept { return mOffsets; }


    reference operator[](size_type n) noexcept {
        return *(data() + n);
    }

    const_reference operator[](size_type n) const noexcept {
        return *(data() + n);
    }

    void clear() noexcept {
        mOffsets = nullptr;
    }

private:
    value_type *mOffsets = nullptr;
};

} // namespace filament::backend

#if !defined(NDEBUG)
utils::io::ostream& operator<<(utils::io::ostream& out, const filament::backend::DescriptorSetOffsetArray& rhs);
#endif

#endif //TNT_FILAMENT_BACKEND_COMMANDSTREAMVECTOR_H
