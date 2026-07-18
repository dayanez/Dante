/*
 * Copyright (C) 2021 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IMAGE_HDRDECODER_H_
#define IMAGE_HDRDECODER_H_

#include <imageio/ImageDecoder.h>

#include <ios>

namespace image {

class HDRDecoder : public ImageDecoder::Decoder {
public:
    static HDRDecoder* create(std::istream& stream);
    static bool checkSignature(char const* buf);

    HDRDecoder(const HDRDecoder&) = delete;
    HDRDecoder& operator=(const HDRDecoder&) = delete;

private:
    explicit HDRDecoder(std::istream& stream);
    ~HDRDecoder() override;

    // ImageDecoder::Decoder interface
    LinearImage decode() override;

    static const char sigRadiance[];
    static const char sigRGBE[];
    std::istream& mStream;
    std::streampos mStreamStartPos;
};

} // namespace image

#endif /* IMAGE_IMAGEDECODER_H_ */
