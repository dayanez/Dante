
#ifndef TNT_DANTE_DETAILS_INDIRECTLIGHT_H
#define TNT_DANTE_DETAILS_INDIRECTLIGHT_H

#include "downcast.h"

#include <dante/IndirectLight.h>
#include <dante/Texture.h>

#include <backend/Handle.h>

#include <utils/compiler.h>

#include <math/mat3.h>

#include <array>

namespace dante {

class FEngine;

class FIndirectLight : public IndirectLight {
public:
    static constexpr float DEFAULT_INTENSITY = 30000.0f;    // lux of the sun

    FIndirectLight(FEngine& engine, const Builder& builder) noexcept;

    void terminate(FEngine& engine);

    backend::Handle<backend::HwTexture> getReflectionHwHandle() const noexcept;
    backend::Handle<backend::HwTexture> getIrradianceHwHandle() const noexcept;
    math::float3 const* getSH() const noexcept{ return mIrradianceCoefs.data(); }
    float getIntensity() const noexcept { return mIntensity; }
    void setIntensity(float const intensity) noexcept { mIntensity = intensity; }
    void setRotation(math::mat3f const& rotation) noexcept { mRotation = rotation; }
    const math::mat3f& getRotation() const noexcept { return mRotation; }
    FTexture const* getReflectionsTexture() const noexcept { return mReflectionsTexture; }
    FTexture const* getIrradianceTexture() const noexcept { return mIrradianceTexture; }
    size_t getLevelCount() const noexcept { return mLevelCount; }
    math::float3 getDirectionEstimate() const noexcept;
    math::float4 getColorEstimate(math::float3 direction) const noexcept;
    static math::float3 getDirectionEstimate(const math::float3 sh[9]) noexcept;
    static math::float4 getColorEstimate(const math::float3 sh[9], math::float3 direction) noexcept;

private:
    FTexture const* mReflectionsTexture = nullptr;
    FTexture const* mIrradianceTexture = nullptr;
    std::array<math::float3, 9> mIrradianceCoefs;
    float mIntensity = DEFAULT_INTENSITY;
    math::mat3f mRotation;
    uint8_t mLevelCount = 0;
};

DANTE_DOWNCAST(IndirectLight)

} // namespace dante

#endif // TNT_DANTE_DETAILS_INDIRECTLIGHT_H
