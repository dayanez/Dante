/*
 * Copyright (C) 2015 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_BACKEND_NOOPDRIVER_H
#define TNT_FILAMENT_BACKEND_NOOPDRIVER_H

#include "private/backend/Driver.h"
#include "DriverBase.h"

#include <utils/FixedCapacityVector.h>
#include <utils/compiler.h>

namespace filament::backend {

class NoopDriver final : public DriverBase {
    NoopDriver(const Platform::DriverConfig& driverConfig) noexcept;
    ~NoopDriver() noexcept override;
    Dispatcher getDispatcher() const noexcept final;

public:
    static Driver* create();

private:
    ShaderModel getShaderModel() const noexcept final;
    utils::FixedCapacityVector<ShaderLanguage> getShaderLanguages(
            ShaderLanguage preferredLanguage) const noexcept final;

    uint64_t nextFakeHandle = 1;

    /*
     * Driver interface
     */

    template<typename T>
    friend class ConcreteDispatcher;

#define DECL_DRIVER_API(methodName, paramsDecl, params) \
    UTILS_ALWAYS_INLINE inline void methodName(paramsDecl);

#define DECL_DRIVER_API_SYNCHRONOUS(RetType, methodName, paramsDecl, params) \
    RetType methodName(paramsDecl) override;

#define DECL_DRIVER_API_RETURN(RetType, methodName, paramsDecl, params) \
    RetType methodName##S() noexcept override { \
        return RetType(nextFakeHandle++); } \
    UTILS_ALWAYS_INLINE inline void methodName##R(RetType, paramsDecl) { }

#include "private/backend/DriverAPI.inc"
};

} // namespace filament

#endif // TNT_FILAMENT_BACKEND_NOOPDRIVER_H
