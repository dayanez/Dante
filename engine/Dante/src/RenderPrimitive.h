
#ifndef TNT_DANTE_DETAILS_RENDERPRIMITIVE_H
#define TNT_DANTE_DETAILS_RENDERPRIMITIVE_H

#include "components/RenderableManager.h"

#include "details/MaterialInstance.h"

#include <dante/RenderableManager.h>

#include <backend/DriverEnums.h>
#include <backend/Handle.h>

#include <stdint.h>

namespace dante {

class FEngine;
class FVertexBuffer;
class FIndexBuffer;
class FRenderer;
class HwRenderPrimitiveFactory;

class FRenderPrimitive {
public:
    FRenderPrimitive() noexcept = default;

    void init(HwRenderPrimitiveFactory& factory, backend::DriverApi& driver,
            FRenderableManager::Entry const& entry) noexcept;

    void set(HwRenderPrimitiveFactory& factory, backend::DriverApi& driver,
            RenderableManager::PrimitiveType type,
            FVertexBuffer const* vertexBuffer, FIndexBuffer const* indexBuffer, size_t offset,
            size_t count) noexcept;

    // frees driver resources, object becomes invalid
    void terminate(HwRenderPrimitiveFactory& factory, backend::DriverApi& driver);

    const FMaterialInstance* getMaterialInstance() const noexcept { return mMaterialInstance; }
    backend::RenderPrimitiveHandle getHwHandle() const noexcept { return mHandle; }
    backend::VertexBufferInfoHandle getVertexBufferInfoHandle() const { return mVertexBufferInfoHandle; }
    // For indexed primitives, this is the index offset; for non-indexed primitives, this is the
    // vertex offset of the draw call. See `isIndexed() and mIsIndexed`.
    uint32_t getOffset() const noexcept { return mOffset; }
    // For indexed primitives, this is the index count; for non-indexed primitives, the vertex
    // count of the draw call.
    uint32_t getCount() const noexcept { return mCount; }
    uint32_t getMorphingBufferOffset() const noexcept { return mMorphingBufferOffset; }

    backend::PrimitiveType getPrimitiveType() const noexcept { return mPrimitiveType; }
    AttributeBitset getEnabledAttributes() const noexcept { return mEnabledAttributes; }
    uint16_t getBlendOrder() const noexcept { return mBlendOrder; }
    bool isGlobalBlendOrderEnabled() const noexcept { return mGlobalBlendOrderEnabled; }
    bool isIndexed() const noexcept { return mIsIndexed; }

    void setMaterialInstance(FMaterialInstance const* mi) noexcept { mMaterialInstance = mi; }

    void setBlendOrder(uint16_t const order) noexcept {
        mBlendOrder = static_cast<uint16_t>(order & 0x7FFF);
    }

    void setGlobalBlendOrderEnabled(bool const enabled) noexcept {
        mGlobalBlendOrderEnabled = enabled;
    }

    void setMorphingBufferOffset(uint32_t const offset) noexcept {
        mMorphingBufferOffset = offset;
    }

private:
    // These first fields are dereferences from PrimitiveInfo, keep them together
    FMaterialInstance const* mMaterialInstance = nullptr;
    backend::Handle<backend::HwRenderPrimitive> mHandle = {};
    backend::Handle<backend::HwVertexBufferInfo> mVertexBufferInfoHandle = {};
    uint32_t mOffset = 0;
    uint32_t mCount = 0;
    uint32_t mMorphingBufferOffset = 0;
    // End PrimitiveInfo fields.

    AttributeBitset mEnabledAttributes = {};
    uint16_t mBlendOrder = 0;
    bool mGlobalBlendOrderEnabled = false;
    // True for the standard indexed draw path; false when the primitive was set up without an
    // IndexBuffer (attribute-less / non-indexed rendering).
    bool mIsIndexed = true;
    backend::PrimitiveType mPrimitiveType = backend::PrimitiveType::TRIANGLES;
};

} // namespace dante

#endif // TNT_DANTE_DETAILS_RENDERPRIMITIVE_H
