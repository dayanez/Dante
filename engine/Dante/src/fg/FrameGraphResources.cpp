
#include "fg/FrameGraphResources.h"

#include "fg/details/PassNode.h"
#include "fg/details/ResourceNode.h"
#include "fg/FrameGraph.h"
#include "fg/FrameGraphId.h"

#include <utils/debug.h>
#include <utils/Panic.h>

#include <cstdint>

namespace dante {

FrameGraphResources::FrameGraphResources(FrameGraph& fg, PassNode& passNode) noexcept
    : mFrameGraph(fg), mPassNode(passNode) {
}

const char* FrameGraphResources::getPassName() const noexcept {
    return mPassNode.getName();
}

// this perhaps weirdly returns a reference, this is to express the fact that if this method
// fails, it has to assert (or throw), it can't return for e.g. a nullptr, because the public
// API doesn't return pointers.
// We use DANTE_CHECK_PRECONDITION() because these failures are due to caller contract violations (preconditions).
VirtualResource& FrameGraphResources::getResource(FrameGraphHandle const handle) const {
    DANTE_CHECK_PRECONDITION(handle) << "Uninitialized handle when using FrameGraphResources.";

    VirtualResource* const resource = mFrameGraph.getResource(handle);

    auto& declaredHandles = mPassNode.mDeclaredHandles;
    const bool hasReadOrWrite = declaredHandles.find(handle.index) != declaredHandles.cend();

    DANTE_CHECK_PRECONDITION(hasReadOrWrite)
            << "Pass \"" << mPassNode.getName() << "\" didn't declare any access to resource \""
            << resource->name.c_str() << "\"";

    assert_invariant(resource->refcount);

    return *resource;
}

FrameGraphResources::RenderPassInfo FrameGraphResources::getRenderPassInfo(uint32_t const id) const {
    // this cast is safe because this can only be called from a RenderPassNode
    RenderPassNode const& renderPassNode = static_cast<RenderPassNode const&>(mPassNode);
    RenderPassNode::RenderPassData const* pRenderPassData = renderPassNode.getRenderPassData(id);

    DANTE_CHECK_PRECONDITION(pRenderPassData) << "using invalid RenderPass index " << id
                                                 << " in Pass \"" << mPassNode.getName() << "\"";

    return { pRenderPassData->backend.target, pRenderPassData->backend.params };
}

} // namespace dante
