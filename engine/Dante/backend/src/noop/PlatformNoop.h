
#ifndef TNT_DANTE_BACKEND_NOOP_PLATFORM_NOOP_H
#define TNT_DANTE_BACKEND_NOOP_PLATFORM_NOOP_H

#include <backend/DriverEnums.h>
#include <backend/Platform.h>

namespace dante::backend {

class PlatformNoop final : public Platform {
public:

    int getOSVersion() const noexcept final { return 0; }
    utils::CString getDeviceInfo(DeviceInfoType, Driver*) const override { return {}; }

    ~PlatformNoop() noexcept override = default;

protected:

    Driver* createDriver(void* sharedContext, const Platform::DriverConfig& driverConfig) override;
};

} // namespace dante

#endif // TNT_DANTE_BACKEND_NOOP_PLATFORM_NOOP_H
