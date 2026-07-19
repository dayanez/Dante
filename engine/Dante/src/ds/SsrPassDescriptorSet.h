
#ifndef TNT_DANTE_SSRPASSDESCRIPTORSET_H
#define TNT_DANTE_SSRPASSDESCRIPTORSET_H

#include "DescriptorSet.h"
#include "TypedUniformBuffer.h"

#include <private/dante/UibStructs.h>

#include <backend/DriverApiForward.h>
#include <backend/Handle.h>

namespace dante {

class FEngine;

struct ScreenSpaceReflectionsOptions;

class SsrPassDescriptorSet {

    using TextureHandle = backend::Handle<backend::HwTexture>;

public:
    SsrPassDescriptorSet() noexcept;

    void init(FEngine& engine) noexcept;

    void terminate(backend::DriverApi& driver);

    void setFrameUniforms(FEngine const& engine, TypedUniformBuffer<PerViewUib>& uniforms) noexcept;

    void prepareStructure(FEngine const& engine, TextureHandle structure) noexcept;

    void prepareHistorySSR(FEngine const& engine, TextureHandle ssr) noexcept;

    // update local data into GPU UBO
    void commit(FEngine& engine) noexcept;

    // bind this descriptor set
    void bind(backend::DriverApi& driver) noexcept;

private:
    DescriptorSet mDescriptorSet;
    backend::BufferObjectHandle mShadowUbh;
};

} // namespace dante

#endif //TNT_DANTE_SSRPASSDESCRIPTORSET_H
