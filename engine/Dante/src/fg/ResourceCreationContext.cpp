
#include "fg/details/ResourceCreationContext.h"

#include "FrameGraph.h"

namespace dante {

TextureCacheInterface& ResourceCreationContext::getTextureCache() const noexcept {
    return fg.getTextureCache();
}

} //namespace dante
