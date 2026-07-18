/*
 * Copyright (C) 2023 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_BACKEND_OPENGLBLOBCACHE_H
#define TNT_FILAMENT_BACKEND_OPENGLBLOBCACHE_H

#include "BlobCacheKey.h"
#include "gl_headers.h"

namespace filament::backend {

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

} // namespace filament::backend

#endif // TNT_FILAMENT_BACKEND_OPENGLBLOBCACHE_H
