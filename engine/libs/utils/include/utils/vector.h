/*
 * Copyright (C) 2015 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_UTILS_VECTOR_H
#define TNT_UTILS_VECTOR_H

#include <algorithm>
#include <vector>

namespace utils {

/**
 * Inserts the specified item in the vector at its sorted position.
 */
template <typename T>
static inline void insert_sorted(std::vector<T>& v, T item) {
    auto pos = std::lower_bound(v.begin(), v.end(), item);
    v.insert(pos, std::move(item));
}

/**
 * Inserts the specified item in the vector at its sorted position.
 * The item type must implement the < operator. If the specified
 * item is already present in the vector, this method returns without
 * inserting the item again.
 *
 * @return True if the item was inserted at is sorted position, false
 *         if the item already exists in the vector.
 */
template <typename T>
static inline bool insert_sorted_unique(std::vector<T>& v, T item) {
    if (UTILS_LIKELY(v.size() == 0 || v.back() < item)) {
        v.push_back(item);
        return true;
    }

    auto pos = std::lower_bound(v.begin(), v.end(), item);
    if (UTILS_LIKELY(pos == v.end() || item < *pos)) {
        v.insert(pos, std::move(item));
        return true;
    }

    return false;
}

} // end utils namespace

#endif // TNT_UTILS_VECTOR_H
