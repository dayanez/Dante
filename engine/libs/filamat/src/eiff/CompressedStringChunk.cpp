/*
 * Copyright (C) 2025 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "CompressedStringChunk.h"

#include <zstd.h>

#include <stdint.h>

namespace filamat {

namespace {
int toZstdCompressionLevel(CompressedStringChunk::CompressionLevel compressionLevel) {
    switch (compressionLevel) {
        case CompressedStringChunk::CompressionLevel::MIN:
            return ZSTD_minCLevel();
        case CompressedStringChunk::CompressionLevel::MAX:
            return ZSTD_maxCLevel();
        case CompressedStringChunk::CompressionLevel::DEFAULT:
            return ZSTD_defaultCLevel();
    }
}
} // namespace

void CompressedStringChunk::flatten(filamat::Flattener& f) {
    const size_t bufferBound = ZSTD_compressBound(mString.size());
    std::vector<uint8_t> compressed(bufferBound);

    const size_t compressedSize = ZSTD_compress(
            compressed.data(), compressed.size(),
            mString.data(), mString.size(),
            toZstdCompressionLevel(mCompressionLevel));

    if (ZSTD_isError(compressedSize)) {
        utils::slog.e << "Error compressing the input string." << utils::io::endl;
        return;
    }

    f.writeBlob((const char*) compressed.data(), compressedSize);
}

} // namespace filamat