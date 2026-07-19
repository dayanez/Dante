
#ifndef TNT_DANTE_DETAILS_SKYBOX_H
#define TNT_DANTE_DETAILS_SKYBOX_H

#include "downcast.h"

#include <dante/Skybox.h>

#include <private/backend/DriverApi.h>

#include <utils/compiler.h>
#include <utils/Entity.h>

namespace dante {

class FEngine;
class FTexture;
class FMaterial;
class FMaterialInstance;
class FRenderableManager;

class FSkybox : public Skybox {
public:
    FSkybox(FEngine& engine, const Builder& builder) noexcept;

    static FMaterial const* createMaterial(FEngine& engine);

    void terminate(FEngine& engine) noexcept;

    utils::Entity getEntity() const noexcept { return mSkybox; }

    void setLayerMask(uint8_t select, uint8_t values) noexcept;
    uint8_t getLayerMask() const noexcept { return mLayerMask; }

    float getIntensity() const noexcept { return mIntensity; }

    FTexture const* getTexture() const noexcept { return mSkyboxTexture; }

    void setColor(math::float4 color) noexcept;

private:
    // we don't own these
    FTexture const* mSkyboxTexture = nullptr;

    // we own these
    FMaterialInstance* mSkyboxMaterialInstance = nullptr;
    utils::Entity mSkybox;
    FRenderableManager& mRenderableManager;
    float mIntensity = 0.0f;
    uint8_t mLayerMask = 0x1;
};

DANTE_DOWNCAST(Skybox)

} // namespace dante


#endif // TNT_DANTE_DETAILS_SKYBOX_H
