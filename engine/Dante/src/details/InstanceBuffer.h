/*
 * Copyright (C) 2023 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_DETAILS_INSTANCEBUFFER_H
#define TNT_FILAMENT_DETAILS_INSTANCEBUFFER_H

#include "downcast.h"

#include <filament/InstanceBuffer.h>

#include <backend/Handle.h>

#include <utils/CString.h>
#include <utils/FixedCapacityVector.h>

#include <math/mat4.h>

#include <cstddef>
#include <cstdint>

namespace filament {

class RenderableManager;
class FEngine;

struct PerRenderableData;

class FInstanceBuffer : public InstanceBuffer {
public:
    FInstanceBuffer(FEngine& engine, const Builder& builder);
    ~FInstanceBuffer() noexcept;

    void terminate(FEngine& engine);

    size_t getInstanceCount() const noexcept { return mInstanceCount; }

    void setLocalTransforms(math::mat4f const* localTransforms, size_t count, size_t offset);

    math::mat4f const& getLocalTransform(size_t index) const;

    void prepare(
            PerRenderableData* buffer, uint32_t index, uint32_t count,
            math::mat4f const& rootTransform, PerRenderableData const& ubo);

    utils::ImmutableCString const& getName() const noexcept { return mName; }

    uint32_t getIndex() const noexcept { return mIndex; }

private:
    friend class RenderableManager;

    utils::FixedCapacityVector<math::mat4f> mLocalTransforms;
    utils::ImmutableCString mName;
    uint32_t mInstanceCount;
    uint32_t mIndex = 0;
};

FILAMENT_DOWNCAST(InstanceBuffer)

} // namespace filament

#endif //TNT_FILAMENT_DETAILS_INSTANCEBUFFER_H
