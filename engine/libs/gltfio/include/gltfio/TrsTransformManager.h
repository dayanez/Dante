/*
 * Copyright (C) 2023 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef GLTFIO_TRSTRANSFORMMANAGER_H
#define GLTFIO_TRSTRANSFORMMANAGER_H

#include <filament/FilamentAPI.h>

#include <utils/compiler.h>
#include <utils/EntityInstance.h>

#include <math/mat4.h>
#include <math/quat.h>
#include <math/vec3.h>

using namespace filament::math;

namespace utils {
class Entity;
} // namespace utils

namespace filament::gltfio {

class FTrsTransformManager;

/**
 * TrsTransformManager is used to add entities with glTF-specific trs information.
 *
 * Trs information here just used for Animation, DON'T use for transform.
 */
class UTILS_PUBLIC TrsTransformManager {
public:
    using Instance = utils::EntityInstance<TrsTransformManager>;
    using Entity = utils::Entity;

    /**
     * Returns whether a particular Entity is associated with a component of this TrsTransformManager
     * @param e An Entity.
     * @return true if this Entity has a component associated with this manager.
     */
    bool hasComponent(Entity e) const noexcept;

    /**
     * Gets an Instance representing the trs transform component associated with the given Entity.
     * @param e An Entity.
     * @return An Instance object, which represents the trs transform component associated with the Entity e.
     * @note Use Instance::isValid() to make sure the component exists.
     * @see hasComponent()
     */
    Instance getInstance(Entity e) const noexcept;

    /**
     * Creates a trs transform component and associates it with the given entity.
     * @param entity            An Entity to associate a trs transform component with.
     * @param translation       The translation to initialize the trs transform component with.
     * @param rotation          The rotation to initialize the trs transform component with.
     * @param scale             The scale to initialize the trs transform component with.
     *
     * If this component already exists on the given entity, it is first destroyed as if
     * destroy(Entity e) was called.
     *
     * @see destroy()
     */
    void create(Entity entity);
    void create(Entity entity, const float3& translation, const quatf& rotation,
            const float3& scale);  //!< \overload

    /**
     * Destroys this component from the given entity.
     * @param e An entity.
     *
     * @see create()
     */
    void destroy(Entity e) noexcept;

    void setTranslation(Instance ci, const float3& translation) noexcept;
    const float3& getTranslation(Instance ci) const noexcept;

    void setRotation(Instance ci, const quatf& rotation) noexcept;
    const quatf& getRotation(Instance ci) const noexcept;

    void setScale(Instance ci, const float3& scale) noexcept;
    const float3& getScale(Instance ci) const noexcept;

    void setTrs(Instance ci, const float3& translation, const quatf& rotation,
            const float3& scale) noexcept;
    const mat4f getTransform(Instance ci) const noexcept;

protected:
    TrsTransformManager() noexcept = default;
    ~TrsTransformManager() = default;

public:
    TrsTransformManager(TrsTransformManager const&) = delete;
    TrsTransformManager(TrsTransformManager&&) = delete;
    TrsTransformManager& operator=(TrsTransformManager const&) = delete;
    TrsTransformManager& operator=(TrsTransformManager&&) = delete;
};

} // namespace filament::gltfio

#endif // GLTFIO_TRSTRANSFORMMANAGER_H
