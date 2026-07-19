
#ifndef TNT_GEOMETRY_TRANSCODER_H
#define TNT_GEOMETRY_TRANSCODER_H

#include <utils/compiler.h>

#include <stddef.h>
#include <stdint.h>

namespace dante {
namespace geometry {

enum class ComponentType {
    BYTE,   //!< If normalization is enabled, this maps from [-127,127] to [-1,+1]
    UBYTE,  //!< If normalization is enabled, this maps from [0,255] to [0, +1]
    SHORT,  //!< If normalization is enabled, this maps from [-32767,32767] to [-1,+1]
    USHORT, //!< If normalization is enabled, this maps from [0,65535] to [0, +1]
    HALF,   //!< 1 sign bit, 5 exponent bits, and 5 mantissa bits.
    FLOAT,  //!< Standard 32-bit float
};

/**
 * Creates a function object that can convert vertex attribute data into tightly packed floats.
 *
 * This is especially useful for 3-component formats which are not supported by all backends.
 * e.g. The Vulkan minspec includes float3 but not short3.
 *
 * Usage Example:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * using dante::geometry::Transcoder;
 * using dante::geometry::ComponentType;
 *
 * Transcoder transcode({
 *     .componentType = ComponentType::BYTE,
 *     .normalized = true,
 *     .componentCount = 3,
 *     .inputStrideBytes = 0
 * });
 *
 * transcode(outputPtr, inputPtr, count);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * The interpretation of signed normalized data is consistent with Vulkan and OpenGL ES 3.0+.
 * Note that this slightly differs from earlier versions of OpenGL ES.  For example, a signed byte
 * value of -127 maps exactly to -1.0f under ES3 and VK rules, but not ES2.
 */
class UTILS_PUBLIC Transcoder {
public:
    /**
     * Describes the format of all input data that get passed to this transcoder object.
     */
    struct Config {
        ComponentType componentType;
        bool normalized;
        uint32_t componentCount;
        uint32_t inputStrideBytes = 0; //!< If stride is 0, the transcoder assumes tight packing.
    };

    /**
     * Creates an immutable function object with the specified configuration.
     *
     * The config is not passed by const reference to allow for type inference at the call site.
     */
    Transcoder(Config config) noexcept : mConfig(config) {}

    /**
     * Converts arbitrary data into tightly packed 32-bit floating point values.
     *
     * If target is non-null, writes up to "count" items into target and returns the number of bytes
     * actually written.
     *
     * If target is null, returns the number of bytes required.
     *
     * @param target Client owned area to write into, or null for a size query
     * @param source Pointer to the data to read from (does not get retained)
     * @param count The maximum number of items to write (i.e. number of float3 values, not bytes)
     * @return Number of bytes required to contain "count" items after conversion to packed floats
     *
     */
    size_t operator()(float* UTILS_RESTRICT target, void const* UTILS_RESTRICT source,
            size_t count) const noexcept;

private:
    const Config mConfig;
};

} // namespace geometry
} // namespace dante

#endif // TNT_GEOMETRY_TRANSCODER_H
