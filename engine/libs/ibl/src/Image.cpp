/*
 * Copyright (C) 2015 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ibl/Image.h>

#include <utility>

namespace filament {
namespace ibl {

Image::Image() = default;

Image::Image(size_t w, size_t h, size_t stride)
        : mBpr((stride ? stride : w) * sizeof(math::float3)),
          mWidth(w),
          mHeight(h),
          mOwnedData(new uint8_t[mBpr * h]),
          mData(mOwnedData.get()) {
}

void Image::reset() {
    mOwnedData.release();
    mWidth = 0;
    mHeight = 0;
    mBpr = 0;
    mData = nullptr;
}

void Image::set(Image const& image) {
    mOwnedData.release();
    mWidth = image.mWidth;
    mHeight = image.mHeight;
    mBpr = image.mBpr;
    mData = image.mData;
}

void Image::subset(Image const& image, size_t x, size_t y, size_t w, size_t h) {
    mOwnedData.release();
    mWidth = w;
    mHeight = h;
    mBpr = image.mBpr;
    mData = static_cast<uint8_t*>(image.getPixelRef(x, y));
}

} // namespace ibl
} // namespace filament

