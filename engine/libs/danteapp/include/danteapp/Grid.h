
#ifndef TNT_DANTE_SAMPLE_GRID_H
#define TNT_DANTE_SAMPLE_GRID_H

#include <dante/Box.h>
#include <dante/Camera.h>
#include <dante/Engine.h>
#include <dante/Material.h>
#include <dante/MaterialInstance.h>

#include <utils/Entity.h>

#include <math/mat4.h>
#include <math/vec3.h>

#include <functional>

class Grid {
public:

    Grid(dante::Engine& engine, dante::Material const* material,
        dante::math::float3 linearColor);

    Grid(Grid const&) = delete;
    Grid& operator=(Grid const&) = delete;

    Grid(Grid&& rhs) noexcept;

    utils::Entity getWireFrameRenderable() const {
        return mWireFrameRenderable;
    }

    ~Grid();

    using Generator = std::function<float(int index)>;

    void update(uint32_t width, uint32_t height, uint32_t depth);

    void update(uint32_t width, uint32_t height, uint32_t depth,
            Generator const& genWidth, Generator const& genHeight, Generator const& genDepth);

    void mapFrustum(dante::Engine& engine, dante::Camera const* camera);
    void mapFrustum(dante::Engine& engine, dante::math::mat4 const& transform);
    void mapAabb(dante::Engine& engine, dante::Box const& box);

private:
    dante::Engine& mEngine;
    dante::VertexBuffer* mVertexBuffer = nullptr;
    dante::IndexBuffer* mIndexBuffer = nullptr;
    dante::Material const* mMaterial = nullptr;
    dante::MaterialInstance* mMaterialInstanceWireFrame = nullptr;
    utils::Entity mWireFrameRenderable{};
};


#endif // TNT_DANTE_SAMPLE_GRID_H
