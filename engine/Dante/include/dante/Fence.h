
//! \file

#ifndef TNT_DANTE_FENCE_H
#define TNT_DANTE_FENCE_H

#include <dante/DanteAPI.h>

#include <backend/DriverEnums.h>

#include <utils/compiler.h>

#include <stdint.h>

namespace dante {

/**
 * Fence is used to synchronize the application main thread with dante's rendering thread.
 */
class UTILS_PUBLIC Fence : public DanteAPI {
public:
    //! Special \p timeout value to disable wait()'s timeout.
    static constexpr uint64_t FENCE_WAIT_FOR_EVER = backend::FENCE_WAIT_FOR_EVER;

    //! Error codes for Fence::wait()
    using FenceStatus = backend::FenceStatus;

    /** Mode controls the behavior of the command stream when calling wait()
     *
     * @attention
     * It would be unwise to call `wait(..., Mode::DONT_FLUSH)` from the same thread
     * the Fence was created, as it would most certainly create a dead-lock.
     */
    enum class Mode : uint8_t {
        FLUSH,          //!< The command stream is flushed
        DONT_FLUSH      //!< The command stream is not flushed
    };

    /**
     * Client-side wait on the Fence.
     *
     * Blocks the current thread until the Fence signals.
     *
     * @param mode      Whether the command stream is flushed before waiting or not.
     * @param timeout   Wait time out in nanoseconds. Using a \p timeout of 0 is a way to query the state of the fence.
     *                  A \p timeout value of FENCE_WAIT_FOR_EVER is used to disable the timeout.
     * @return          FenceStatus::CONDITION_SATISFIED on success,
     *                  FenceStatus::TIMEOUT_EXPIRED if the time out expired or
     *                  FenceStatus::ERROR in other cases.
     * @throws std::exception (or derived) if the backend thread encountered an unrecoverable error (when exceptions are enabled and mode is Mode::FLUSH).
     * @throws utils::Panic if called again after a backend exception was already thrown.
     *
     * @see #Mode
     */
    FenceStatus wait(Mode mode = Mode::FLUSH, uint64_t timeout = FENCE_WAIT_FOR_EVER);

    /**
     * Client-side wait on a Fence and destroy the Fence.
     *
     * @param fence Fence object to wait on.
     *
     * @param mode  Whether the command stream is flushed before waiting or not.
     *
     * @return  FenceStatus::CONDITION_SATISFIED on success,
     *          FenceStatus::ERROR otherwise.
     */
    static FenceStatus waitAndDestroy(Fence* UTILS_NONNULL fence, Mode mode = Mode::FLUSH);

protected:
    // prevent heap allocation
    ~Fence() = default;
};

} // namespace dante

#endif // TNT_DANTE_FENCE_H
