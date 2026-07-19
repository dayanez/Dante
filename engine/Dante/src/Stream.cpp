
#include "details/Stream.h"

namespace dante {

using namespace backend;

StreamType Stream::getStreamType() const noexcept {
    return downcast(this)->getStreamType();
}

void Stream::setAcquiredImage(void* image, Callback const callback, void* userdata, math::mat3f const& transform) noexcept {
    downcast(this)->setAcquiredImage(image, callback, userdata, transform);
}

void Stream::setAcquiredImage(void* image,
        CallbackHandler* handler, Callback const callback, void* userdata, math::mat3f const& transform) noexcept {
    downcast(this)->setAcquiredImage(image, handler, callback, userdata, transform);
}

void Stream::setDimensions(uint32_t const width, uint32_t const height) noexcept {
    downcast(this)->setDimensions(width, height);
}

int64_t Stream::getTimestamp() const noexcept {
    return downcast(this)->getTimestamp();
}

} // namespace dante
