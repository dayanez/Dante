
#include "fg/FrameGraphTexture.h"

#include "TextureCache.h"

#include <utils/StaticString.h>

#include <algorithm>
#include <array>

namespace dante {

void FrameGraphTexture::create(TextureCacheInterface& textureCache,
        utils::StaticString const name,
        Descriptor const& descriptor, Usage usage) noexcept {
    std::array const swizzle = {
            descriptor.swizzle.r,
            descriptor.swizzle.g,
            descriptor.swizzle.b,
            descriptor.swizzle.a };
    handle = textureCache.createTexture(name,
            descriptor.type, descriptor.levels, descriptor.format, descriptor.samples,
            descriptor.width, descriptor.height, descriptor.depth,
            swizzle, usage);
}

void FrameGraphTexture::destroy(TextureCacheInterface& textureCache) noexcept {
    if (handle) {
        textureCache.destroyTexture(handle);
        handle.clear();
    }
}

FrameGraphTexture::Descriptor FrameGraphTexture::generateSubResourceDescriptor(
        Descriptor descriptor,
        SubResourceDescriptor const& srd) noexcept {
    descriptor.levels = 1;
    descriptor.width  = std::max(1u, descriptor.width >> srd.level);
    descriptor.height = std::max(1u, descriptor.height >> srd.level);
    return descriptor;
}

} // namespace dante
