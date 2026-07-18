/*
 * Copyright (C) 2024 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMAT_PUSH_CONSTANT_DEFINTITIONS_H
#define TNT_FILAMAT_PUSH_CONSTANT_DEFINTITIONS_H

#include <private/filament/EngineEnums.h>
#include <private/filament/PushConstantInfo.h>

#include <utils/FixedCapacityVector.h>

#include <tuple>

namespace filamat {

constexpr char PUSH_CONSTANT_STRUCT_VAR_NAME[] = "pushConstants";

utils::FixedCapacityVector<filament::MaterialPushConstant> const PUSH_CONSTANTS = {
    {
        "morphingBufferOffset",
        filament::backend::ConstantType::INT,
        filament::backend::ShaderStage::VERTEX,
    },
};

// Make sure that the indices defined in filabridge match the actual array indices defined here.
static_assert(static_cast<uint8_t>(filament::PushConstantIds::MORPHING_BUFFER_OFFSET) == 0u);

}// namespace filamat

#endif
