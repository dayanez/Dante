/*
 * Copyright (C) 2015 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IBL_CUBEMAPIBL_H
#define IBL_CUBEMAPIBL_H

#include <math/vec3.h>

#include <utils/Slice.h>
#include <utils/compiler.h>

#include <vector>

#include <stdint.h>
#include <stddef.h>

namespace utils {
class JobSystem;
} // namespace utils

namespace filament {
namespace ibl {

class Cubemap;
class Image;

/**
 * Generates cubemaps for the IBL.
 */
class UTILS_PUBLIC CubemapIBL {
public:
    typedef void (*Progress)(size_t, float, void*);

    /**
     * Computes a roughness LOD using prefiltered importance sampling GGX
     *
     * @param dst               the destination cubemap
     * @param levels            a list of prefiltered lods of the source environment
     * @param linearRoughness   roughness
     * @param maxNumSamples     number of samples for importance sampling
     * @param updater           a callback for the caller to track progress
     */
    static void roughnessFilter(
            utils::JobSystem& js, Cubemap& dst, utils::Slice<const Cubemap> levels,
            float linearRoughness, size_t maxNumSamples, math::float3 mirror, bool prefilter,
            Progress updater = nullptr, void* userdata = nullptr);

    static void roughnessFilter(
            utils::JobSystem& js, Cubemap& dst, const std::vector<Cubemap>& levels,
            float linearRoughness, size_t maxNumSamples, math::float3 mirror, bool prefilter,
            Progress updater = nullptr, void* userdata = nullptr);

    //! Computes the "DFG" term of the "split-sum" approximation and stores it in a 2D image
    static void DFG(utils::JobSystem& js, Image& dst, bool multiscatter, bool cloth);

    /**
     * Computes the diffuse irradiance using prefiltered importance sampling GGX
     *
     * @note Usually this is done using spherical harmonics instead.
     *
     * @param dst               the destination cubemap
     * @param levels            a list of prefiltered lods of the source environment
     * @param maxNumSamples     number of samples for importance sampling
     * @param updater           a callback for the caller to track progress
     *
     * @see CubemapSH
     */
    static void diffuseIrradiance(utils::JobSystem& js, Cubemap& dst, const std::vector<Cubemap>& levels,
            size_t maxNumSamples = 1024, Progress updater = nullptr, void* userdata = nullptr);

    // for debugging. ignore.
    static void brdf(utils::JobSystem& js, Cubemap& dst, float linearRoughness);
};

} // namespace ibl
} // namespace filament

#endif /* IBL_CUBEMAPIBL_H */
