/*
 * Copyright (C) 2025 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */
#include "ProgramSpecialization.h"

#include <utils/Hash.h>

namespace filament {

size_t ProgramSpecialization::hash() const noexcept {
    size_t seed = 0;
    utils::hash::combine_fast(seed, materialCrc32);
    utils::hash::combine_fast(seed, variant.key);
    utils::hash::combine_fast(seed, specKey.key);
    // Both specKey and dynamic constants (indices 16-31) track the same dynamic state.
    // Direct slice hashing is safe and fast because indices 16-31 are unmutated defaults during cache lookup.
    utils::hash::combine_fast(seed, specializationConstants.hash());
    return seed;
}

bool ProgramSpecialization::operator==(ProgramSpecialization const& rhs) const noexcept {
    if (this == &rhs) {
        return true;
    }
    // Both specKey and dynamic constants track the same state. Direct slice comparison
    // safely avoids element-wise loop overhead because dynamic constants are unmutated defaults here.
    return materialCrc32 == rhs.materialCrc32 && variant == rhs.variant &&
           specKey == rhs.specKey &&
           specializationConstants == rhs.specializationConstants;
}

} // namespace filament
