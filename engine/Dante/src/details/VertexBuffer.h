
#ifndef TNT_DANTE_DETAILS_VERTEXBUFFER_H
#define TNT_DANTE_DETAILS_VERTEXBUFFER_H

#include "downcast.h"

#include <dante/MaterialEnums.h>
#include <dante/VertexBuffer.h>

#include <backend/BufferDescriptor.h>
#include <backend/DriverEnums.h>
#include <backend/Handle.h>

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>

namespace dante {

class FBufferObject;
class FEngine;

class FVertexBuffer : public VertexBuffer {
public:
    using VertexBufferInfoHandle = backend::VertexBufferInfoHandle;
    using VertexBufferHandle = backend::VertexBufferHandle;
    using BufferObjectHandle = backend::BufferObjectHandle;

    FVertexBuffer(FEngine& engine, const Builder& builder);

    // frees driver resources, object becomes invalid
    void terminate(FEngine& engine);

    VertexBufferHandle getHwHandle() const noexcept { return mHandle; }

    VertexBufferInfoHandle getVertexBufferInfoHandle() const { return mVertexBufferInfoHandle; }

    size_t getVertexCount() const noexcept;

    AttributeBitset getDeclaredAttributes() const noexcept {
        return mDeclaredAttributes;
    }

    // no-op if bufferIndex out of range
    void setBufferAt(FEngine& engine, uint8_t bufferIndex,
            backend::BufferDescriptor&& buffer, uint32_t byteOffset = 0);

    AsyncCallId setBufferAtAsync(FEngine& engine, uint8_t bufferIndex,
            backend::BufferDescriptor&& buffer, uint32_t byteOffset,
            backend::CallbackHandler* handler, AsyncCompletionCallback callback,
            void* user = nullptr);

    void setBufferObjectAt(FEngine& engine, uint8_t bufferIndex,
            FBufferObject const* bufferObject);

    AsyncCallId setBufferObjectAtAsync(FEngine& engine, uint8_t bufferIndex,
            FBufferObject const* bufferObject, backend::CallbackHandler* handler,
            AsyncCompletionCallback callback, void* user = nullptr);

    void updateBoneIndicesAndWeights(FEngine& engine, std::unique_ptr<uint16_t[]> skinJoints,
                                        std::unique_ptr<float[]> skinWeights);

    bool isCreationComplete() const noexcept { return mCreationComplete.load(std::memory_order_relaxed); }

private:
    friend class VertexBuffer;
    VertexBufferInfoHandle mVertexBufferInfoHandle;
    VertexBufferHandle mHandle;
    backend::AttributeArray mAttributes;
    std::array<BufferObjectHandle, backend::MAX_VERTEX_BUFFER_COUNT> mBufferObjects;
    AttributeBitset mDeclaredAttributes;
    uint32_t mVertexCount = 0;
    uint8_t mBufferCount = 0;
    bool mBufferObjectsEnabled = false;
    bool mAdvancedSkinningEnabled = false;

    // This field is set to true when the creation process is complete. This is especially useful
    // asynchronous creation. If we can guarantee that this field is only referenced by the main
    // thread, we don't have to use atomic here.
    std::atomic_bool mCreationComplete{ false };
};

DANTE_DOWNCAST(VertexBuffer)

} // namespace dante

#endif // TNT_DANTE_DETAILS_VERTEXBUFFER_H
