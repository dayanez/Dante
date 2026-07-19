
#include "details/RenderTarget.h"

#include "details/Texture.h"

namespace dante {

using namespace backend;

Texture* RenderTarget::getTexture(AttachmentPoint const attachment) const noexcept {
    return downcast(this)->getAttachment(attachment).texture;
}

uint8_t RenderTarget::getMipLevel(AttachmentPoint const attachment) const noexcept {
    return downcast(this)->getAttachment(attachment).mipLevel;
}

RenderTarget::CubemapFace RenderTarget::getFace(AttachmentPoint const attachment) const noexcept {
    return downcast(this)->getAttachment(attachment).face;
}

uint32_t RenderTarget::getLayer(AttachmentPoint const attachment) const noexcept {
    return downcast(this)->getAttachment(attachment).layer;
}

uint8_t RenderTarget::getSupportedColorAttachmentsCount() const noexcept {
    return downcast(this)->getSupportedColorAttachmentsCount();
}

} // namespace dante
