/*
 * Copyright (C) 2018 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_BACKEND_NOOP_PLATFORM_NOOP_H
#define TNT_FILAMENT_BACKEND_NOOP_PLATFORM_NOOP_H

#include <backend/DriverEnums.h>
#include <backend/Platform.h>

namespace filament::backend {

class PlatformNoop final : public Platform {
public:

    int getOSVersion() const noexcept final { return 0; }
    utils::CString getDeviceInfo(DeviceInfoType, Driver*) const override { return {}; }

    ~PlatformNoop() noexcept override = default;

protected:

    Driver* createDriver(void* sharedContext, const Platform::DriverConfig& driverConfig) override;
};

} // namespace filament

#endif // TNT_FILAMENT_BACKEND_NOOP_PLATFORM_NOOP_H
