
#ifndef TNT_UTILS_DEBUG_H
#define TNT_UTILS_DEBUG_H

#include <utils/compiler.h>

namespace utils {
UTILS_PUBLIC
UTILS_NORETURN
void panic(const char *func, const char * file, int line, const char *assertion) noexcept;
} // namespace dante

#ifdef NDEBUG
#   define	assert_invariant(e)	((void)0)
#else
#   define	assert_invariant(e) \
            (UTILS_VERY_LIKELY(e) ? ((void)0) : utils::panic(__func__, __FILE__, __LINE__, #e))
#endif // NDEBUG

#endif // TNT_UTILS_DEBUG_H
