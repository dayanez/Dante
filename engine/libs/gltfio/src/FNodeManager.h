
#ifndef GLTFIO_FNODEMANAGER_H
#define GLTFIO_FNODEMANAGER_H

#include "downcast.h"

#include <gltfio/NodeManager.h>

#include <utils/compiler.h>
#include <utils/debug.h>
#include <utils/Entity.h>
#include <utils/SingleInstanceComponentManager.h>
#include <utils/Slice.h>

namespace dante::gltfio {

class UTILS_PRIVATE FNodeManager : public NodeManager {
public:
    using Instance = NodeManager::Instance;

    FNodeManager() noexcept {}

    ~FNodeManager() noexcept {
        assert_invariant(mManager.getComponentCount() == 0);
    }

    void terminate() noexcept;

    bool hasComponent(utils::Entity e) const noexcept {
        return mManager.hasComponent(e);
    }

    Instance getInstance(utils::Entity e) const noexcept {
        return Instance(mManager.getInstance(e));
    }

    void create(utils::Entity entity) {
        if (UTILS_UNLIKELY(mManager.hasComponent(entity))) {
            destroy(entity);
        }
        UTILS_UNUSED_IN_RELEASE Instance ci = mManager.addComponent(entity);
        assert_invariant(ci);
    }

    void destroy(utils::Entity e) noexcept {
        if (Instance const ci = mManager.getInstance(e); ci) {
            mManager.removeComponent(e);
        }
    }

    void gc(utils::EntityManager& em) noexcept {
        mManager.gc(em, [this](Entity e) {
            destroy(e);
        });
    }

    void setMorphTargetNames(Instance ci, utils::FixedCapacityVector<CString> names) noexcept {
        assert_invariant(ci.isValid());
        mManager[ci].morphTargetNames = std::move(names);
    }

    const utils::FixedCapacityVector<CString>& getMorphTargetNames(Instance ci) const noexcept {
        return mManager[ci].morphTargetNames;
    }

    void setExtras(Instance ci, CString extras) noexcept {
        mManager[ci].extras = std::move(extras);
    }

    const CString& getExtras(Instance ci) const noexcept {
        return mManager[ci].extras;
    }

    void setSceneMembership(Instance ci, SceneMask scenes) noexcept {
        mManager[ci].scenes = scenes;
    }

    SceneMask getSceneMembership(Instance ci) const noexcept {
        return mManager[ci].scenes;
    }

private:
    enum {
        MORPH_TARGET_NAMES,
        EXTRAS_STRING,
        SCENE_MEMBERSHIP,
    };

    using Base = utils::SingleInstanceComponentManager<  // 28 bytes
            utils::FixedCapacityVector<CString>,  // 16
            CString,                              // 8
            SceneMask>;                           // 4

    struct Sim : public Base {
        using Base::gc;
        using Base::swap;

        typename Base::SoA& getSoA() { return mData; }

        struct Proxy {
            UTILS_ALWAYS_INLINE
            Proxy(Base& sim, utils::EntityInstanceBase::Type i) noexcept :
                    morphTargetNames{ sim, i } { }

            union {
                Field<MORPH_TARGET_NAMES>   morphTargetNames;
                Field<EXTRAS_STRING>        extras;
                Field<SCENE_MEMBERSHIP>     scenes;
            };
        };

        UTILS_ALWAYS_INLINE Proxy operator[](Instance i) noexcept {
            return { *this, i };
        }
        UTILS_ALWAYS_INLINE const Proxy operator[](Instance i) const noexcept {
            return { const_cast<Sim&>(*this), i };
        }
    };

    Sim mManager;
};

DANTE_DOWNCAST(NodeManager)

} // namespace dante::gltfio

#endif // GLTFIO_FNODEMANAGER_H
