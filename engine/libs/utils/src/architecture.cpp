
#include <utils/architecture.h>

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#   include <unistd.h>
#elif defined(WIN32)
#   include <windows.h>
#endif

namespace utils::arch {

size_t getPageSize() noexcept {
#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
    return size_t(sysconf(_SC_PAGESIZE));
#elif defined(WIN32)
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    return size_t(sysInfo.dwPageSize);
#else
    return 4096u;
#endif
}

} // namespace utils::arch
