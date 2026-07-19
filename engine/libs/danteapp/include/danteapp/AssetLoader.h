
#ifndef TNT_DANTEAPP_ASSETLOADER_H
#define TNT_DANTEAPP_ASSETLOADER_H

#include <utils/Path.h>

#include <cstdint>
#include <vector>

namespace dante::app {

class AssetLoader {
public:
    virtual ~AssetLoader() = default;

    virtual std::vector<uint8_t> load(utils::Path const& path) const = 0;
};

} // namespace dante::app

#endif // TNT_DANTEAPP_ASSETLOADER_H
