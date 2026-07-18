/*
 * Copyright (C) 2020 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "DependencyGraph.h"

#include <utils/debug.h>
#include <utils/Panic.h>

using namespace filament;
using namespace utils;

namespace filament::gltfio {

size_t DependencyGraph::popRenderables(Entity* result, size_t count) noexcept {
    if (result == nullptr) {
        return mReadyRenderables.size();
    }
    size_t numWritten = 0;
    while (numWritten < count && !mReadyRenderables.empty()) {
        result[numWritten++] = mReadyRenderables.front();
        mReadyRenderables.pop();
    }
    return numWritten;
}

void DependencyGraph::addEdge(Entity entity, MaterialInstance* mi) {
    if (mDisabled) {
        if (mEntityToMaterial.count(entity) == 0) {
            mEntityToMaterial[entity] = {};
            mReadyRenderables.push(entity);
        }
    } else {
        mMaterialToEntity[mi].insert(entity);
        mEntityToMaterial[entity].materials.insert(mi);
    }
}

void DependencyGraph::addEdge(MaterialInstance* mi, const char* parameter) {
    if (auto iter = mMaterialToTexture.find(mi); iter != mMaterialToTexture.end()) {
        const tsl::robin_map<std::string, TextureNode*>& params = iter.value().params;
        if (params.find(parameter) != params.end()) {
            return;
        }
    }
    mMaterialToTexture[mi].params[parameter] = nullptr;
}

void DependencyGraph::commitEdges() {
    for (const auto& [material, entities] : mMaterialToEntity) {
        if (mMaterialToTexture.find(material) == mMaterialToTexture.end()) {
            markAsReady(material);
        } else {
            checkReadiness(material);
        }
    }
}

void DependencyGraph::addEdge(Texture* texture, MaterialInstance* material, const char* parameter) {
    assert_invariant(texture);
    if (!mDisabled) {
        mTextureToMaterial[texture].insert(material);
        mMaterialToTexture.at(material).params.at(parameter) = getStatus(texture);
    }
}

void DependencyGraph::checkReadiness(Material* material) {
    auto& status = mMaterialToTexture.at(material);

    // Check this material's texture parameters, there are 5 in the worst case.
    bool materialIsReady = true;
    for (const auto& [name, texture] : status.params) {
        if (!texture || !texture->ready) {
            materialIsReady = false;
            break;
        }
    }

    // If all of its textures are ready, then the material has become ready.
    if (materialIsReady) {
        markAsReady(material);
    }
}

void DependencyGraph::markAsReady(Texture* texture) {
    assert_invariant(texture);
    auto iter = mTextureNodes.find(texture);
    if (iter == mTextureNodes.end()) {
        return;
    }
    iter.value()->ready = true;

    // Iterate over the materials associated with this texture to check if any have become ready.
    // This is O(n2) but the inner loop is always small.
    auto& materials = mTextureToMaterial.at(texture);
    for (auto material : materials) {
        checkReadiness(material);
    }
}

void DependencyGraph::markAsReady(MaterialInstance* material) {
    auto iter = mMaterialToEntity.find(material);
    if (iter == mMaterialToEntity.end()) {
        // It's fine if no entities exist yet.
        return;
    }
    for (auto entity : iter->second) {
        auto& status = mEntityToMaterial.at(entity);
        assert_invariant(status.numReadyMaterials <= status.materials.size());
        if (status.numReadyMaterials == status.materials.size()) {
            continue;
        }
        if (++status.numReadyMaterials == status.materials.size()) {
            mReadyRenderables.push(entity);
        }
    }
}

DependencyGraph::TextureNode* DependencyGraph::getStatus(Texture* texture) {
    assert_invariant(texture);
    auto iter = mTextureNodes.find(texture);
    if (iter == mTextureNodes.end()) {
        TextureNode* status = (mTextureNodes[texture] = std::make_unique<TextureNode>()).get();
        *status = {texture, false};
        return status;
    }
    return iter->second.get();
}

void DependencyGraph::disableProgressiveReveal() {
    mDisabled = true;
    for (auto& [entity, status] : mEntityToMaterial) {
        if (status.numReadyMaterials < status.materials.size()) {
            mReadyRenderables.push(entity);
        }
    }
}

} // namespace filament::gltfio
