/*
 * Copyright (C) 2025 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_SAMPLE_GRID_H
#define TNT_FILAMENT_SAMPLE_GRID_H

#include <filament/Box.h>
#include <filament/Camera.h>
#include <filament/Engine.h>
#include <filament/Material.h>
#include <filament/MaterialInstance.h>

#include <utils/Entity.h>

#include <math/mat4.h>
#include <math/vec3.h>

#include <functional>

class Grid {
public:

    Grid(filament::Engine& engine, filament::Material const* material,
        filament::math::float3 linearColor);

    Grid(Grid const&) = delete;
    Grid& operator=(Grid const&) = delete;

    Grid(Grid&& rhs) noexcept;

    utils::Entity getWireFrameRenderable() const {
        return mWireFrameRenderable;
    }

    ~Grid();

    using Generator = std::function<float(int index)>;

    void update(uint32_t width, uint32_t height, uint32_t depth);

    void update(uint32_t width, uint32_t height, uint32_t depth,
            Generator const& genWidth, Generator const& genHeight, Generator const& genDepth);

    void mapFrustum(filament::Engine& engine, filament::Camera const* camera);
    void mapFrustum(filament::Engine& engine, filament::math::mat4 const& transform);
    void mapAabb(filament::Engine& engine, filament::Box const& box);

private:
    filament::Engine& mEngine;
    filament::VertexBuffer* mVertexBuffer = nullptr;
    filament::IndexBuffer* mIndexBuffer = nullptr;
    filament::Material const* mMaterial = nullptr;
    filament::MaterialInstance* mMaterialInstanceWireFrame = nullptr;
    utils::Entity mWireFrameRenderable{};
};


#endif // TNT_FILAMENT_SAMPLE_GRID_H
