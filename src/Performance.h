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

// Calculates CPU usage percentage between two sampling points
static float CalculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks)
{
    static unsigned long long previousTotalTicks = 0;
    static unsigned long long previousIdleTicks = 0;

    unsigned long long totalTicksSinceLastTime = totalTicks - previousTotalTicks;
    unsigned long long idleTicksSinceLastTime = idleTicks - previousIdleTicks;

    previousTotalTicks = totalTicks;
    previousIdleTicks = idleTicks;

    if (totalTicksSinceLastTime == 0) return 0.0f;

    return 1.0f - ((float)idleTicksSinceLastTime / totalTicksSinceLastTime);
}

// Fetches system times and computes the current CPU load
inline float GetCPULoad()
{
    FILETIME idleTime, kernelTime, userTime;
    
    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        return -1.0f;
    }

    unsigned long long idle = FileTimeToInt64(idleTime);
    unsigned long long total = FileTimeToInt64(kernelTime) + FileTimeToInt64(userTime);
   
	//this_thread::sleep_for(chrono::seconds(1));


    return CalculateCPULoad(idle, total);
}


inline float CPUSLEEP() {
    //calling cpu load function to compute the current cpu load
    GetCPULoad();

    //This will sleep the thread for 1 second 
    this_thread::sleep_for(chrono::seconds(1));



    return 0; 
}

