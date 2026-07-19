
#include "details/SwapChain.h"

#include "details/Engine.h"

#include <backend/CallbackHandler.h>

#include <utility>

namespace dante {

void* SwapChain::getNativeWindow() const noexcept {
    return downcast(this)->getNativeWindow();
}

void SwapChain::setFrameScheduledCallback(
        backend::CallbackHandler* handler, FrameScheduledCallback&& callback, uint64_t const flags) {
    downcast(this)->setFrameScheduledCallback(handler, std::move(callback), flags);
}

bool SwapChain::isFrameScheduledCallbackSet() const noexcept {
    return downcast(this)->isFrameScheduledCallbackSet();
}

void SwapChain::setFrameCompletedCallback(backend::CallbackHandler* handler,
            utils::Invocable<void(SwapChain*)>&& callback) noexcept {
    return downcast(this)->setFrameCompletedCallback(handler, std::move(callback));
}

bool SwapChain::isSRGBSwapChainSupported(Engine& engine) noexcept {
    return FSwapChain::isSRGBSwapChainSupported(downcast(engine));
}

bool SwapChain::isMSAASwapChainSupported(Engine& engine, uint32_t samples) noexcept {
    return FSwapChain::isMSAASwapChainSupported(downcast(engine), samples);
}

bool SwapChain::isProtectedContentSupported(Engine& engine) noexcept {
    return FSwapChain::isProtectedContentSupported(downcast(engine));
}

utils::tribool SwapChain::isFrameRateChangeSupported() const noexcept {
    return downcast(this)->isFrameRateChangeSupported();
}


void SwapChain::setFrameRate(float const frameRate, FrameRateCompatibility const compatibility,
        ChangeFrameRateStrategy const strategy) noexcept {
    downcast(this)->setFrameRate(frameRate, compatibility, strategy);
}

} // namespace dante

