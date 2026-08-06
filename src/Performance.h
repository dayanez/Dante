#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <chrono>

inline double GetCpuUsage() {
    static ULONGLONG prevProcessTime = 0;
    static auto prevWallTime = std::chrono::steady_clock::now();

    FILETIME ftCreation, ftExit, ftKernel, ftUser;
    GetProcessTimes(GetCurrentProcess(), &ftCreation, &ftExit, &ftKernel, &ftUser);

    ULARGE_INTEGER ulKernel, ulUser;
    ulKernel.LowPart = ftKernel.dwLowDateTime;
    ulKernel.HighPart = ftKernel.dwHighDateTime;
    ulUser.LowPart = ftUser.dwLowDateTime; 
    ulUser.HighPart = ftUser.dwHighDateTime;

    ULONGLONG currentProcessTime = ulKernel.QuadPart + ulUser.QuadPart; // 100-nanosecond intervals

    auto now = std::chrono::steady_clock::now();
    auto wallClockDeltaMs = std::chrono::duration<double, std::milli>(now - prevWallTime).count();

    // Initialize on the first call
    if (prevProcessTime == 0) {
        prevProcessTime = currentProcessTime;
        prevWallTime = now;
        return 0.0;
    }

    ULONGLONG processTimeDelta = currentProcessTime - prevProcessTime;
    prevProcessTime = currentProcessTime;
    prevWallTime = now;

    if (wallClockDeltaMs <= 0.0) return 0.0;

    // 1 millisecond = 10,000 100-nanosecond units.
    // Formula: (Process Ticks Delta / Available Ticks in Wall Time) * 100
    double usage = (static_cast<double>(processTimeDelta) / (wallClockDeltaMs * 100.0));
    
    return usage; // e.g., 15.4 means 15.4% of a single core
}
