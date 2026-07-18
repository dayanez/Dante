/*
 * Copyright (C) 2018 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_SAMPLE_SPHERE_H
#define TNT_FILAMENT_SAMPLE_SPHERE_H

#include <utils/Entity.h>

#include <math/vec3.h>

namespace filament {
class Engine;
class IndexBuffer;
class Material;
class MaterialInstance;
class VertexBuffer;
}

class Sphere {
public:
    Sphere( filament::Engine& engine,
            filament::Material const* material,
            bool culling = true);
    ~Sphere();

    Sphere(Sphere const&) = delete;
    Sphere& operator = (Sphere const&) = delete;

    Sphere(Sphere&& rhs) noexcept
            : mEngine(rhs.mEngine),
              mMaterialInstance(rhs.mMaterialInstance),
              mRenderable(rhs.mRenderable) {
        rhs.mMaterialInstance = {};
        rhs.mRenderable = {};
    }

    utils::Entity getSolidRenderable() const {
        return mRenderable;
    }

    filament::MaterialInstance* getMaterialInstance() {
        return mMaterialInstance;
    }

    Sphere& setPosition(filament::math::float3 const& position) noexcept;
    Sphere& setRadius(float radius) noexcept;

private:
    filament::Engine& mEngine;
    filament::MaterialInstance* mMaterialInstance = nullptr;
    utils::Entity mRenderable;

};

#endif //TNT_FILAMENT_SAMPLE_SPHERE_H
