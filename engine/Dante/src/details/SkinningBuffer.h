
#ifndef TNT_DANTE_DETAILS_SKINNINGBUFFER_H
#define TNT_DANTE_DETAILS_SKINNINGBUFFER_H

#include "downcast.h"

#include <private/dante/EngineEnums.h>
#include <private/dante/UibStructs.h>

#include <dante/SkinningBuffer.h>

#include <backend/DriverApiForward.h>
#include <backend/Handle.h>

#include <utils/FixedCapacityVector.h>

#include <math/mat4.h>
#include <math/vec2.h>

#include <stddef.h>
#include <stdint.h>

// for gtest
class DanteTest_Bones_Test;

namespace dante {

class FEngine;
class FRenderableManager;

class FSkinningBuffer : public SkinningBuffer {
public:
    FSkinningBuffer(FEngine& engine, const Builder& builder);

    // frees driver resources, object becomes invalid
    void terminate(FEngine& engine);

    void setBones(FEngine& engine, RenderableManager::Bone const* transforms, size_t count, size_t offset);
    void setBones(FEngine& engine, math::mat4f const* transforms, size_t count, size_t offset);
    size_t getBoneCount() const noexcept { return mBoneCount; }

    // round count to the size of the UBO in the shader
    static size_t getPhysicalBoneCount(size_t const count) noexcept {
        static_assert((CONFIG_MAX_BONE_COUNT & (CONFIG_MAX_BONE_COUNT - 1)) == 0);
        return (count + CONFIG_MAX_BONE_COUNT - 1) & ~(CONFIG_MAX_BONE_COUNT - 1);
    }

private:
    friend class DanteTest_Bones_Test;
    friend class SkinningBuffer;
    friend class FRenderableManager;

    static void setBones(FEngine& engine, backend::Handle<backend::HwBufferObject> handle,
            RenderableManager::Bone const* transforms, size_t boneCount, size_t offset) noexcept;

    static void setBones(FEngine& engine, backend::Handle<backend::HwBufferObject> handle,
            math::mat4f const* transforms, size_t boneCount, size_t offset) noexcept;

    static PerRenderableBoneUib::BoneData makeBone(math::mat4f transform) noexcept;

    backend::Handle<backend::HwBufferObject> getHwHandle() const noexcept {
        return mHandle;
    }

    static backend::TextureHandle createIndicesAndWeightsHandle(FEngine& engine, size_t count);

    static void setIndicesAndWeightsData(FEngine& engine,
          backend::Handle<backend::HwTexture> textureHandle,
          const utils::FixedCapacityVector<math::float2>& pairs,
          size_t count);

    backend::Handle<backend::HwBufferObject> mHandle;
    uint32_t mBoneCount;
};

DANTE_DOWNCAST(SkinningBuffer)

} // namespace dante

#endif //TNT_DANTE_DETAILS_SKINNINGBUFFER_H
