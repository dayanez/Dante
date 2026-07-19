
#ifndef TNT_DANTE_DETAILS_RENDERERUTILS_H
#define TNT_DANTE_DETAILS_RENDERERUTILS_H

#include "PostProcessManager.h"
#include "RenderPass.h"

#include "fg/FrameGraphId.h"
#include "fg/FrameGraphTexture.h"

#include <dante/Viewport.h>

#include <backend/DriverEnums.h>
#include <backend/Handle.h>

#include <math/vec2.h>
#include <math/vec4.h>

#include <stdint.h>

namespace dante {

namespace backend {
class PixelBufferDescriptor;
}

class FRenderTarget;
class FrameGraph;
class FrameGraph;
class FView;

class RendererUtils {
public:

    struct ColorPassConfig {
        // Rendering viewport (e.g. scaled down viewport from dynamic resolution)
        Viewport physicalViewport;
        // Logical viewport (e.g. left-bottom non-zero when we have guard bands), origin
        // relative to physicalViewport
        Viewport logicalViewport;
        // dynamic resolution scale
        math::float2 scale;
        // HDR format
        backend::TextureFormat hdrFormat;
        // MSAA sample count
        uint8_t msaa;
        // Clear flags
        backend::TargetBufferFlags clearFlags;
        // Clear color
        backend::ClearColorValue clearColor;
        // Clear stencil
        uint8_t clearStencil = 0u;
        // Contact shadow enabled?
        bool hasContactShadows;
        // Screen space reflections enabled
        bool hasScreenSpaceReflectionsOrRefractions;
        // Use a depth format with a stencil component.
        bool enabledStencilBuffer;
        // Backend feature level
        backend::FeatureLevel featureLevel;
        // Auto depth resolve supported
        bool isAutoDepthResolveSupported;
        // Use post-process fog
        bool fogAsPostProcess;
    };

    struct ColorPassInput {
        FrameGraphId<FrameGraphTexture> linearColor;
        FrameGraphId<FrameGraphTexture> tonemappedColor;
        FrameGraphId<FrameGraphTexture> depth;
        FrameGraphId<FrameGraphTexture> shadows;
        FrameGraphId<FrameGraphTexture> ssao;
        FrameGraphId<FrameGraphTexture> ssr;
        FrameGraphId<FrameGraphTexture> structure;
    };
    struct ColorPassOutput {
        FrameGraphId<FrameGraphTexture> linearColor;
        FrameGraphId<FrameGraphTexture> tonemappedColor;
        FrameGraphId<FrameGraphTexture> depth;
    };

    static ColorPassOutput colorPass(
            FrameGraph& fg, const char* name, FEngine& engine, FView const& view,
            ColorPassInput const& colorPassInput,
            FrameGraphTexture::Descriptor const& colorBufferDesc,
            ColorPassConfig const& config,
            PostProcessManager::ColorGradingConfig colorGradingConfig,
            RenderPass::Executor passExecutor) noexcept;

    static ColorPassOutput refractionPass(
            FrameGraph& fg, FEngine& engine, FView const& view,
            ColorPassInput colorPassInput,
            ColorPassConfig config,
            PostProcessManager::ScreenSpaceRefConfig const& ssrConfig,
            PostProcessManager::ColorGradingConfig colorGradingConfig,
            RenderPass const& pass, RenderPass::Command const* firstRefractionCommand) noexcept;

    static void readPixels(backend::DriverApi& driver,
            backend::Handle<backend::HwRenderTarget> renderTargetHandle,
            uint32_t xoffset, uint32_t yoffset, uint32_t width, uint32_t height,
            backend::PixelBufferDescriptor&& buffer);

    static RenderPass::Command const* getFirstRefractionCommand(RenderPass const& pass) noexcept;
};

} // namespace dante

#endif // TNT_DANTE_DETAILS_RENDERERUTILS_H
