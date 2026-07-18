/*
 * Copyright (C) 2025 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_BACKEND_OPENGL_OPENGLDRIVERBASE_H
#define TNT_FILAMENT_BACKEND_OPENGL_OPENGLDRIVERBASE_H

#include "DriverBase.h"

#include <backend/Platform.h>

#include <utils/CString.h>

namespace filament::backend {

class OpenGLDriverBase : public DriverBase {
protected:
    explicit OpenGLDriverBase(const Platform::DriverConfig& driverConfig)
        : DriverBase(driverConfig) {}
    ~OpenGLDriverBase() override;

public:
    virtual utils::CString getVendorString() const noexcept = 0;
    virtual utils::CString getRendererString() const noexcept = 0;
    virtual utils::CString getVersionString() const noexcept = 0;
};

} // filament::backend

#endif //TNT_FILAMENT_BACKEND_OPENGL_OPENGLDRIVERBASE_H
