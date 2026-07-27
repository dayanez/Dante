
#ifndef TNT_DANTE_SAMPLE_IBL_H
#define TNT_DANTE_SAMPLE_IBL_H

#include <dante/Texture.h>

#include <math/vec3.h>

#include <sstream>
#include <string>

namespace dante {
class Engine;
class IndexBuffer;
class IndirectLight;
class Material;
class MaterialInstance;
class Renderable;
class Texture;
class Skybox;
}

namespace utils {
    class Path;
}

class IBL {
public:
    explicit IBL(dante::Engine& engine);
    ~IBL();

    bool loadFromEquirect(const utils::Path& path);
    bool loadFromDirectory(const utils::Path& path);

    dante::IndirectLight* getIndirectLight() const noexcept {
        return mIndirectLight;
    }

    dante::Skybox* getSkybox() const noexcept {
        return mSkybox;
    }

    dante::Texture* getFogTexture() const noexcept {
        return mFogTexture;
    }

    bool hasSphericalHarmonics() const { return mHasSphericalHarmonics; }
    dante::math::float3 const* getSphericalHarmonics() const { return mBands; }

private:
    bool loadCubemapLevel(dante::Texture** texture, const utils::Path& path,
            size_t level = 0, std::string const& levelPrefix = "") const;


    bool loadCubemapLevel(dante::Texture** texture,
            dante::Texture::PixelBufferDescriptor* outBuffer,
            uint32_t* dim,
            const utils::Path& path,
            size_t level = 0, std::string const& levelPrefix = "") const;

    dante::Engine& mEngine;

    dante::math::float3 mBands[9] = {};
    bool mHasSphericalHarmonics = false;

    dante::Texture* mTexture = nullptr;
    dante::IndirectLight* mIndirectLight = nullptr;
    dante::Texture* mSkyboxTexture = nullptr;
    dante::Texture* mFogTexture = nullptr;
    dante::Skybox* mSkybox = nullptr;
};

#endif // TNT_DANTE_SAMPLE_IBL_H
