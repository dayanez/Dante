/*
 * Copyright (C) 2015 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <gtest/gtest.h>

#include <utils/CyclicBarrier.h>
#include <atomic>
#include <thread>

using namespace utils;

static inline bool isPrime(long number) {
    for (long i=2; i*i<=number; i++) {
        if (number % i == 0) {
            return false;
        }
    }
    return true;
}

static inline long countPrimes(long max) {
    long r = 0;
    for (long i=0 ; i<max ; i++) {
        if (isPrime(i)) {
            r++;
        }
    }
    return r;
}

TEST(CyclicBarrierTest, Simple) {
    std::atomic_int counter(0);
    int p0, p1, p2;
    int r0, r1, r2;
    const long k0 = countPrimes(0x7FFFF);
    const long k1 = countPrimes(0x7FFF);
    const long k2 = countPrimes(0x7FF);
    CyclicBarrier barrier(4);

    std::thread a( [&]()->int { p0=countPrimes(0x7FFFF); barrier.await(); r0 = p0+p1+p2; barrier.await(); return 0; } );
    std::thread b( [&]()->int { p1=countPrimes(0x7FFF);  barrier.await(); r1 = p0+p1+p2; barrier.await(); return 0; } );
    std::thread c( [&]()->int { p2=countPrimes(0x7FF);   barrier.await(); r2 = p0+p1+p2; barrier.await(); return 0; } );

    barrier.await();

    EXPECT_EQ(k0, p0);
    EXPECT_EQ(k1, p1);
    EXPECT_EQ(k2, p2);

    barrier.await();

    EXPECT_EQ(k0+k1+k2, r0);
    EXPECT_EQ(k0+k1+k2, r1);
    EXPECT_EQ(k0+k1+k2, r2);

    a.join();
    b.join();
    c.join();
}
