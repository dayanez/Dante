/*
 * Copyright (C) 2023 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_CONSTANTINFO_H
#define TNT_FILAMENT_CONSTANTINFO_H

#include <backend/DriverEnums.h>

#include <utils/CString.h>

namespace filament {

struct MaterialConstant {
    using ConstantType = backend::ConstantType;
    using ConstantValue = backend::ConstantValue;

    utils::CString name;
    ConstantType type;
    ConstantValue defaultValue;

    MaterialConstant() = default;
    MaterialConstant(utils::CString name, ConstantType type, ConstantValue defaultValue)
            : name(std::move(name)), type(type), defaultValue(defaultValue) {}
};

}

#endif  // TNT_FILAMENT_CONSTANTINFO_H
