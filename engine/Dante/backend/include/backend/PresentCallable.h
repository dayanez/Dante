
//! \file

#ifndef TNT_DANTE_BACKEND_PRESENTCALLABLE
#define TNT_DANTE_BACKEND_PRESENTCALLABLE

#include <utils/compiler.h>

namespace dante::backend {

/**
 * A PresentCallable is a callable object that, when called, schedules a frame for presentation on
 * a SwapChain.
 *
 * Typically, Dante's backend is responsible for scheduling a frame's presentation. However,
 * there are certain cases where the application might want to control when a frame is scheduled for
 * presentation.
 *
 * For example, on iOS, UIKit elements can be synchronized to 3D content by scheduling a present
 * within a CATransaction:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * void myFrameScheduledCallback(PresentCallable presentCallable, void* user) {
 *     [CATransaction begin];
 *     // Update other UI elements...
 *     presentCallable();
 *     [CATransaction commit];
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * To obtain a PresentCallable, set a SwapChain::FrameScheduledCallback on a SwapChain with the
 * SwapChain::setFrameScheduledCallback method. The callback is called with a PresentCallable object
 * and optional user data:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * swapChain->setFrameScheduledCallback(nullptr, myFrameScheduledCallback);
 * if (renderer->beginFrame(swapChain)) {
 *     renderer->render(view);
 *     renderer->endFrame();
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @remark The PresentCallable mechanism for user-controlled presentation is only supported by
 * Dante's Metal backend. On other backends, the FrameScheduledCallback is still invoked, but the
 * PresentCallable passed to it is a no-op and calling it has no effect.
 *
 * When using the Metal backend, applications *must* call each PresentCallable they receive. Each
 * PresentCallable represents a frame that is waiting to be presented, and failing to call it
 * will result in a memory leak. To "cancel" the presentation of a frame, pass false to the
 * PresentCallable, which will cancel the presentation of the frame and release associated memory.
 *
 * @see Renderer, SwapChain::setFrameScheduledCallback
 */
class UTILS_PUBLIC PresentCallable {
public:

    using PresentFn = void(*)(bool presentFrame, void* user);
    static void noopPresent(bool, void*) {}

    PresentCallable(PresentFn fn, void* user) noexcept;
    ~PresentCallable() noexcept = default;
    PresentCallable(const PresentCallable& rhs) = default;
    PresentCallable& operator=(const PresentCallable& rhs) = default;

    /**
     * Call this PresentCallable, scheduling the associated frame for presentation. Pass false for
     * presentFrame to effectively "cancel" the presentation of the frame.
     *
     * @param presentFrame if false, will not present the frame but releases associated memory
     */
    void operator()(bool presentFrame = true) noexcept;

private:

    PresentFn mPresentFn;
    void* mUser = nullptr;

};

/**
 * @deprecated, FrameFinishedCallback has been renamed to SwapChain::FrameScheduledCallback.
 */
using FrameFinishedCallback UTILS_DEPRECATED = void(*)(PresentCallable callable, void* user);

} // namespace dante::backend

#endif // TNT_DANTE_BACKEND_PRESENTCALLABLE
