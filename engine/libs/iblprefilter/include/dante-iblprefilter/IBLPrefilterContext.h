
#ifndef TNT_IBL_PREFILTER_IBLPREFILTER_H
#define TNT_IBL_PREFILTER_IBLPREFILTER_H

#include <utils/compiler.h>
#include <utils/Entity.h>

#include <dante/Texture.h>

namespace dante {
class Engine;
class View;
class Scene;
class Renderer;
class Material;
class MaterialInstance;
class VertexBuffer;
class IndexBuffer;
class Camera;
class Texture;
} // namespace dante

/**
 * IBLPrefilterContext creates and initializes GPU state common to all environment map filters
 * supported. Typically, only one instance per dante Engine of this object needs to exist.
 *
 * Usage Example:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * #include <dante/Engine.h>
 * using namespace dante;
 *
 * Engine* engine = Engine::create();
 *
 * IBLPrefilterContext context(engine);
 * IBLPrefilterContext::SpecularFilter filter(context);
 * Texture* texture = filter(environment_cubemap);
 *
 * IndirectLight* indirectLight = IndirectLight::Builder()
 *     .reflections(texture)
 *     .build(engine);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
class UTILS_PUBLIC IBLPrefilterContext {
public:

    enum class Kernel : uint8_t {
        D_GGX,        // Trowbridge-reitz distribution
    };

    /**
     * Creates an IBLPrefilter context.
     * @param engine dante engine to use
     */
    explicit IBLPrefilterContext(dante::Engine& engine);

    /**
     * Destroys all GPU resources created during initialization.
     */
    ~IBLPrefilterContext() noexcept;

    // not copyable
    IBLPrefilterContext(IBLPrefilterContext const&) = delete;
    IBLPrefilterContext& operator=(IBLPrefilterContext const&) = delete;

    // movable
    IBLPrefilterContext(IBLPrefilterContext&& rhs) noexcept;
    IBLPrefilterContext& operator=(IBLPrefilterContext&& rhs) noexcept;

    // -------------------------------------------------------------------------------------------

    /**
     * EquirectangularToCubemap is use to convert an equirectangluar image to a cubemap.
     */
    class EquirectangularToCubemap {
    public:

        struct Config {
            bool mirror = true;  //!< mirror the source horizontally
        };

        /**
         * Creates a EquirectangularToCubemap processor using the default Config
         * @param context IBLPrefilterContext to use
         */
        explicit EquirectangularToCubemap(IBLPrefilterContext& context);

        /**
         * Creates a EquirectangularToCubemap processor using the provided Config
         * @param context IBLPrefilterContext to use
         */
       EquirectangularToCubemap(IBLPrefilterContext& context, Config const& config);

        /**
         * Destroys all GPU resources created during initialization.
         */
        ~EquirectangularToCubemap() noexcept;

        EquirectangularToCubemap(EquirectangularToCubemap const&) = delete;
        EquirectangularToCubemap& operator=(EquirectangularToCubemap const&) = delete;
        EquirectangularToCubemap(EquirectangularToCubemap&& rhs) noexcept;
        EquirectangularToCubemap& operator=(EquirectangularToCubemap&& rhs) noexcept;

        /**
         * Converts an equirectangular image to a cubemap.
         * @param equirectangular   Texture to convert to a cubemap.
         *                          - Can't be null.
         *                          - Must be a 2d texture
         *                          - Must have equirectangular geometry, that is width == 2*height.
         *                          - Must be allocated with all mip levels.
         *                          - Must be SAMPLEABLE
         * @param outCubemap        Output cubemap. If null the texture is automatically created
         *                          with default parameters (size of 256 with 9 levels).
         *                          - Must be a cubemap
         *                          - Must have SAMPLEABLE and COLOR_ATTACHMENT usage bits
         * @return returns outCubemap
         */
        dante::Texture* operator()(
                dante::Texture const* equirectangular,
                dante::Texture* outCubemap = nullptr);

    private:
        IBLPrefilterContext& mContext;
        dante::Material* mEquirectMaterial = nullptr;
        Config mConfig{};
    };

    /**
     * IrradianceFilter is a GPU based implementation of the diffuse probe pre-integration filter.
     * An instance of IrradianceFilter is needed per filter configuration. A filter configuration
     * contains the filter's kernel and sample count.
     */
    class IrradianceFilter {
    public:
        using Kernel = Kernel;

        /**
         * Filter configuration.
         */
        struct Config {
            uint16_t sampleCount = 1024u;   //!< filter sample count (max 2048)
            Kernel kernel = Kernel::D_GGX;  //!< filter kernel
        };

        /**
         * Filtering options for the current environment.
         */
        struct Options {
            float hdrLinear = 1024.0f;   //!< no HDR compression up to this value
            float hdrMax = 16384.0f;     //!< HDR compression between hdrLinear and hdrMax
            float lodOffset = 2.0f;      //!< Good values are 2.0 or 3.0. Higher values help with heavily HDR inputs.
            bool generateMipmap = true;  //!< set to false if the input environment map already has mipmaps
        };

        /**
         * Creates a IrradianceFilter processor.
         * @param context IBLPrefilterContext to use
         * @param config  Configuration of the filter
         */
        IrradianceFilter(IBLPrefilterContext& context, Config config);

        /**
         * Creates a filter with the default configuration.
         * @param context IBLPrefilterContext to use
         */
        explicit IrradianceFilter(IBLPrefilterContext& context);

        /**
         * Destroys all GPU resources created during initialization.
         */
        ~IrradianceFilter() noexcept;

        IrradianceFilter(IrradianceFilter const&) = delete;
        IrradianceFilter& operator=(IrradianceFilter const&) = delete;
        IrradianceFilter(IrradianceFilter&& rhs) noexcept;
        IrradianceFilter& operator=(IrradianceFilter&& rhs) noexcept;

        /**
         * Generates an irradiance cubemap. Mipmaps are not generated even if present.
         * @param options                   Options for this environment
         * @param environmentCubemap        Environment cubemap (input). Can't be null.
         *                                  This cubemap must be SAMPLEABLE and must have all its
         *                                  levels allocated. If Options.generateMipmap is true,
         *                                  the mipmap levels will be overwritten, otherwise
         *                                  it is assumed that all levels are correctly initialized.
         * @param outIrradianceTexture      Output irradiance texture or, if null, it is
         *                                  automatically created with some default parameters.
         *                                  outIrradianceTexture must be a cubemap, it must have
         *                                  at least COLOR_ATTACHMENT and SAMPLEABLE usages.
         *
         * @return returns outIrradianceTexture
         */
        dante::Texture* operator()(Options options,
                dante::Texture const* environmentCubemap,
                dante::Texture* outIrradianceTexture = nullptr);

        /**
         * Generates a prefiltered cubemap.
         * @param environmentCubemap        Environment cubemap (input). Can't be null.
         *                                  This cubemap must be SAMPLEABLE and must have all its
         *                                  levels allocated. If Options.generateMipmap is true,
         *                                  the mipmap levels will be overwritten, otherwise
         *                                  it is assumed that all levels are correctly initialized.
         * @param outIrradianceTexture      Output irradiance texture or, if null, it is
         *                                  automatically created with some default parameters.
         *                                  outIrradianceTexture must be a cubemap, it must have
         *                                  at least COLOR_ATTACHMENT and SAMPLEABLE usages.
         *
         * @return returns outReflectionsTexture
         */
        dante::Texture* operator()(
                dante::Texture const* environmentCubemap,
                dante::Texture* outIrradianceTexture = nullptr);

    private:
        IBLPrefilterContext& mContext;
        dante::Material* mKernelMaterial = nullptr;
        dante::Texture* mKernelTexture = nullptr;
        uint32_t mSampleCount = 0u;
    };

    /**
     * SpecularFilter is a GPU based implementation of the specular probe pre-integration filter.
     * An instance of SpecularFilter is needed per filter configuration. A filter configuration
     * contains the filter's kernel and sample count.
     */
    class SpecularFilter {
    public:
        using Kernel = Kernel;

        /**
         * Filter configuration.
         */
        struct Config {
            uint16_t sampleCount = 1024u;   //!< filter sample count (max 2048)
            uint8_t levelCount = 5u;        //!< number of roughness levels
            Kernel kernel = Kernel::D_GGX;  //!< filter kernel
        };

        /**
         * Filtering options for the current environment.
         */
        struct Options {
            float hdrLinear = 1024.0f;   //!< no HDR compression up to this value
            float hdrMax = 16384.0f;     //!< HDR compression between hdrLinear and hdrMax
            float lodOffset = 1.0f;      //!< Good values are 1.0 or 2.0. Higher values help with heavily HDR inputs.
            bool generateMipmap = true;  //!< set to false if the input environment map already has mipmaps
        };

        /**
         * Creates a SpecularFilter processor.
         * @param context IBLPrefilterContext to use
         * @param config  Configuration of the filter
         */
        SpecularFilter(IBLPrefilterContext& context, Config config);

        /**
         * Creates a filter with the default configuration.
         * @param context IBLPrefilterContext to use
         */
        explicit SpecularFilter(IBLPrefilterContext& context);

        /**
         * Destroys all GPU resources created during initialization.
         */
        ~SpecularFilter() noexcept;

        SpecularFilter(SpecularFilter const&) = delete;
        SpecularFilter& operator=(SpecularFilter const&) = delete;
        SpecularFilter(SpecularFilter&& rhs) noexcept;
        SpecularFilter& operator=(SpecularFilter&& rhs) noexcept;

        /**
         * Generates a prefiltered cubemap.
         * @param options                   Options for this environment
         * @param environmentCubemap        Environment cubemap (input). Can't be null.
         *                                  This cubemap must be SAMPLEABLE and must have all its
         *                                  levels allocated. If Options.generateMipmap is true,
         *                                  the mipmap levels will be overwritten, otherwise
         *                                  it is assumed that all levels are correctly initialized.
         * @param outReflectionsTexture     Output prefiltered texture or, if null, it is
         *                                  automatically created with some default parameters.
         *                                  outReflectionsTexture must be a cubemap, it must have
         *                                  at least COLOR_ATTACHMENT and SAMPLEABLE usages and at
         *                                  least the same number of levels than requested by Config.
         * @return returns outReflectionsTexture
         */
        dante::Texture* operator()(Options options,
                dante::Texture const* environmentCubemap,
                dante::Texture* outReflectionsTexture = nullptr);

        /**
         * Generates a prefiltered cubemap.
         * @param environmentCubemap        Environment cubemap (input). Can't be null.
         *                                  This cubemap must be SAMPLEABLE and must have all its
         *                                  levels allocated. All mipmap levels will be overwritten.
         * @param outReflectionsTexture     Output prefiltered texture or, if null, it is
         *                                  automatically created with some default parameters.
         *                                  outReflectionsTexture must be a cubemap, it must have
         *                                  at least COLOR_ATTACHMENT and SAMPLEABLE usages and at
         *                                  least the same number of levels than requested by Config.
         * @return returns outReflectionsTexture
         */
        dante::Texture* operator()(
                dante::Texture const* environmentCubemap,
                dante::Texture* outReflectionsTexture = nullptr);

        // TODO: option for progressive filtering

        // TODO: add a callback for when the processing is done?

    private:
        IBLPrefilterContext& mContext;
        dante::Material* mKernelMaterial = nullptr;
        dante::Texture* mKernelTexture = nullptr;
        uint32_t mSampleCount = 0u;
        uint8_t mLevelCount = 1u;
    };

private:
    friend class Filter;
    dante::Engine& mEngine;
    dante::Renderer* mRenderer{};
    dante::Scene* mScene{};
    dante::VertexBuffer* mVertexBuffer{};
    dante::IndexBuffer* mIndexBuffer{};
    dante::Camera* mCamera{};
    utils::Entity mFullScreenQuadEntity{};
    utils::Entity mCameraEntity{};
    dante::View* mView{};
    dante::Material* mIntegrationMaterial{};
    dante::Material* mIrradianceIntegrationMaterial{};
};

#endif //TNT_IBL_PREFILTER_IBLPREFILTER_H
