
#include "DescriptorSetLayout.h"

#include "HwDescriptorSetLayoutFactory.h"

#include "details/Engine.h"

#include <backend/DriverEnums.h>

#include <utils/debug.h>
#include <utils/FixedCapacityVector.h>

#include <algorithm>
#include <utility>

namespace dante {

DescriptorSetLayout::DescriptorSetLayout() noexcept = default;

DescriptorSetLayout::DescriptorSetLayout(
        HwDescriptorSetLayoutFactory& factory,
        backend::DriverApi& driver,
        backend::DescriptorSetLayout descriptorSetLayout) noexcept  {
    for (auto&& desc : descriptorSetLayout.descriptors) {
        mMaxDescriptorBinding = std::max(mMaxDescriptorBinding, desc.binding);
        mSamplers.set(desc.binding, backend::DescriptorSetLayoutDescriptor::isSampler(desc.type));
        mUniformBuffers.set(desc.binding, desc.type == backend::DescriptorType::UNIFORM_BUFFER);
    }

    assert_invariant(mMaxDescriptorBinding < utils::bitset64::BIT_COUNT);

    mDescriptorTypes = utils::FixedCapacityVector<backend::DescriptorType>(mMaxDescriptorBinding + 1);
    for (auto&& desc : descriptorSetLayout.descriptors) {
        mDescriptorTypes[desc.binding] = desc.type;
    }

    mDescriptorSetLayoutHandle = factory.create(driver,
            std::move(descriptorSetLayout));
}

void DescriptorSetLayout::terminate(
        HwDescriptorSetLayoutFactory& factory,
        backend::DriverApi& driver) noexcept {
    if (mDescriptorSetLayoutHandle) {
        factory.destroy(driver, mDescriptorSetLayoutHandle);
    }
}

DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout&& rhs) noexcept = default;

DescriptorSetLayout& DescriptorSetLayout::operator=(DescriptorSetLayout&& rhs) noexcept = default;

} // namespace dante
