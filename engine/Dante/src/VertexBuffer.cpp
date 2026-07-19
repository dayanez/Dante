
#include "details/VertexBuffer.h"

#include "details/Engine.h"

namespace dante {

size_t VertexBuffer::getVertexCount() const noexcept {
    return downcast(this)->getVertexCount();
}

void VertexBuffer::setBufferAt(Engine& engine, uint8_t const bufferIndex,
        backend::BufferDescriptor&& buffer, uint32_t const byteOffset) {
    downcast(this)->setBufferAt(downcast(engine), bufferIndex, std::move(buffer), byteOffset);
}

backend::AsyncCallId VertexBuffer::setBufferAtAsync(Engine& engine, uint8_t const bufferIndex,
        backend::BufferDescriptor&& buffer, uint32_t const byteOffset,
        backend::CallbackHandler* handler, AsyncCompletionCallback callback, void* user) {
    return downcast(this)->setBufferAtAsync(downcast(engine), bufferIndex, std::move(buffer), byteOffset,
            handler, std::move(callback), user);
}

void VertexBuffer::setBufferObjectAt(Engine& engine, uint8_t const bufferIndex,
        BufferObject const* bufferObject) {
    downcast(this)->setBufferObjectAt(downcast(engine), bufferIndex, downcast(bufferObject));
}

backend::AsyncCallId VertexBuffer::setBufferObjectAtAsync(Engine& engine, uint8_t const bufferIndex,
        BufferObject const* bufferObject, backend::CallbackHandler* handler,
        AsyncCompletionCallback callback, void* user) {
    return downcast(this)->setBufferObjectAtAsync(downcast(engine), bufferIndex,
            downcast(bufferObject), handler, std::move(callback), user);
}

bool VertexBuffer::isCreationComplete() const noexcept {
    return downcast(this)->isCreationComplete();
}

} // namespace dante
