/*
 * Copyright (C) 2020 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CAMUTILS_BOOKMARK_H
#define CAMUTILS_BOOKMARK_H

#include <camutils/compiler.h>

#include <math/vec2.h>
#include <math/vec3.h>

namespace filament {
namespace camutils {

template <typename FLOAT> class FreeFlightManipulator;
template <typename FLOAT> class OrbitManipulator;
template <typename FLOAT> class MapManipulator;
template <typename FLOAT> class Manipulator;

enum class Mode { ORBIT, MAP, FREE_FLIGHT };

/**
 * Opaque memento to a viewing position and orientation (e.g. the "home" camera position).
 *
 * This little struct is meant to be passed around by value and can be used to track camera
 * animation between waypoints. In map mode this implements Van Wijk interpolation.
 *
 * @see Manipulator::getCurrentBookmark, Manipulator::jumpToBookmark
 */
template <typename FLOAT>
struct CAMUTILS_PUBLIC Bookmark {
    /**
     * Interpolates between two bookmarks. The t argument must be between 0 and 1 (inclusive), and
     * the two endpoints must have the same mode (ORBIT or MAP).
     */
    static Bookmark<FLOAT> interpolate(Bookmark<FLOAT> a, Bookmark<FLOAT> b, double t);

    /**
     * Recommends a duration for animation between two MAP endpoints. The return value is a unitless
     * multiplier.
     */
    static double duration(Bookmark<FLOAT> a, Bookmark<FLOAT> b);

private:
    struct MapParams {
        FLOAT extent;
        filament::math::vec2<FLOAT> center;
    };
    struct OrbitParams {
        FLOAT phi;
        FLOAT theta;
        FLOAT distance;
        filament::math::vec3<FLOAT> pivot;
    };
    struct FlightParams {
        FLOAT pitch;
        FLOAT yaw;
        filament::math::vec3<FLOAT> position;
    };
    Mode mode;
    MapParams map;
    OrbitParams orbit;
    FlightParams flight;
    friend class FreeFlightManipulator<FLOAT>;
    friend class OrbitManipulator<FLOAT>;
    friend class MapManipulator<FLOAT>;
};

} // namespace camutils
} // namespace filament

#endif // CAMUTILS_BOOKMARK_H
