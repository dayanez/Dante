
#ifndef GLTFIO_WIREFRAME_H
#define GLTFIO_WIREFRAME_H

#include <dante/IndexBuffer.h>
#include <dante/VertexBuffer.h>

#include <utils/Entity.h>

namespace dante::gltfio {

struct FDanteAsset;

struct Wireframe {
    Wireframe(FDanteAsset* asset);
    ~Wireframe();
    const FDanteAsset* mAsset;
    utils::Entity mEntity;
    dante::VertexBuffer* mVertexBuffer;
    dante::IndexBuffer* mIndexBuffer;
};

} // namsepace gltfio

#endif // GLTFIO_WIREFRAME_H
