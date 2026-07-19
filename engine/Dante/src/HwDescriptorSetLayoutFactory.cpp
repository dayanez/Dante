
#include "HwDescriptorSetLayoutFactory.h"

#include <private/backend/DriverApi.h>

#include <backend/DriverApiForward.h>
#include <backend/DriverEnums.h>
#include <backend/Handle.h>

#include <utils/compiler.h>
#include <utils/debug.h>
#include <utils/Hash.h>
#include <utils/Log.h>

#include <algorithm>
#include <utility>

#include <stdint.h>
#include <stdlib.h>

namespace dante {

using namespace utils;
using namespace backend;

size_t HwDescriptorSetLayoutFactory::Parameters::hash() const noexcept {
    return hash::murmurSlow(
            reinterpret_cast<uint8_t const *>(dsl.descriptors.data()),
            dsl.descriptors.size() * sizeof(DescriptorSetLayoutDescriptor),
            42);
}

bool operator==(HwDescriptorSetLayoutFactory::Parameters const& lhs,
        HwDescriptorSetLayoutFactory::Parameters const& rhs) noexcept {
    return (lhs.dsl.descriptors.size() == rhs.dsl.descriptors.size()) &&
           std::equal(
                   lhs.dsl.descriptors.begin(), lhs.dsl.descriptors.end(),
                   rhs.dsl.descriptors.begin());
}

// ------------------------------------------------------------------------------------------------

HwDescriptorSetLayoutFactory::HwDescriptorSetLayoutFactory()
        : mArena("HwDescriptorSetLayoutFactory::mArena", SET_ARENA_SIZE),
          mBimap(mArena) {
    mBimap.reserve(256);
}

HwDescriptorSetLayoutFactory::~HwDescriptorSetLayoutFactory() noexcept = default;

void HwDescriptorSetLayoutFactory::terminate(DriverApi&) noexcept {
    assert_invariant(mBimap.empty());
}

auto HwDescriptorSetLayoutFactory::create(DriverApi& driver,
        DescriptorSetLayout dsl) noexcept -> Handle {

    std::sort(dsl.descriptors.begin(), dsl.descriptors.end(),
            [](auto&& lhs, auto&& rhs) {
        return lhs.binding < rhs.binding;
    });

    // see if we already have seen this RenderPrimitive
    Key const key({ dsl });
    auto pos = mBimap.find(key);

    // the common case is that we've never seen it (i.e.: no reuse)
    if (UTILS_LIKELY(pos == mBimap.end())) {
        auto handle = driver.createDescriptorSetLayout(std::move(dsl));
        mBimap.insert(key, { handle });
        return handle;
    }

    ++(pos->first.pKey->refs);

    return pos->second.handle;
}

void HwDescriptorSetLayoutFactory::destroy(DriverApi& driver, Handle handle) noexcept {
    // look for this handle in our map
    auto pos = mBimap.findValue(Value{ handle });
    if (--pos->second.pKey->refs == 0) {
        mBimap.erase(pos);
        driver.destroyDescriptorSetLayout(handle);
    }
}

} // namespace dante
