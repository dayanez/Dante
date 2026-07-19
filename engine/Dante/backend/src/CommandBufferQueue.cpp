
#include <private/backend/CircularBuffer.h>
#include <private/backend/CommandBufferQueue.h>
#include <private/backend/CommandStream.h>

#include <private/utils/Tracing.h>

#include <utils/compiler.h>
#include <utils/debug.h>
#include <utils/Logger.h>
#include <utils/Mutex.h>
#include <utils/ostream.h>
#include <utils/Panic.h>

#include <algorithm>
#include <exception>
#include <iterator>
#include <mutex>
#include <utility>
#include <vector>

#include <stddef.h>
#include <stdint.h>

using namespace utils;

namespace dante::backend {

CommandBufferQueue::CommandBufferQueue(size_t const requiredSize, size_t const bufferSize, bool const paused)
        : mRequiredSize((requiredSize + (CircularBuffer::getBlockSize() - 1u)) & ~(CircularBuffer::getBlockSize() -1u)),
          mCircularBuffer(std::max(mRequiredSize, bufferSize)),
          mFreeSpace(mCircularBuffer.size()),
          mPaused(paused) {
    assert_invariant(mCircularBuffer.size() >= mRequiredSize);
}

CommandBufferQueue::~CommandBufferQueue() {
    assert_invariant(mCommandBuffersToExecute.empty());
}

void CommandBufferQueue::requestExit() {
    LockGuard const lock(mLock);
    mExitRequested = EXIT_REQUESTED;
    mCondition.notify_one();
}

bool CommandBufferQueue::isPaused() const noexcept {
    LockGuard const lock(mLock);
    return mPaused;
}

void CommandBufferQueue::setPaused(bool const paused) {
    LockGuard const lock(mLock);
    if (paused) {
        mPaused = true;
    } else {
        mPaused = false;
        mCondition.notify_one();
    }
}

bool CommandBufferQueue::isExitRequested() const {
    LockGuard const lock(mLock);
    return bool(mExitRequested);
}


#ifdef __EXCEPTIONS
void CommandBufferQueue::propagateBackendException() const {
    if (UTILS_VERY_UNLIKELY(hasUnrecoverableError())) {
        if (!mExceptionRethrown.exchange(true, std::memory_order_relaxed)) {
            std::rethrow_exception(mBackendException);
        } else {
            DANTE_CHECK_POSTCONDITION(false)
                    << "Engine is in unrecoverable state due to previous backend exception";
        }
    }
}
#endif

void CommandBufferQueue::flush() {
    DANTE_TRACING_CALL(DANTE_TRACING_CATEGORY_DANTE);
#ifdef __EXCEPTIONS
    if (UTILS_VERY_UNLIKELY(hasUnrecoverableError())) {
        // Drop the current buffer to avoid filling up the circular buffer
        mCircularBuffer.getBuffer();
        propagateBackendException();
    }
#endif

    CircularBuffer& circularBuffer = mCircularBuffer;
    if (circularBuffer.empty()) {
        return;
    }

    // add the terminating command
    // always guaranteed to have enough space for the NoopCommand
    new(circularBuffer.allocate(sizeof(NoopCommand))) NoopCommand(nullptr);

    const size_t requiredSize = mRequiredSize;

    // get the current buffer
    auto const [begin, end] = circularBuffer.getBuffer();

    assert_invariant(circularBuffer.empty());

    // size of the current buffer
    size_t const used = std::distance(
            static_cast<char const*>(begin), static_cast<char const*>(end));


    UniqueLock lock(mLock);

    // circular buffer is too small, we corrupted the stream
    DANTE_CHECK_POSTCONDITION(used <= mFreeSpace) <<
            "Backend CommandStream overflow. Commands are corrupted and unrecoverable.\n"
            "Please increase minCommandBufferSizeMB inside the Config passed to Engine::create.\n"
            "Space used at this time: " << used <<
            " bytes, overflow: " << used - mFreeSpace << " bytes";

    mFreeSpace -= used;
    mCommandBuffersToExecute.push_back({ begin, end });
    mCondition.notify_one();

    // wait until there is enough space in the buffer
    if (UTILS_UNLIKELY(mFreeSpace < requiredSize)) {

#ifndef NDEBUG
        size_t const totalUsed = circularBuffer.size() - mFreeSpace;
        DLOG(INFO) << "CommandStream used too much space (will block): "
                   << "needed space " << requiredSize << " out of " << mFreeSpace
                   << ", totalUsed=" << totalUsed << ", current=" << used
                   << ", queue size=" << mCommandBuffersToExecute.size() << " buffers";

        mHighWatermark = std::max(mHighWatermark, totalUsed);
#endif

        DANTE_TRACING_NAME(DANTE_TRACING_CATEGORY_DANTE, "waiting: CircularBuffer::flush()");

        DANTE_CHECK_POSTCONDITION(!mPaused) <<
                "CommandStream is full, but since the rendering thread is paused, "
                "the buffer cannot flush and we will deadlock. Instead, abort.";

        while (mFreeSpace < requiredSize) {
            // TODO: on macOS, we need to call pumpEvents from time to time
            mCondition.wait(lock);
        }
    }
}

std::vector<CommandBufferQueue::Range> CommandBufferQueue::waitForCommands() const {
    if constexpr (!UTILS_HAS_THREADING) {
        return std::move(mCommandBuffersToExecute);
    }
    UniqueLock lock(mLock);
    while ((mCommandBuffersToExecute.empty() || mPaused) && !mExitRequested) {
        mCondition.wait(lock);
    }
    return std::move(mCommandBuffersToExecute);
}

void CommandBufferQueue::releaseBuffer(CommandBufferQueue::Range const& buffer) {
    size_t const used = std::distance(
            static_cast<char const*>(buffer.begin), static_cast<char const*>(buffer.end));
    LockGuard const lock(mLock);
    mFreeSpace += used;
    mCondition.notify_one();
}

} // namespace dante::backend
