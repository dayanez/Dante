
#include <backend/PresentCallable.h>

#include <utils/debug.h>
#include <utils/Panic.h>

namespace dante::backend {

PresentCallable::PresentCallable(PresentFn fn, void* user) noexcept
    : mPresentFn(fn), mUser(user) {
    assert_invariant(fn != nullptr);
}

void PresentCallable::operator()(bool presentFrame) noexcept {
    DANTE_CHECK_PRECONDITION(mPresentFn) << "This PresentCallable was already called. "
                                               "PresentCallables should be called exactly once.";
    mPresentFn(presentFrame, mUser);
    // Set mPresentFn to nullptr to denote that the callable has been called.
    mPresentFn = nullptr;
}

} // namespace dante::backend
