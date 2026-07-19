
#ifndef TNT_DANTE_DETAILS_FENCE_H
#define TNT_DANTE_DETAILS_FENCE_H

#include "downcast.h"

#include <dante/Fence.h>

#include <backend/Handle.h>

#include <utils/compiler.h>
#include <utils/Condition.h>
#include <utils/Mutex.h>

namespace dante {

class FEngine;

struct FenceSignal {
    enum State : uint8_t { UNSIGNALED, SIGNALED, DESTROYED };
    State mState = UNSIGNALED;
};

class FFence : public Fence {
public:
    FFence(FEngine& engine);

    void terminate(FEngine& engine) noexcept;

    FenceStatus wait(Mode mode, uint64_t timeout);

    static FenceStatus waitAndDestroy(FFence* fence, Mode mode) noexcept;

private:
    FEngine& mEngine;
    // TODO: use custom allocator for these small objects
    std::shared_ptr<FenceSignal> mFenceSignal;
};

DANTE_DOWNCAST(Fence)

} // namespace dante

#endif // TNT_DANTE_DETAILS_FENCE_H
