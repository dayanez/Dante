
#ifndef IBL_UTILITIES_H
#define IBL_UTILITIES_H

#include <math.h>

#include <math/vec2.h>
#include <math/vec3.h>

namespace dante {
namespace ibl {

template<typename T>
static inline constexpr T sq(T x) {
    return x * x;
}

template<typename T>
static inline constexpr T log4(T x) {
    // log2(x)/log2(4)
    // log2(x)/2
    return std::log2(x) * T(0.5);
}

inline bool isPOT(size_t x) {
    return !(x & (x - 1));
}

inline dante::math::float2 hammersley(uint32_t i, float iN) {
    constexpr float tof = 0.5f / 0x80000000U;
    uint32_t bits = i;
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return { i * iN, bits * tof };
}

} // namespace ibl
} // namespace dante
#endif /* IBL_UTILITIES_H */
