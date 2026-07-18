/*
 * Copyright (C) 2015 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_FILAMENTAPI_H
#define TNT_FILAMENT_FILAMENTAPI_H

#include <utils/compiler.h>
#include <utils/ImmutableCString.h>
#include <utils/PrivateImplementation.h>
#include <utils/StaticString.h>

#include <stddef.h>

namespace filament {

/**
 * \privatesection
 * FilamentAPI is used to define an API in filament.
 * It ensures the class defining the API can't be created, destroyed
 * or copied by the caller.
 */
class UTILS_PUBLIC FilamentAPI {
protected:
    // disallow creation on the stack
    FilamentAPI() noexcept = default;
    ~FilamentAPI() = default;

public:
    // disallow copy and assignment
    FilamentAPI(FilamentAPI const&) = delete;
    FilamentAPI(FilamentAPI&&) = delete;
    FilamentAPI& operator=(FilamentAPI const&) = delete;
    FilamentAPI& operator=(FilamentAPI&&) = delete;

    // allow placement-new allocation, don't use "noexcept", to avoid compiler null check
    static void *operator new     (size_t, void* p) { return p; }

    // prevent heap allocation
    static void *operator new     (size_t) = delete;
    static void *operator new[]   (size_t) = delete;
};

template<typename T>
using BuilderBase = utils::PrivateImplementation<T>;

// This needs to be public because it is used in the following template.
UTILS_PUBLIC void builderMakeName(utils::ImmutableCString& outName, const char* name, size_t len) noexcept;

template <typename Builder>
class UTILS_PUBLIC BuilderNameMixin {
public:
    UTILS_DEPRECATED
    Builder& name(const char* name, size_t len) noexcept {
        builderMakeName(mName, name, len);
        return static_cast<Builder&>(*this);
    }

    Builder& name(utils::StaticString const& name) noexcept {
        builderMakeName(mName, name.data(), name.size());
        return static_cast<Builder&>(*this);
    }

    utils::ImmutableCString const& getName() const noexcept { return mName; }

    utils::ImmutableCString const& getNameOrDefault() const noexcept {
        if (const auto& name = getName(); !name.empty()) {
            return name;
        }
        static const utils::ImmutableCString sDefaultName = "(none)";
        return sDefaultName;
    }

private:
    utils::ImmutableCString mName;
};

} // namespace filament

#endif // TNT_FILAMENT_FILAMENTAPI_H
