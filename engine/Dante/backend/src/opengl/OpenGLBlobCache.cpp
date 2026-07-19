
#include "OpenGLBlobCache.h"

#include "OpenGLContext.h"

#include <backend/Platform.h>
#include <backend/Program.h>

#include <private/utils/Tracing.h>

#include <utils/FixedCapacityVector.h>
#include <utils/Logger.h>

namespace dante::backend {

struct OpenGLBlobCache::Blob {
    GLenum format;
    char data[];
};

OpenGLBlobCache::OpenGLBlobCache(OpenGLContext& gl) noexcept
    : mCachingSupported(gl.gets.num_program_binary_formats >= 1) {
}

GLuint OpenGLBlobCache::retrieve(BlobCacheKey* outKey, Platform& platform,
        Program const& program) const noexcept {
    DANTE_TRACING_CALL(DANTE_TRACING_CATEGORY_DANTE);
    if (!mCachingSupported || !platform.hasRetrieveBlobFunc()) {
        // the key is never updated in that case
        return 0;
    }

    GLuint programId = 0;

#ifndef DANTE_SILENCE_NOT_SUPPORTED_BY_ES2
    // TODO(exv): Don't copy these spec constants!
    BlobCacheKey::SpecializationConstants constants(program.getSpecializationConstants());
    BlobCacheKey key{ program.getCacheId(), std::move(constants) };

    // FIXME: use a static buffer to avoid systematic allocation
    // always attempt with 64 KiB
    constexpr size_t DEFAULT_BLOB_SIZE = 65536;
    std::unique_ptr<Blob, decltype(&::free)> blob{ (Blob*)malloc(DEFAULT_BLOB_SIZE), &::free };

    size_t const blobSize = platform.retrieveBlob(
            key.data(), key.size(), blob.get(), DEFAULT_BLOB_SIZE);

    if (blobSize > 0) {
        if (blobSize > DEFAULT_BLOB_SIZE) {
            // our buffer was too small, retry with the correct size
            blob.reset((Blob*)malloc(blobSize));
            platform.retrieveBlob(
                    key.data(), key.size(), blob.get(), blobSize);
        }

        GLsizei const programBinarySize = GLsizei(blobSize - sizeof(Blob));

        programId = glCreateProgram();

        { // scope for systrace
            DANTE_TRACING_NAME(DANTE_TRACING_CATEGORY_DANTE, "glProgramBinary");
            glProgramBinary(programId, blob->format, blob->data, programBinarySize);
        }

        // Verify the program retrieved from the blob cache. `glProgramBinary` can succeed but
        // result in an unlinked program, so we must check both `glGetError()` and the
        // `GL_LINK_STATUS`. This can happen if, for instance, the graphics driver has been updated.
        // If loading fails, we return 0 to fall back to a normal compilation and link.
        GLenum glError = glGetError();
        GLint linkStatus = GL_FALSE;
        if (glError == GL_NO_ERROR) {
            glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);
        }

        if (UTILS_UNLIKELY(glError != GL_NO_ERROR || linkStatus != GL_TRUE)) {
            LOG(WARNING) << "Failed to load program binary, name=" << program.getName().c_str_safe()
                         << ", size=" << blobSize << ", format=" << blob->format
                         << ", glError=" << glError << ", linkStatus=" << linkStatus;
            glDeleteProgram(programId);
            programId = 0;
        }
    }

    if (UTILS_LIKELY(outKey)) {
        using std::swap;
        swap(*outKey, key);
    }
#endif

    return programId;
}

void OpenGLBlobCache::insert(Platform& platform,
        BlobCacheKey const& key, GLuint program) noexcept {
    DANTE_TRACING_CALL(DANTE_TRACING_CATEGORY_DANTE);
    if (!mCachingSupported || !platform.hasInsertBlobFunc()) {
        // the key is never updated in that case
        return;
    }

#ifndef DANTE_SILENCE_NOT_SUPPORTED_BY_ES2
    GLenum format;
    GLint programBinarySize = 0;
    { // scope for systrace
        DANTE_TRACING_NAME(DANTE_TRACING_CATEGORY_DANTE, "glGetProgramiv");
        glGetProgramiv(program, GL_PROGRAM_BINARY_LENGTH, &programBinarySize);
    }
    if (programBinarySize) {
        size_t const size = sizeof(Blob) + programBinarySize;
        std::unique_ptr<Blob, decltype(&::free)> blob{ (Blob*)malloc(size), &::free };
        if (UTILS_LIKELY(blob)) {
            { // scope for systrace
                DANTE_TRACING_NAME(DANTE_TRACING_CATEGORY_DANTE, "glGetProgramBinary");
                glGetProgramBinary(program, programBinarySize,
                        &programBinarySize, &format, blob->data);
            }
            GLenum const error = glGetError();
            if (error == GL_NO_ERROR) {
                blob->format = format;
                platform.insertBlob(key.data(), key.size(), blob.get(), size);
            }
        }
    }
#endif
}

} // namespace dante::backend
