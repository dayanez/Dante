/*
 * Copyright (C) 2017 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMAT_CHUNK_H
#define TNT_FILAMAT_CHUNK_H

#include "Flattener.h"

#include <filament/MaterialChunkType.h>

#include <vector>

namespace filamat {

class Chunk{
public:
    virtual ~Chunk();

    ChunkType getType() const noexcept {
        return mType;
    }

    virtual void flatten(Flattener &f) = 0;

protected:
    explicit Chunk(ChunkType type) : mType(type) {
    }

private:
    ChunkType mType;
};

} // namespace filamat
#endif // TNT_FILAMAT_CHUNK_H