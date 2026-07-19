
#ifndef TNT_DANTE_SAMPLE_SPHERE_H
#define TNT_DANTE_SAMPLE_SPHERE_H

#include <utils/Entity.h>

#include <math/vec3.h>

namespace dante {
class Engine;
class IndexBuffer;
class Material;
class MaterialInstance;
class VertexBuffer;
}

class Sphere {
public:
    Sphere( dante::Engine& engine,
            dante::Material const* material,
            bool culling = true);
    ~Sphere();

    Sphere(Sphere const&) = delete;
    Sphere& operator = (Sphere const&) = delete;

    Sphere(Sphere&& rhs) noexcept
            : mEngine(rhs.mEngine),
              mMaterialInstance(rhs.mMaterialInstance),
              mRenderable(rhs.mRenderable) {
        rhs.mMaterialInstance = {};
        rhs.mRenderable = {};
    }

    utils::Entity getSolidRenderable() const {
        return mRenderable;
    }

    dante::MaterialInstance* getMaterialInstance() {
        return mMaterialInstance;
    }

    Sphere& setPosition(dante::math::float3 const& position) noexcept;
    Sphere& setRadius(float radius) noexcept;

private:
    dante::Engine& mEngine;
    dante::MaterialInstance* mMaterialInstance = nullptr;
    utils::Entity mRenderable;

};

#endif //TNT_DANTE_SAMPLE_SPHERE_H
