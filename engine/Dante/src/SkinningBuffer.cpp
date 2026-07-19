
#include "details/SkinningBuffer.h"

#include "details/Engine.h"

namespace dante {

using namespace backend;
using namespace math;

void SkinningBuffer::setBones(Engine& engine,
        RenderableManager::Bone const* transforms, size_t const count, size_t const offset) {
    downcast(this)->setBones(downcast(engine), transforms, count, offset);
}

void SkinningBuffer::setBones(Engine& engine,
        mat4f const* transforms, size_t const count, size_t const offset) {
    downcast(this)->setBones(downcast(engine), transforms, count, offset);
}

size_t SkinningBuffer::getBoneCount() const noexcept {
    return downcast(this)->getBoneCount();
}

} // namespace dante

