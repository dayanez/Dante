
#ifndef TNT_DANTE_DETAILS_MORPHTARGETBUFFER_H
#define TNT_DANTE_DETAILS_MORPHTARGETBUFFER_H

#include "downcast.h"

#include <dante/MorphTargetBuffer.h>

#include <backend/DriverApiForward.h>
#include <backend/DriverEnums.h>
#include <backend/Handle.h>

#include <math/vec3.h>
#include <math/vec4.h>

#include <stddef.h>
#include <stdint.h>

namespace dante {

class FEngine;

class FMorphTargetBuffer : public MorphTargetBuffer {
public:
    class EmptyMorphTargetBuilder : public Builder {
    public:
        EmptyMorphTargetBuilder();
    };

    FMorphTargetBuffer(FEngine& engine, const Builder& builder);

    // frees driver resources, object becomes invalid
    void terminate(FEngine& engine);

    void setPositionsAt(FEngine& engine, size_t targetIndex,
            math::float3 const* positions, size_t count, size_t offset);

    void setPositionsAt(FEngine& engine, size_t targetIndex,
            math::float4 const* positions, size_t count, size_t offset);

    void setTangentsAt(FEngine& engine, size_t targetIndex,
            math::short4 const* tangents, size_t count, size_t offset);

    bool hasPositions() const noexcept {
        return bool(mPbHandle);
    }

    bool hasTangents() const noexcept {
        return bool(mTbHandle);
    }

    bool isCustomMorphingEnabled() const noexcept {
        return mEnableCustomMorphing;
    }

    inline size_t getVertexCount() const noexcept { return mVertexCount; }
    inline size_t getCount() const noexcept { return mCount; }

    backend::TextureHandle getPositionsHandle() const noexcept {
        return mPbHandle;
    }

    backend::TextureHandle getTangentsHandle() const noexcept {
        return mTbHandle;
    }

private:
    void updateDataAt(backend::DriverApi& driver, backend::Handle <backend::HwTexture> handle,
            backend::PixelDataFormat format, backend::PixelDataType type, const char* out,
            size_t elementSize, size_t targetIndex, size_t count, size_t offset);

    bool mEnableCustomMorphing;
    backend::TextureHandle mPbHandle;
    backend::TextureHandle mTbHandle;
    uint32_t mVertexCount;
    uint32_t mCount;
};

DANTE_DOWNCAST(MorphTargetBuffer)

} // namespace dante

#endif //TNT_DANTE_DETAILS_MORPHTARGETBUFFER_H
