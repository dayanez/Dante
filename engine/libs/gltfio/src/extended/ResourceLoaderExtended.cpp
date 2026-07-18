/*
 * Copyright (C) 2024 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ResourceLoaderExtended.h"

#include "../FFilamentAsset.h"

#include <filament/BufferObject.h>

#include <backend/BufferDescriptor.h>

#include <cgltf.h>

#include <vector>

static const auto FREE_CALLBACK = [](void* mem, size_t, void*) {
    free(mem);
 };

namespace filament::gltfio {


void ResourceLoaderExtended::loadResources(std::vector<BufferSlot> const& slots,
        filament::Engine* engine, std::vector<BufferObject*>& bufferObjects) {
    for (auto& slot: slots) {
        size_t const byteCount = slot.sizeInBytes;
        if (slot.vertices) {
            BufferObject* bo = BufferObject::Builder().size(byteCount).build(*engine);
            bo->setBuffer(*engine, BufferDescriptor(slot.data, byteCount, FREE_CALLBACK));
            slot.vertices->setBufferObjectAt(*engine, slot.slot, bo);
            bufferObjects.push_back(bo);
        }
        if (slot.indices) {
            slot.indices->setBuffer(*engine, BufferDescriptor(slot.data, byteCount, FREE_CALLBACK));
        }
        if (slot.target) {
            assert_invariant(slot.targetData.positions && slot.targetData.tbn);
            slot.target->setPositionsAt(*engine, slot.slot,
                    (float3 const*) slot.targetData.positions,
                    slot.count, slot.offset);
            slot.target->setTangentsAt(*engine, slot.slot, (short4 const*) slot.targetData.tbn,
                    slot.count, slot.offset);

            free(slot.targetData.positions);
            free(slot.targetData.tbn);
        }
    }
}

} // namespace filament::gltfio
