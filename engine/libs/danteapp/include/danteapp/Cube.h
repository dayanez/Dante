
#ifndef TNT_DANTE_SAMPLE_CUBE_H
#define TNT_DANTE_SAMPLE_CUBE_H

#include <dante/Box.h>
#include <dante/Camera.h>
#include <dante/Engine.h>
#include <dante/Material.h>
#include <dante/MaterialInstance.h>

#include <utils/Entity.h>

#include <vector>

class Cube {
public:

    Cube(dante::Engine& engine, dante::Material const* material, dante::math::float3 linearColor, bool culling = true);

    Cube(Cube const&) = delete;
    Cube& operator=(Cube const&) = delete;

    Cube(Cube&& rhs) noexcept;

    utils::Entity getSolidRenderable() {
        return mSolidRenderable;
    }

    utils::Entity getWireFrameRenderable() {
        return mWireFrameRenderable;
    }

    ~Cube();

    void mapFrustum(dante::Engine& engine, dante::Camera const* camera);
    void mapFrustum(dante::Engine& engine, dante::math::mat4 const& transform);
    void mapAabb(dante::Engine& engine, dante::Box const& box);

private:
    static constexpr size_t WIREFRAME_OFFSET = 3*2*6;
    static const uint32_t mIndices[];
    static const dante::math::float3 mVertices[];

    dante::Engine& mEngine;
    dante::VertexBuffer* mVertexBuffer = nullptr;
    dante::IndexBuffer* mIndexBuffer = nullptr;
    dante::Material const* mMaterial = nullptr;
    dante::MaterialInstance* mMaterialInstanceSolid = nullptr;
    dante::MaterialInstance* mMaterialInstanceWireFrame = nullptr;
    utils::Entity mSolidRenderable{};
    utils::Entity mWireFrameRenderable{};
};


#endif // TNT_DANTE_SAMPLE_CUBE_H
