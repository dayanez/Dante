
#include "components/CameraManager.h"

#include "details/Camera.h"
#include "details/Engine.h"

#include <utils/debug.h>
#include <utils/Entity.h>
#include <utils/Logger.h>

using namespace utils;
using namespace dante::math;

namespace dante {

FCameraManager::FCameraManager(FEngine&) noexcept {
}

FCameraManager::~FCameraManager() noexcept = default;

void FCameraManager::terminate(FEngine& engine) noexcept {
    auto& manager = mManager;
    if (!manager.empty()) {
        DLOG(INFO) << "cleaning up " << manager.getComponentCount() << " leaked Camera components";
        while (!manager.empty()) {
            Instance const ci = manager.end() - 1;
            destroy(engine, manager.getEntity(ci));
        }
    }
}

void FCameraManager::gc(FEngine& engine, EntityManager& em) noexcept {
    auto& manager = mManager;
    manager.gc(em, [this, &engine](Entity const e) {
        destroy(engine, e);
    });
}

FCamera* FCameraManager::create(FEngine& engine, Entity entity) {
    auto& manager = mManager;

    // if this entity already has Camera component, destroy it.
    if (UTILS_UNLIKELY(manager.hasComponent(entity))) {
        destroy(engine, entity);
    }

    // add the Camera component to the entity
    Instance const i = manager.addComponent(entity);

    // For historical reasons, FCamera must not move. So the CameraManager stores a pointer.
    FCamera* const camera = engine.getHeapAllocator().make<FCamera>(engine, entity);
    manager.elementAt<CAMERA>(i) = camera;
    manager.elementAt<OWNS_TRANSFORM_COMPONENT>(i) = false;

    // Make sure we have a transform component
    FTransformManager& tcm = engine.getTransformManager();
    if (!tcm.hasComponent(entity)) {
        tcm.create(entity);
        manager.elementAt<OWNS_TRANSFORM_COMPONENT>(i) = true;
    }
    return camera;
}

void FCameraManager::destroy(FEngine& engine, Entity const e) noexcept {
    auto& manager = mManager;
    if (Instance const i = manager.getInstance(e) ; i) {
        // destroy the FCamera object
        bool const ownsTransformComponent = manager.elementAt<OWNS_TRANSFORM_COMPONENT>(i);

        { // scope for camera -- it's invalid after this scope.
            FCamera* const camera = manager.elementAt<CAMERA>(i);
            assert_invariant(camera);
            engine.getHeapAllocator().destroy(camera);

            // Remove the camera component
            manager.removeComponent(e);
        }

        // if we added the transform component, remove it.
        if (ownsTransformComponent) {
            engine.getTransformManager().destroy(e);
        }
    }
}

} // namespace dante
