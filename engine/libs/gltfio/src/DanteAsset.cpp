
#include "FDanteAsset.h"
#include "GltfEnums.h"
#include "Wireframe.h"

#include <gltfio/Animator.h>

#include <dante/RenderableManager.h>
#include <dante/Scene.h>

#include <utils/EntityManager.h>
#include <utils/Log.h>
#include <utils/NameComponentManager.h>

using namespace dante;
using namespace utils;

namespace dante::gltfio {

FDanteAsset::~FDanteAsset() {
    // Free transient load-time data if they haven't been freed yet.
    releaseSourceData();

    for (FDanteInstance* instance : mInstances) {
        mEntityManager->destroy(instance->mRoot);
    }

    delete mWireframe;

    // Destroy name components.
    if (mNameManager) {
        for (auto entity : mEntities) {
            mNameManager->removeComponent(entity);
        }
    }

    // Destroy gltfio node components.
    for (auto entity : mEntities) {
        mNodeManager->destroy(entity);

    }

    // Destroy gltfio trs transform components.
    for (auto entity : mEntities) {
        mTrsTransformManager->destroy(entity);
    }

    // Destroy all renderable, light, transform, and camera components,
    // then destroy the actual entities. This includes instances.
    if (!mDetachedDanteComponents) {
        mEngine->destroy(mRoot);
        mEntityManager->destroy(mRoot);
        for (auto entity : mEntities) {
            mEngine->destroy(entity);
            mEntityManager->destroy(entity);
        }
    }

    // DanteInstances need to be destroyed after the renderables have been destroyed
    // so that there are no dangling MaterialInstance around
    for (FDanteInstance* instance : mInstances) {
        delete instance;
    }

    for (auto vb : mVertexBuffers) {
        mEngine->destroy(vb);
    }
    for (auto bo : mBufferObjects) {
        mEngine->destroy(bo);
    }
    for (auto ib : mIndexBuffers) {
        mEngine->destroy(ib);
    }
    for (auto tx : mTextures) {
        if (UTILS_LIKELY(tx.isOwner)) {
            mEngine->destroy(tx.texture);
        }
    }
    for (auto tb : mMorphTargetBuffers) {
        mEngine->destroy(tb);
    }
}

const char* FDanteAsset::getExtras(utils::Entity entity) const noexcept {
    if (entity.isNull()) {
        return mAssetExtras.c_str();
    }
    return mNodeManager->getExtras(mNodeManager->getInstance(entity)).c_str();
}

void FDanteAsset::addTextureBinding(MaterialInstance* materialInstance,
        const char* parameterName, const cgltf_texture* srcTexture,
        TextureProvider::TextureFlags flags) {
    if (!srcTexture->image && !srcTexture->basisu_image && !srcTexture->webp_image) {
#ifndef NDEBUG
        slog.w << "Texture is missing image (" << srcTexture->name << ")." << io::endl;
#endif
        return;
    }

    const size_t gltfTextureIndex = (size_t) (srcTexture - mSourceAsset->hierarchy->textures);
    const size_t assetTextureIndex = obtainAssetTextureIndex(gltfTextureIndex, flags);
    TextureInfo& info = mTextures[assetTextureIndex];

    const TextureSlot slot = { materialInstance, parameterName };
    if (info.texture) {
        applyTextureBinding(assetTextureIndex, slot, false);
    } else {
        mDependencyGraph.addEdge(materialInstance, parameterName);
        info.bindings.push_back(slot);
    }
}

void FDanteAsset::applyTextureBinding(size_t assetTextureIndex, const TextureSlot& tb,
        bool addDependency) {
    const TextureInfo& info = mTextures[assetTextureIndex];
    assert_invariant(info.texture);
    const cgltf_sampler* srcSampler =
            mSourceAsset->hierarchy->textures[info.gltfTextureIndex].sampler;
    TextureSampler sampler;
    if (srcSampler) {
        sampler.setWrapModeS(getWrapMode(srcSampler->wrap_s));
        sampler.setWrapModeT(getWrapMode(srcSampler->wrap_t));
        sampler.setMagFilter(getMagFilter(srcSampler->mag_filter));
        sampler.setMinFilter(getMinFilter(srcSampler->min_filter));
    } else {
        // These defaults are stipulated by the spec:
        sampler.setWrapModeS(TextureSampler::WrapMode::REPEAT);
        sampler.setWrapModeT(TextureSampler::WrapMode::REPEAT);

        // These defaults are up to the implementation but since we try to provide mipmaps,
        // we might as well use them. In practice the conformance models look awful without
        // using mipmapping by default.
        sampler.setMagFilter(TextureSampler::MagFilter::LINEAR);
        sampler.setMinFilter(TextureSampler::MinFilter::LINEAR_MIPMAP_LINEAR);
    }
    tb.materialInstance->setParameter(tb.materialParameter, info.texture, sampler);
    if (addDependency) {
        mDependencyGraph.addEdge(info.texture, tb.materialInstance, tb.materialParameter);
    }
}

const char* FDanteAsset::getMorphTargetNameAt(utils::Entity entity,
        size_t targetIndex) const noexcept {
    if (!mResourcesLoaded) {
        return nullptr;
    }

    const auto& names =  mNodeManager->getMorphTargetNames(mNodeManager->getInstance(entity));
    if (targetIndex >= names.size()) {
        return nullptr;
    }

    return names[targetIndex].c_str();
}

size_t FDanteAsset::getMorphTargetCountAt(utils::Entity entity) const noexcept {
    if (!mResourcesLoaded) {
        return 0;
    }

    const auto& names = mNodeManager->getMorphTargetNames(mNodeManager->getInstance(entity));
    return names.size();
}

Entity FDanteAsset::getWireframe() noexcept {
    if (!mWireframe) {
        mWireframe = new Wireframe(this);
    }
    return mWireframe->mEntity;
}

void FDanteAsset::releaseSourceData() noexcept {
    // To ensure that all possible memory is freed, we reassign to new containers rather than
    // calling clear(). With many container types, clearing is a fast operation that merely frees
    // the storage for the items but not the actual container.
    for (auto& info : mTextures) {
        info.bindings = {};
    }
    mMeshCache = {};
    mResourceUris = {};
    mSourceAsset.reset();
}

const char* FDanteAsset::getName(utils::Entity entity) const noexcept {
    if (mNameManager == nullptr) {
        return nullptr;
    }
    auto nameInstance = mNameManager->getInstance(entity);
    return nameInstance ? mNameManager->getName(nameInstance) : nullptr;
}

Entity FDanteAsset::getFirstEntityByName(const char* name) noexcept {
    const auto iter = mNameToEntity.find(name);
    if (iter == mNameToEntity.end()) {
        return {};
    }
    return iter->second.front();
}

size_t FDanteAsset::getEntitiesByName(const char* name, Entity* entities,
        size_t maxCount) const noexcept {
    const auto iter = mNameToEntity.find(name);
    if (iter == mNameToEntity.end()) {
        return 0;
    }
    const auto& source = iter->second;
    if (entities == nullptr) {
        return source.size();
    }
    maxCount = std::min(maxCount, source.size());
    if (maxCount == 0) {
        return 0;
    }
    size_t count = 0;
    for (Entity entity : source) {
        entities[count] = entity;
        if (++count >= maxCount) {
            return count;
        }
    }
    return count;
}

size_t FDanteAsset::getEntitiesByPrefix(const char* prefix, Entity* entities,
        size_t maxCount) const noexcept {
    if (maxCount == 0) {
        return 0;
    }
    std::string_view prefixString(prefix);
    size_t count = 0;
    for (auto& [k, v] : mNameToEntity) {
        if (k.compare(0, prefixString.size(), prefixString) == 0) {
            for (Entity entity : v) {
                if (entities) {
                    entities[count] = entity;
                }
                if (++count >= maxCount) {
                    return count;
                }
            }
        }
    }
    return count;
}

void FDanteAsset::addEntitiesToScene(Scene& targetScene, const Entity* entities,
        size_t count, SceneMask sceneFilter) const {
    auto& nm = *mNodeManager;
    for (size_t ei = 0; ei < count; ++ei) {
        const Entity entity = entities[ei];
        NodeManager::SceneMask mask = nm.getSceneMembership(nm.getInstance(entity));
        if ((mask & sceneFilter).any()) {
            targetScene.addEntity(entity);
        }
    }
}

void DanteAsset::detachDanteComponents() noexcept {
    downcast(this)->detachDanteComponents();
}

bool DanteAsset::areDanteComponentsDetached() const noexcept {
    return downcast(this)->mDetachedDanteComponents;
}

size_t DanteAsset::getEntityCount() const noexcept {
    return downcast(this)->getEntityCount();
}

const Entity* DanteAsset::getEntities() const noexcept {
    return downcast(this)->getEntities();
}

const Entity* DanteAsset::getLightEntities() const noexcept {
    return downcast(this)->getLightEntities();
}

size_t DanteAsset::getLightEntityCount() const noexcept {
    return downcast(this)->getLightEntityCount();
}

const Entity* DanteAsset::getRenderableEntities() const noexcept {
    return downcast(this)->getRenderableEntities();
}

size_t DanteAsset::getRenderableEntityCount() const noexcept {
    return downcast(this)->getRenderableEntityCount();
}

const utils::Entity* DanteAsset::getCameraEntities() const noexcept {
    return downcast(this)->getCameraEntities();
}

size_t DanteAsset::getCameraEntityCount() const noexcept {
    return downcast(this)->getCameraEntityCount();
}

Entity DanteAsset::getRoot() const noexcept {
    return downcast(this)->getRoot();
}

Entity DanteAsset::popRenderable() noexcept {
    Entity result[1];
    const bool empty = !popRenderables(result, 1);
    return empty ? Entity() : result[0];
}

size_t DanteAsset::popRenderables(Entity* result, size_t count) noexcept {
    return downcast(this)->popRenderables(result, count);
}

size_t DanteAsset::getResourceUriCount() const noexcept {
    return downcast(this)->getResourceUriCount();
}

const char* const* DanteAsset::getResourceUris() const noexcept {
    return downcast(this)->getResourceUris();
}

dante::Aabb DanteAsset::getBoundingBox() const noexcept {
    return downcast(this)->getBoundingBox();
}

const char* DanteAsset::getName(Entity entity) const noexcept {
    return downcast(this)->getName(entity);
}

Entity DanteAsset::getFirstEntityByName(const char* name) noexcept {
    return downcast(this)->getFirstEntityByName(name);
}

size_t DanteAsset::getEntitiesByName(const char* name, Entity* entities,
        size_t maxCount) const noexcept {
    return downcast(this)->getEntitiesByName(name, entities, maxCount);
}

size_t DanteAsset::getEntitiesByPrefix(const char* prefix, Entity* entities,
        size_t maxCount) const noexcept {
    return downcast(this)->getEntitiesByPrefix(prefix, entities, maxCount);
}

const char* DanteAsset::getExtras(Entity entity) const noexcept {
    return downcast(this)->getExtras(entity);
}

const char* DanteAsset::getMorphTargetNameAt(utils::Entity entity,
        size_t targetIndex) const noexcept {
    return downcast(this)->getMorphTargetNameAt(entity, targetIndex);
}

size_t DanteAsset::getMorphTargetCountAt(utils::Entity entity) const noexcept {
    return downcast(this)->getMorphTargetCountAt(entity);
}

Entity DanteAsset::getWireframe() noexcept {
    return downcast(this)->getWireframe();
}

Engine* DanteAsset::getEngine() const noexcept {
    return downcast(this)->getEngine();
}

void DanteAsset::releaseSourceData() noexcept {
    return downcast(this)->releaseSourceData();
}

const void* DanteAsset::getSourceAsset() noexcept {
    return downcast(this)->getSourceAsset();
}

DanteInstance** DanteAsset::getAssetInstances() noexcept {
    return downcast(this)->getAssetInstances();
}

size_t DanteAsset::getAssetInstanceCount() const noexcept {
    return downcast(this)->getAssetInstanceCount();
}

size_t DanteAsset::getSceneCount() const noexcept {
    return downcast(this)->getSceneCount();
}

const char* DanteAsset::getSceneName(size_t sceneIndex) const noexcept {
    return downcast(this)->getSceneName(sceneIndex);
}

void DanteAsset::addEntitiesToScene(Scene& targetScene, const Entity* entities, size_t count,
        SceneMask sceneFilter) const {
    downcast(this)->addEntitiesToScene(targetScene, entities, count, sceneFilter);
}

} // namespace dante::gltfio
