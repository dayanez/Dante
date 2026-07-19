
#ifndef TNT_DANTE_DETAILS_BUFFEROBJECT_H
#define TNT_DANTE_DETAILS_BUFFEROBJECT_H

#include "downcast.h"

#include <dante/BufferObject.h>

#include <backend/Handle.h>

#include <utils/compiler.h>

namespace dante {

class FEngine;

class FBufferObject : public BufferObject {
public:
    FBufferObject(FEngine& engine, const Builder& builder);

    // frees driver resources, object becomes invalid
    void terminate(FEngine& engine);

    backend::Handle<backend::HwBufferObject> getHwHandle() const noexcept { return mHandle; }

    size_t getByteCount() const noexcept { return mByteCount; }

    BindingType getBindingType() const noexcept { return mBindingType; }

private:
    friend class BufferObject;
    void setBuffer(FEngine& engine, BufferDescriptor&& buffer, uint32_t byteOffset = 0);
    backend::Handle<backend::HwBufferObject> mHandle;
    uint32_t mByteCount;
    BindingType mBindingType;
};

DANTE_DOWNCAST(BufferObject)

} // namespace dante

#endif // TNT_DANTE_DETAILS_BUFFEROBJECT_H
