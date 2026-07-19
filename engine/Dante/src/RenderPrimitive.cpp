
#include "RenderPrimitive.h"

#include "details/IndexBuffer.h"
#include "details/MaterialInstance.h"
#include "details/VertexBuffer.h"

#include <dante/MaterialEnums.h>
#include <dante/RenderableManager.h>

#include <private/backend/CommandStream.h>

#include <backend/DriverApiForward.h>

#include <utils/debug.h>

#include <stddef.h>

namespace dante {

void FRenderPrimitive::init(HwRenderPrimitiveFactory& factory, backend::DriverApi& driver,
        FRenderableManager::Entry const& entry) noexcept {

    assert_invariant(entry.materialInstance);

    mMaterialInstance = downcast(entry.materialInstance);
    mBlendOrder = entry.blendOrder;
    mGlobalBlendOrderEnabled = entry.globalBlendOrderEnabled;

    // We require a VertexBuffer to set up the primitive, but the IndexBuffer is optional.
    // When entry.indices is null, the primitive is configured for non-indexed (attribute-less)
    // rendering: offset/count are interpreted as vertexOffset/vertexCount.
    if (entry.vertices) {
        FVertexBuffer const* vertexBuffer = downcast(entry.vertices);
        FIndexBuffer const* indexBuffer = downcast(entry.indices);
        set(factory, driver, entry.type, vertexBuffer, indexBuffer, entry.offset, entry.count);
    }
}

void FRenderPrimitive::terminate(HwRenderPrimitiveFactory& factory, backend::DriverApi& driver) {
    if (mHandle) {
        factory.destroy(driver, mHandle);
    }
}

void FRenderPrimitive::set(HwRenderPrimitiveFactory& factory, backend::DriverApi& driver,
        RenderableManager::PrimitiveType const type,
        FVertexBuffer const* vertexBuffer, FIndexBuffer const* indexBuffer,
        size_t const offset, size_t const count) noexcept {
    if (mHandle) {
        factory.destroy(driver, mHandle);
    }

    AttributeBitset const enabledAttributes = vertexBuffer->getDeclaredAttributes();

    auto const& ebh = vertexBuffer->getHwHandle();
    // A null IndexBuffer is permitted for non-indexed (attribute-less) rendering.
    auto const ibh = indexBuffer ? indexBuffer->getHwHandle() : backend::IndexBufferHandle{};

    mHandle = factory.create(driver, ebh, ibh, type);
    mVertexBufferInfoHandle = vertexBuffer->getVertexBufferInfoHandle();

    mPrimitiveType = type;
    mOffset = offset;
    mCount = count;
    mEnabledAttributes = enabledAttributes;
    mIsIndexed = (indexBuffer != nullptr);
}

} // namespace dante
