
#ifndef TNT_DANTE_FG_FRAMEGRAPHRENDERPASS_H
#define TNT_DANTE_FG_FRAMEGRAPHRENDERPASS_H

#include "fg/FrameGraphTexture.h"

#include <dante/Viewport.h>

#include <backend/DriverEnums.h>
#include <backend/TargetBufferInfo.h>

#include <utils/debug.h>

namespace dante {

/**
 * FrameGraphRenderPass is used to draw into a set of FrameGraphTexture resources.
 * These are transient objects that exist inside a pass only.
 */
struct FrameGraphRenderPass {
    static constexpr size_t ATTACHMENT_COUNT = backend::MRT::MAX_SUPPORTED_RENDER_TARGET_COUNT + 2;
    struct Attachments {
        FrameGraphId<FrameGraphTexture> color[backend::MRT::MAX_SUPPORTED_RENDER_TARGET_COUNT];
        FrameGraphId<FrameGraphTexture> depth;
        FrameGraphId<FrameGraphTexture> stencil;

        FrameGraphId<FrameGraphTexture>& operator[](size_t index) noexcept {
            return const_cast<FrameGraphId<FrameGraphTexture>&>(
                    static_cast<const Attachments*>(this)->operator[](index));
        }

        FrameGraphId<FrameGraphTexture> const& operator[](size_t index) const noexcept {
            assert_invariant(index < ATTACHMENT_COUNT);
            if (index < backend::MRT::MAX_SUPPORTED_RENDER_TARGET_COUNT) {
                return color[index];
            } else if (index == backend::MRT::MAX_SUPPORTED_RENDER_TARGET_COUNT) {
                return depth;
            } else {
                return stencil;
            }
        }
    };

    struct Descriptor {
        Attachments attachments{};
        Viewport viewport{};
        backend::ClearColorValue clearColor{};
        uint8_t samples = 0;    // # of samples (0 = unset, default)
        uint8_t layerCount = 1; // # of layer (# > 1 = multiview)
        backend::TargetBufferFlags clearFlags{};
    };

    struct ImportDescriptor {
        backend::TargetBufferFlags attachments = backend::TargetBufferFlags::COLOR0;
        Viewport viewport{};
        backend::ClearColorValue clearColor{};   // this overrides Descriptor::clearColor
        uint8_t samples = 0;                     // # of samples (0 = unset, default)
        backend::TargetBufferFlags clearFlags{}; // this overrides Descriptor::clearFlags
        backend::TargetBufferFlags keepOverrideStart{};
        backend::TargetBufferFlags keepOverrideEnd{};
    };

    uint32_t id = 0;
};

} // namespace dante

#endif // TNT_DANTE_FG_FRAMEGRAPHRENDERPASS_H
