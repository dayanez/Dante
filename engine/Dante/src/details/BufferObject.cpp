/*
 * Copyright (C) 2021 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "details/BufferObject.h"

#include "FilamentAPI-impl.h"

#include "details/Engine.h"

#include <filament/BufferObject.h>

#include <backend/DriverEnums.h>

#include <utils/CString.h>
#include <utils/Panic.h>
#include <utils/StaticString.h>

#include <utility>

#include <stddef.h>
#include <stdint.h>

namespace filament {

struct BufferObject::BuilderDetails {
    BindingType mBindingType = BindingType::VERTEX;
    uint32_t mByteCount = 0;
};

using BuilderType = BufferObject;
BuilderType::Builder::Builder() noexcept = default;
BuilderType::Builder::~Builder() noexcept = default;
BuilderType::Builder::Builder(Builder const& rhs) noexcept = default;
BuilderType::Builder::Builder(Builder&& rhs) noexcept = default;
BuilderType::Builder& BuilderType::Builder::operator=(Builder const& rhs) noexcept = default;
BuilderType::Builder& BuilderType::Builder::operator=(Builder&& rhs) noexcept = default;

BufferObject::Builder& BufferObject::Builder::size(uint32_t const byteCount) noexcept {
    mImpl->mByteCount = byteCount;
    return *this;
}

BufferObject::Builder& BufferObject::Builder::bindingType(BindingType const bindingType) noexcept {
    mImpl->mBindingType = bindingType;
    return *this;
}

BufferObject::Builder& BufferObject::Builder::name(const char* name, size_t const len) noexcept {
    return BuilderNameMixin::name(name, len);
}

BufferObject::Builder& BufferObject::Builder::name(utils::StaticString const& name) noexcept {
    return BuilderNameMixin::name(name);
}

BufferObject* BufferObject::Builder::build(Engine& engine) {
    return downcast(engine).createBufferObject(*this);
}

// ------------------------------------------------------------------------------------------------

FBufferObject::FBufferObject(FEngine& engine, const Builder& builder)
        : mByteCount(builder->mByteCount), mBindingType(builder->mBindingType) {
    FEngine::DriverApi& driver = engine.getDriverApi();
    mHandle = driver.createBufferObject(builder->mByteCount, builder->mBindingType,
            backend::BufferUsage::STATIC, utils::ImmutableCString{ builder.getName() });
}

void FBufferObject::terminate(FEngine& engine) {
    FEngine::DriverApi& driver = engine.getDriverApi();
    driver.destroyBufferObject(mHandle);
}

void FBufferObject::setBuffer(FEngine& engine, BufferDescriptor&& buffer, uint32_t const byteOffset) {

    FILAMENT_CHECK_PRECONDITION((byteOffset & 0x3) == 0)
            << "byteOffset must be a multiple of 4";

    engine.getDriverApi().updateBufferObject(mHandle, std::move(buffer), byteOffset);
}

} // namespace filament
