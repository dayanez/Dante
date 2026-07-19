
#include "HwRenderPrimitiveFactory.h"

#include <private/backend/DriverApi.h>

#include <backend/DriverApiForward.h>
#include <backend/DriverEnums.h>
#include <backend/Handle.h>

#include <utils/compiler.h>
#include <utils/debug.h>
#include <utils/Hash.h>

#include <stdlib.h>

namespace dante {

using namespace utils;
using namespace backend;

size_t HwRenderPrimitiveFactory::Parameters::hash() const noexcept {
    return hash::combine(vbh.getId(),
            hash::combine(ibh.getId(),
                    (size_t)type));
}

bool operator==(HwRenderPrimitiveFactory::Parameters const& lhs,
        HwRenderPrimitiveFactory::Parameters const& rhs) noexcept {
    return lhs.vbh == rhs.vbh &&
           lhs.ibh == rhs.ibh &&
           lhs.type == rhs.type;
}

// ------------------------------------------------------------------------------------------------

HwRenderPrimitiveFactory::HwRenderPrimitiveFactory()
        : mArena("HwRenderPrimitiveFactory::mArena", SET_ARENA_SIZE),
          mBimap(mArena) {
    mBimap.reserve(256);
}

HwRenderPrimitiveFactory::~HwRenderPrimitiveFactory() noexcept = default;

void HwRenderPrimitiveFactory::terminate(DriverApi&) noexcept {
    assert_invariant(mBimap.empty());
}

auto HwRenderPrimitiveFactory::create(DriverApi& driver,
        VertexBufferHandle vbh,
        IndexBufferHandle ibh,
        PrimitiveType const type) noexcept -> Handle {

    // see if we already have seen this RenderPrimitive
    Key const key({ vbh, ibh, type });
    auto pos = mBimap.find(key);

    // the common case is that we've never seen it (i.e.: no reuse)
    if (UTILS_LIKELY(pos == mBimap.end())) {
        auto handle = driver.createRenderPrimitive(vbh, ibh, type);
        mBimap.insert(key, { handle });
        return handle;
    }

    ++(pos->first.pKey->refs);
    return pos->second.handle;
}

void HwRenderPrimitiveFactory::destroy(DriverApi& driver, Handle handle) noexcept {
    // look for this handle in our map
    auto pos = mBimap.findValue(Value{ handle });
    if (--pos->second.pKey->refs == 0) {
        mBimap.erase(pos);
        driver.destroyRenderPrimitive(handle);
    }
}

} // namespace dante
