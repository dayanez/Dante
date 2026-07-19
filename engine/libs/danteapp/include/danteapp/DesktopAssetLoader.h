
#ifndef TNT_DANTEAPP_DESKTOPASSETLOADER_H
#define TNT_DANTEAPP_DESKTOPASSETLOADER_H

#include <danteapp/AssetLoader.h>

#include <utils/Path.h>

#include <cstdint>
#include <vector>

namespace dante::app {

class DesktopAssetLoader : public AssetLoader {
public:
    ~DesktopAssetLoader() override = default;

    std::vector<uint8_t> load(utils::Path const& path) const override;
};

} // namespace dante::app

#endif // TNT_DANTEAPP_DESKTOPASSETLOADER_H
