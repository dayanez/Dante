/*
 * Copyright (C) 2025 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_ZSTD_HELPER_H
#define TNT_FILAMENT_ZSTD_HELPER_H

#include <stddef.h>

namespace filament {

class ZstdHelper {
public:
    /**
     * Checks if the given binary blob is Zstd compressed.
     * @param src Pointer to the source data.
     * @param src_size Size of the source data.
     * @return True if the data is Zstd compressed, false otherwise.
     */
    static bool isCompressed(const void* src, size_t src_size) noexcept;

    /**
     * Returns the decompressed size of a Zstd compressed blob.
     * @param src Pointer to the source data.
     * @param src_size Size of the source data.
     * @return The decompressed size, or 0 if an error occurs.
     */
    static size_t getDecodedSize(const void* src, size_t src_size) noexcept;

    /**
     * Decompresses a Zstd compressed blob into a pre-allocated buffer.
     * @param dst Pointer to the destination buffer.
     * @param dst_size Size of the destination buffer.
     * @param src Pointer to the source data.
     * @param src_size Size of the source data.
     * @return The number of bytes decompressed, or 0 if an error occurs.
     */
    static size_t decompress(void* dst, size_t dst_size, const void* src, size_t src_size) noexcept;
};

} // namespace filament

#endif // TNT_FILAMENT_ZSTD_HELPER_H
