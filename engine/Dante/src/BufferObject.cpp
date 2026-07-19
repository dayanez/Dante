
#include "details/BufferObject.h"

#include "DanteAPI-impl.h"

#include "details/Engine.h"

namespace dante {

void BufferObject::setBuffer(Engine& engine,
        BufferDescriptor&& buffer, uint32_t const byteOffset) {
    downcast(this)->setBuffer(downcast(engine), std::move(buffer), byteOffset);
}

size_t BufferObject::getByteCount() const noexcept {
    return downcast(this)->getByteCount();
}

} // namespace dante
