
#ifndef TNT_DANTE_BACKEND_OPENGLBLOBCACHE_H
#define TNT_DANTE_BACKEND_OPENGLBLOBCACHE_H

#include "BlobCacheKey.h"
#include "gl_headers.h"

namespace dante::backend {

class Platform;
class Program;
class OpenGLContext;

class OpenGLBlobCache {
public:
    explicit OpenGLBlobCache(OpenGLContext& gl) noexcept;

    GLuint retrieve(BlobCacheKey* key, Platform& platform,
            Program const& program) const noexcept;

    void insert(Platform& platform,
            BlobCacheKey const& key, GLuint program) noexcept;

private:
    struct Blob;
    bool mCachingSupported = false;
};

} // namespace dante::backend

#endif // TNT_DANTE_BACKEND_OPENGLBLOBCACHE_H
