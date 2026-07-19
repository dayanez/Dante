
#ifndef GLTFIO_FDANTEINSTANCE_H
#define GLTFIO_FDANTEINSTANCE_H

#include "downcast.h"

#include <gltfio/DanteInstance.h>

#include <utils/CString.h>
#include <utils/Entity.h>
#include <utils/FixedCapacityVector.h>

#include <math/mat4.h>

#include <tsl/robin_set.h>

#include <vector>

struct cgltf_node;

namespace dante {
    class MaterialInstance;
}

namespace dante::gltfio {

struct FDanteAsset;
class Animator;

struct VariantMapping {
    utils::Entity renderable;
    size_t primitiveIndex;
    MaterialInstance* material;
};

struct Variant {
    utils::CString name;
    std::vector<VariantMapping> mappings;
};

struct FDanteInstance : public DanteInstance {
    FDanteInstance(utils::Entity root, FDanteAsset const* owner);
    ~FDanteInstance();

    size_t getMaterialInstanceCount() const noexcept {
        return mMaterialInstances.size();
    }

    const MaterialInstance* const* getMaterialInstances() const noexcept {
        return mMaterialInstances.data();
    }

    MaterialInstance* const* getMaterialInstances() noexcept {
        return mMaterialInstances.data();
    }

    void detachMaterialInstances() {
        mMaterialInstances.clear();
    }

    void applyMaterialVariant(size_t variantIndex) noexcept;

    size_t getMaterialVariantCount() const noexcept;

    const char* getMaterialVariantName(size_t variantIndex) const noexcept;

    // The per-instance skin structure caches information to allow animation to be applied
    // efficiently at run time. Note that shared immutable data, such as the skin name and inverse
    // bind transforms, are stored in FDanteAsset.
    struct Skin {
        // The list of entities whose transform components define the joints of the skin.
        utils::FixedCapacityVector<utils::Entity> joints;

        // The set of all entities that are influenced by this skin.
        // This is initially derived from the glTF, but users can dynamically add or remove targets.
        tsl::robin_set<utils::Entity, utils::Entity::Hasher> targets;
    };

    const utils::Entity mRoot;
    FDanteAsset const* mOwner;

    std::vector<utils::Entity> mEntities;
    utils::FixedCapacityVector<Variant> mVariants;
    Animator* mAnimator = nullptr;
    utils::FixedCapacityVector<Skin> mSkins;

    // Note that nodeMap is yet another a vector of entities, but unlike the "entities" field, it
    // may be sparsely populated. This is used as a simple mapping between cgltf_node and Entity,
    // and therefore has the same size as the number of cgltf_node in the original asset. We
    // considered using the ECS for this, but we need Node => Entity, not the other way around.
    utils::FixedCapacityVector<utils::Entity> mNodeMap;

    Aabb mBoundingBox;

    utils::FixedCapacityVector<MaterialInstance*> mMaterialInstances;

    void createAnimator();
    Animator* getAnimator() const noexcept;
    size_t getSkinCount() const noexcept;
    const char* getSkinNameAt(size_t skinIndex) const noexcept;
    size_t getJointCountAt(size_t skinIndex) const noexcept;
    const utils::Entity* getJointsAt(size_t skinIndex) const noexcept;
    void attachSkin(size_t skinIndex, utils::Entity target) noexcept;
    void detachSkin(size_t skinIndex, utils::Entity target) noexcept;
    math::mat4f const* getInverseBindMatricesAt(size_t skinIndex) const;

    void recomputeBoundingBoxes();
};

DANTE_DOWNCAST(DanteInstance)

} // namespace dante::gltfio

#endif // GLTFIO_FDANTEINSTANCE_H
