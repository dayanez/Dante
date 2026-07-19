
#ifndef GLTFIO_RESOURCELOADEREXTENDED_H
#define GLTFIO_RESOURCELOADEREXTENDED_H

#include "../FDanteAsset.h"

#include <vector>

namespace dante::gltfio {

struct ResourceLoaderExtended {
    using BufferSlot = FDanteAsset::ResourceInfoExtended::BufferSlot;
    static void loadResources(
        std::vector<BufferSlot> const& slots, dante::Engine* engine,
        std::vector<BufferObject*>& bufferObjects);
};

} // namespace dante::gltfio

#endif // GLTFIO_RESOURCELOADEREXTENDED_H
