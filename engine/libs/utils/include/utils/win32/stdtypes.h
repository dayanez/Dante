
#ifndef TNT_UTILS_WIN32_STDTYPES_H
#define TNT_UTILS_WIN32_STDTYPES_H

#if defined(WIN32)
#include <windows.h>

// Copied from linux libc sys/stat.h:
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#define PATH_MAX (MAX_PATH)

// For getcwd
#include <direct.h>
#define getcwd _getcwd

#endif
#endif // TNT_UTILS_WIN32_STDTYPES_H
