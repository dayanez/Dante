

#include "details/Sync.h"

#include "details/Engine.h"

#include <dante/Sync.h>

#include <backend/Platform.h>

namespace dante {

using DriverApi = backend::DriverApi;

FSync::FSync(FEngine& engine)
    : mEngine(engine) {
    DriverApi& driverApi = engine.getDriverApi();
    mHwSync = driverApi.createSync();
}

void FSync::terminate(FEngine& engine) noexcept {
    engine.getDriverApi().destroySync(mHwSync);
}

void FSync::getExternalHandle(Sync::CallbackHandler* handler, Sync::Callback callback,
        void* userData) noexcept {
    mEngine.getDriverApi().getPlatformSync(mHwSync, handler, callback, userData);
}

} // namespace dante
