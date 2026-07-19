
#ifndef TNT_DANTE_BACKEND_OPENGL_GLMEMORYMAPPEDBUFFER_H
#define TNT_DANTE_BACKEND_OPENGL_GLMEMORYMAPPEDBUFFER_H

#include "DriverBase.h"
#include "gl_headers.h"

#include <private/backend/HandleAllocator.h>

#include <backend/DriverEnums.h>
#include <backend/Handle.h>

#include <stddef.h>
#include <stdint.h>

namespace dante::backend {

class OpenGLState;
class OpenGLDriver;
class BufferDescriptor;

struct GLBufferObject;

struct GLMemoryMappedBuffer : public HwMemoryMappedBuffer {
    BufferObjectHandle boh;
    MapBufferAccessFlags access;
    struct {
        void* vaddr = nullptr;
        uint32_t size = 0;
        uint32_t offset = 0;
        GLenum binding = 0;
        GLuint id = 0;
    } gl;

    GLMemoryMappedBuffer();

    GLMemoryMappedBuffer(OpenGLState& gls, HandleAllocatorGL& handleAllocator,
            BufferObjectHandle boh, size_t offset, size_t size, MapBufferAccessFlags access);

    ~GLMemoryMappedBuffer();

    void unmap(OpenGLState& gls, HandleAllocatorGL& handleAllocator) const;

    void copy(OpenGLState& gls, OpenGLDriver& gld,
            size_t offset, BufferDescriptor&& data) const;
};

} // namespace dante::backend

#endif //TNT_DANTE_BACKEND_OPENGL_GLMEMORYMAPPEDBUFFER_H
