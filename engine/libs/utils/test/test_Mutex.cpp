
#include <utils/debug/Mutex.h>
#include <utils/Mutex.h>
#include <utils/Panic.h>

#include <gtest/gtest.h>

#include <thread>
#include <vector>

using namespace utils;

TEST(MutexTest, BasicLockUnlock) {
    Mutex mLock;
    mLock.lock();
    mLock.unlock();

    EXPECT_TRUE(mLock.try_lock());
    mLock.unlock();

    {
        LockGuard const guard(mLock);
    }

    {
        UniqueLock lock(mLock);
        lock.unlock();
        lock.lock();
    }
}

TEST(DebugMutexTest, SelfDeadlock) {
#ifdef GTEST_HAS_DEATH_TEST
    EXPECT_DEATH({
        debug::Mutex mLock;
        mLock.lock();
        mLock.lock();
    }, "Self-Deadlock / Recursive Lock Detected");
#endif
}

TEST(DebugMutexTest, ValidOrderNoCycle) {
    debug::Mutex A;
    debug::Mutex B;
    debug::Mutex C;

    A.lock();
    B.lock();
    C.lock();

    C.unlock();
    B.unlock();
    A.unlock();
}

TEST(DebugMutexTest, DirectOrderInversion) {
#ifdef GTEST_HAS_DEATH_TEST
    EXPECT_DEATH({
        debug::Mutex A;
        debug::Mutex B;

        // Thread 1 establishes A -> B order
        A.lock();
        B.lock();
        B.unlock();
        A.unlock();

        // Thread 2 attempts B -> A inversion
        std::thread t([&A, &B]() {
            B.lock();
            A.lock();
        });
        t.join();
    }, "Lock-Order Inversion / Potential Deadlock Detected");
#endif
}

TEST(DebugMutexTest, TransitiveOrderInversion) {
#ifdef GTEST_HAS_DEATH_TEST
    EXPECT_DEATH({
        debug::Mutex A;
        debug::Mutex B;
        debug::Mutex C;

        // Thread 1 establishes A -> B
        A.lock();
        B.lock();
        B.unlock();
        A.unlock();

        // Thread 2 establishes B -> C
        std::thread t2([&B, &C]() {
            B.lock();
            C.lock();
            C.unlock();
            B.unlock();
        });
        t2.join();

        // Thread 3 attempts C -> A (closing the 3-node cycle A -> B -> C -> A)
        std::thread t3([&C, &A]() {
            C.lock();
            A.lock();
        });
        t3.join();
    }, "Lock-Order Inversion / Potential Deadlock Detected");
#endif
}

TEST(DebugMutexTest, DestructionPointerReuse) {
    debug::Mutex B;
    {
        debug::Mutex A;
        A.lock();
        B.lock();
        B.unlock();
        A.unlock();
    } // A is destroyed here, clearing edge A -> B from the graph

    // Now creating a new mutex and establishing B -> A2 should not trigger a cycle
    debug::Mutex A2;
    B.lock();
    A2.lock();
    A2.unlock();
    B.unlock();
}

TEST(DebugMutexTest, OutOfOrderUnlock) {
    debug::Mutex A;
    debug::Mutex B;

    A.lock();
    B.lock();
    // Unlock out of LIFO order
    A.unlock();
    B.unlock();
}

TEST(DebugMutexTest, EbrStallTimelineLockInversion) {
#ifdef GTEST_HAS_DEATH_TEST
    EXPECT_DEATH({
        debug::Mutex mEbrEntitiesLock; // Lock A
        debug::Mutex mTimelineLock;    // Lock B

        // Trace 1 (Main Thread): removeComponentsImpl() locks A, then suspend() -> unregisterWatermark() locks B
        mEbrEntitiesLock.lock();
        mTimelineLock.lock();
        mTimelineLock.unlock();
        mEbrEntitiesLock.unlock();

        // Trace 2 (Background/Engine Thread): advanceEpoch() locks B, then getExpiredEpochsLocked() locks A
        std::thread backgroundThread([&mTimelineLock, &mEbrEntitiesLock]() {
            mTimelineLock.lock();
            mEbrEntitiesLock.lock();
        });
        backgroundThread.join();
    }, "Lock-Order Inversion / Potential Deadlock Detected");
#endif
}
