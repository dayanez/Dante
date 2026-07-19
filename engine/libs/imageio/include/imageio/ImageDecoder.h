
#ifndef IMAGE_IMAGEDECODER_H_
#define IMAGE_IMAGEDECODER_H_

#include <iosfwd>
#include <string>

#include <image/LinearImage.h>

#include <utils/compiler.h>

namespace image {

class UTILS_PUBLIC ImageDecoder {
public:
    enum class ColorSpace {
        LINEAR,
        SRGB
    };

    // Returns linear floating-point data, or a non-valid image if an error occurred.
    static LinearImage decode(std::istream& stream, const std::string& sourceName,
            ColorSpace sourceSpace = ColorSpace::SRGB);

    class Decoder {
    public:
        virtual LinearImage decode() = 0;
        virtual ~Decoder() = default;

        ColorSpace getColorSpace() const noexcept {
            return mColorSpace;
        }

        void setColorSpace(ColorSpace colorSpace) noexcept {
            mColorSpace = colorSpace;
        }

    private:
        ColorSpace mColorSpace = ColorSpace::SRGB;
    };

private:
    enum class Format {
        NONE,
        PNG,
        HDR,
        PSD,
        EXR
    };
};

} // namespace image

#endif /* IMAGE_IMAGEDECODER_H_ */
