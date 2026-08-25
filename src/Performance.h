#pragma once
#include <chrono>
#include <thread>
#include <windows.h>

using namespace std; 


// Converts a FILETIME Windows structure into a 64-bit integer
static unsigned long long FileTimeToInt64(const FILETIME& ft)
{
    return (((unsigned long long)(ft.dwHighDateTime)) << 32) | ((unsigned long long)ft.dwLowDateTime);
}

// Calculates CPU usage as a percentage (0-100) between two sampling points. The previous
// (idleTicks, totalTicks) is kept in static state, so each call measures load over the
// interval since the *previous* call, not since system boot.
static float CalculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks)
{
    static unsigned long long previousTotalTicks = 0;
    static unsigned long long previousIdleTicks = 0;
    static bool hasPreviousSample = false;

    unsigned long long totalTicksSinceLastTime = totalTicks - previousTotalTicks;
    unsigned long long idleTicksSinceLastTime = idleTicks - previousIdleTicks;

    previousTotalTicks = totalTicks;
    previousIdleTicks = idleTicks;

    // First call has no real interval to measure yet (previous*Ticks defaults to 0, so the
    // "delta" above is actually ticks-since-boot) - just record the baseline and report 0
    // rather than a meaningless first reading.
    if (!hasPreviousSample) {
        hasPreviousSample = true;
        return 0.0f;
    }

    if (totalTicksSinceLastTime == 0) return 0.0f;

    // Must divide as floating point - dividing the unsigned long long tick counts directly
    // truncates to an integer 0 or 1 almost every time, which is what made this always report
    // either 0% or 100% regardless of actual load.
    double idleFraction = (double)idleTicksSinceLastTime / (double)totalTicksSinceLastTime;
    return (float)((1.0 - idleFraction) * 100.0);
}

// Fetches system times and computes the current system-wide CPU load, as a percentage (0-100).
inline float GetCPULoad()
{
    FILETIME idleTime, kernelTime, userTime;

    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        return -1.0f;
    }

    unsigned long long idle = FileTimeToInt64(idleTime);
    unsigned long long total = FileTimeToInt64(kernelTime) + FileTimeToInt64(userTime);

    return CalculateCPULoad(idle, total);
}

// System-wide CPU usage percentage, sampled at most twice a second rather than every frame -
// GetSystemTimes' delta over a single ~16ms frame is tiny and noisy, and nothing needs it
// queried 60-100+ times a second for a number a human is just glancing at. Never blocks (no
// sleep_for) - this runs inline in the render loop, and sleeping there would freeze rendering.
inline float GetCPUUsagePercent()
{
    static float lastValue = 0.0f;
    static std::chrono::steady_clock::time_point lastSampleTime{};

    auto const now = std::chrono::steady_clock::now();
    if (now - lastSampleTime >= std::chrono::milliseconds(500)) {
        lastSampleTime = now;
        float const load = GetCPULoad();
        if (load >= 0.0f) {
            lastValue = load;
        }
    }
    return lastValue;
}

