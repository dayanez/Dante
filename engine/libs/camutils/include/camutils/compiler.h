
#ifndef CAMUTILS_COMPILER_H
#define CAMUTILS_COMPILER_H

#if __has_attribute(visibility)
#    define CAMUTILS_PUBLIC __attribute__((visibility("default")))
#else
#    define CAMUTILS_PUBLIC
#endif

#endif // CAMUTILS_COMPILER_H
