/*
 * Copyright (C) 2016 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <backend/Handle.h>

#include <utils/compiler.h>
#include <utils/CallStack.h>
#include <utils/ostream.h>

#ifndef NDEBUG
#   include <utils/CString.h>
#   include <string_view>
#endif

#include <stddef.h>

using namespace utils;

namespace filament::backend {

#ifndef NDEBUG

static char const * const kOurNamespace = "filament::backend::";

// removes all occurrences of "what" from "str"
UTILS_NOINLINE
static CString& removeAll(CString& str, const std::string_view what) noexcept {
    if (!what.empty()) {
        const CString empty;
        size_t pos = 0;
        while ((pos = std::string_view{ str.data(), str.size() }.find(what, pos)) != std::string_view::npos) {
            str.replace(pos, what.length(), empty);
        }
    }
    return str;
}

template <typename T>
UTILS_NOINLINE
static io::ostream& logHandle(io::ostream& out, CString& typeName, T id) noexcept {
    return out << removeAll(typeName, kOurNamespace) << " @ " << id;
}

template <typename T>
io::ostream& operator<<(io::ostream& out, const Handle<T>& h) noexcept {
    CString s{ CallStack::typeName<Handle<T>>() };
    return logHandle(out, s, h.getId());
}

// Explicit Instantiation of the streaming operators (so they're not inlined)
template io::ostream& operator<<(io::ostream& out, const Handle<HwVertexBuffer>& h) noexcept;
template io::ostream& operator<<(io::ostream& out, const Handle<HwIndexBuffer>& h) noexcept;
template io::ostream& operator<<(io::ostream& out, const Handle<HwRenderPrimitive>& h) noexcept;
template io::ostream& operator<<(io::ostream& out, const Handle<HwProgram>& h) noexcept;
template io::ostream& operator<<(io::ostream& out, const Handle<HwTexture>& h) noexcept;
template io::ostream& operator<<(io::ostream& out, const Handle<HwRenderTarget>& h) noexcept;
template io::ostream& operator<<(io::ostream& out, const Handle<HwFence>& h) noexcept;
template io::ostream& operator<<(io::ostream& out, const Handle<HwSwapChain>& h) noexcept;
template io::ostream& operator<<(io::ostream& out, const Handle<HwStream>& h) noexcept;
template io::ostream& operator<<(io::ostream& out, const Handle<HwTimerQuery>& h) noexcept;
template io::ostream& operator<<(io::ostream& out, const Handle<HwBufferObject>& h) noexcept;
template io::ostream& operator<<(io::ostream& out, const Handle<HwDescriptorSet>& h) noexcept;
template io::ostream& operator<<(io::ostream& out, const Handle<HwDescriptorSetLayout>& h) noexcept;
template io::ostream& operator<<(io::ostream& out, const Handle<HwVertexBufferInfo>& h) noexcept;

#endif

} // namespace filament::backend
