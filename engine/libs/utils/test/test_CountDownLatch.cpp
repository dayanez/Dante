
#include <gtest/gtest.h>

#include <utils/CountDownLatch.h>
#include <atomic>
#include <thread>

using namespace utils;

template <typename T> using sptr = std::shared_ptr<T>;

TEST(CountDownLatchTest, Simple) {

    CountDownLatch send_latch(1);
    CountDownLatch receive_latch(1);

    std::atomic_int sendbuf;
    std::atomic_int receivebuf;

    std::thread t([&]()->int {
        receive_latch.await();
        int v = receivebuf.load();
        EXPECT_EQ(1, v);
        sendbuf.store(v*2);
        send_latch.latch();
        return 0;
    });

    // send an int to the thread
    receivebuf.store(1);
    receive_latch.latch();

    // receive an int back
    send_latch.await();
    EXPECT_EQ(2, sendbuf.load());

    t.join();
}
