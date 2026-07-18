/*
 * Copyright (C) 2018 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMAT_DIC_SPIRV_CHUNK_H
#define TNT_FILAMAT_DIC_SPIRV_CHUNK_H

#include "BlobDictionary.h"
#include "Chunk.h"
#include "Flattener.h"

#include <vector>

#include <stdint.h>

namespace filamat {

class DictionarySpirvChunk final : public Chunk {
public:
    explicit DictionarySpirvChunk(BlobDictionary&& dictionary, bool stripDebugInfo);
    ~DictionarySpirvChunk() = default;

private:
    void flatten(Flattener& f) override;

    BlobDictionary mDictionary;
    bool mStripDebugInfo;
};

} // namespace filamat

#endif // TNT_FILAMAT_DIC_SPIRV_CHUNK_H
