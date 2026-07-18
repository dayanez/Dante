/*
 * Copyright (C) 2023 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_PUSHCONSTANTINFO_H
#define TNT_FILAMENT_PUSHCONSTANTINFO_H

#include <backend/DriverEnums.h>

#include <utils/CString.h>

namespace filament {

struct MaterialPushConstant {
    using ShaderStage = backend::ShaderStage;
    using ConstantType = backend::ConstantType;

    utils::CString name;
    ConstantType type;
    ShaderStage stage;

    MaterialPushConstant() = default;
    MaterialPushConstant(const char* name, ConstantType type, ShaderStage stage)
        : name(name),
          type(type),
          stage(stage) {}
};

}

#endif  // TNT_FILAMENT_PUSHCONSTANTINFO_H
