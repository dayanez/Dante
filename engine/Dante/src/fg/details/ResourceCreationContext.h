
#pragma once

#include <backend/DriverApiForward.h>

namespace dante {

class FrameGraph;
class TextureCacheInterface;

struct ResourceCreationContext {
    FrameGraph& fg;
    backend::DriverApi& driver;
    bool useProtectedMemory;
    TextureCacheInterface& getTextureCache() const noexcept;
};

} // namespace dante
