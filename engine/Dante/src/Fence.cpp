
#include "details/Fence.h"

namespace dante {

using namespace backend;

FenceStatus Fence::waitAndDestroy(Fence* fence, Mode const mode) {
    return FFence::waitAndDestroy(downcast(fence), mode);
}

FenceStatus Fence::wait(Mode const mode, uint64_t const timeout) {
    return downcast(this)->wait(mode, timeout);
}

} // namespace dante
