
#ifndef TNT_DANTE_COMPONENTS_CAMERAMANAGER_H
#define TNT_DANTE_COMPONENTS_CAMERAMANAGER_H

#include "downcast.h"

#include <dante/DanteAPI.h>

#include <utils/compiler.h>
#include <utils/Entity.h>
#include <utils/SingleInstanceComponentManager.h>

namespace dante {

class CameraManager : public DanteAPI {
public:
    using Instance = utils::EntityInstance<CameraManager>;
};

class FEngine;
class FCamera;

class UTILS_PRIVATE FCameraManager : public CameraManager {
public:
    using Instance = Instance;

    explicit FCameraManager(FEngine& engine) noexcept;

    ~FCameraManager() noexcept;

    // free-up all resources
    void terminate(FEngine& engine) noexcept;

    void gc(FEngine& engine, utils::EntityManager& em) noexcept;

    /*
    * Component Manager APIs
    */

    bool hasComponent(utils::Entity const e) const noexcept {
        return mManager.hasComponent(e);
    }

    Instance getInstance(utils::Entity const e) const noexcept {
        return { mManager.getInstance(e) };
    }

    size_t getComponentCount() const noexcept {
        return mManager.getComponentCount();
    }

    bool empty() const noexcept {
        return mManager.empty();
    }

    utils::Entity getEntity(Instance const i) const noexcept {
        return mManager.getEntity(i);
    }

    utils::Entity const* getEntities() const noexcept {
        return mManager.getEntities();
    }

    FCamera* getCamera(Instance const i) noexcept {
        return mManager.elementAt<CAMERA>(i);
    }

    FCamera* create(FEngine& engine, utils::Entity entity);

    void destroy(FEngine& engine, utils::Entity e) noexcept;

private:

    enum {
        CAMERA,
        OWNS_TRANSFORM_COMPONENT
    };

    using Base = utils::SingleInstanceComponentManager<FCamera*, bool>;

    struct CameraManagerImpl : public Base {
        using Base::gc;
        using Base::swap;
        using Base::hasComponent;
    } mManager;
};

} // namespace dante

#endif // TNT_DANTE_COMPONENTS_CAMERAMANAGER_H
