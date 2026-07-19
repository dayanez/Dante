
#ifndef DANTE_MATERIALS_SSAO_UTILS
#define DANTE_MATERIALS_SSAO_UTILS

#include "../utils/depthUtils.fs"

vec2 pack(highp float normalizedDepth) {
    // we need 16-bits of precision
    highp float z = clamp(normalizedDepth, 0.0, 1.0);
    highp float t = floor(256.0 * z);
    mediump float hi = t * (1.0 / 256.0);   // we only need 8-bits of precision
    mediump float lo = (256.0 * z) - t;     // we only need 8-bits of precision
    return vec2(hi, lo);
}

highp float unpack(highp vec2 depth) {
    // depth here only has 8-bits of precision, but the unpacked depth is highp
    // this is equivalent to (x8 * 256 + y8) / 65535, which gives a value between 0 and 1
    return (depth.x * (256.0 / 257.0) + depth.y * (1.0 / 257.0));
}

vec3 packBentNormal(vec3 bn) {
    return bn * 0.5 + 0.5;
}

vec3 unpackBentNormal(vec3 bn) {
    return bn * 2.0 - 1.0;
}


#endif // DANTE_MATERIALS_SSAO_UTILS

