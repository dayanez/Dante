/*
 * Copyright (C) 2024 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_DESCRIPTORSETLAYOUT_H
#define TNT_FILAMENT_DESCRIPTORSETLAYOUT_H

#include <backend/DriverApiForward.h>
#include <backend/DriverEnums.h>
#include <backend/Handle.h>

#include <utils/bitset.h>
#include <utils/FixedCapacityVector.h>

#include <stddef.h>
#include <stdint.h>

namespace filament {

class HwDescriptorSetLayoutFactory;

class DescriptorSetLayout {
public:
    DescriptorSetLayout() noexcept;
    DescriptorSetLayout(
            HwDescriptorSetLayoutFactory& factory,
            backend::DriverApi& driver,
            backend::DescriptorSetLayout descriptorSetLayout) noexcept;

    DescriptorSetLayout(DescriptorSetLayout const&) = delete;
    DescriptorSetLayout(DescriptorSetLayout&& rhs) noexcept;
    DescriptorSetLayout& operator=(DescriptorSetLayout const&) = delete;
    DescriptorSetLayout& operator=(DescriptorSetLayout&& rhs) noexcept;

    void terminate(
            HwDescriptorSetLayoutFactory& factory,
            backend::DriverApi& driver) noexcept;

    backend::DescriptorSetLayoutHandle getHandle() const noexcept {
        return mDescriptorSetLayoutHandle;
    }

    size_t getMaxDescriptorBinding() const noexcept {
        return mMaxDescriptorBinding;
    }

    bool isValid(backend::descriptor_binding_t const binding) const noexcept {
        return mSamplers[binding] || mUniformBuffers[binding];
    }

    bool isSampler(backend::descriptor_binding_t const binding) const noexcept {
        return mSamplers[binding];
    }

    utils::bitset64 getValidDescriptors() const noexcept {
        return mSamplers | mUniformBuffers;
    }

    utils::bitset64 getSamplerDescriptors() const noexcept {
        return mSamplers;
    }

    utils::bitset64 getUniformBufferDescriptors() const noexcept {
        return mUniformBuffers;
    }

    backend::DescriptorType getDescriptorType(
        backend::descriptor_binding_t const binding) const noexcept {
        return mDescriptorTypes[binding];
    }

private:
    backend::DescriptorSetLayoutHandle mDescriptorSetLayoutHandle;
    utils::bitset64 mSamplers;
    utils::bitset64 mUniformBuffers;
    uint8_t mMaxDescriptorBinding = 0;
    utils::FixedCapacityVector<backend::DescriptorType> mDescriptorTypes;
};


} // namespace filament

#endif //TNT_FILAMENT_DESCRIPTORSETLAYOUT_H
