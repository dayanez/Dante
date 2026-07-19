
/***********************************************************************
 * DEPRECATED                                                          *
 *                                                                     *
 * Systrace.h is deprecated as a public API. There is no replacement.  *
 * Applications should instead use the Perfetto SDK directly.          *
 ***********************************************************************/

#ifndef TNT_UTILS_SYSTRACE_H
#define TNT_UTILS_SYSTRACE_H

#define SYSTRACE_TAG_NEVER          (0)
#define SYSTRACE_TAG_ALWAYS         (1<<0)
#define SYSTRACE_TAG_DANTE       (1<<1)  // don't change, used in makefiles
#define SYSTRACE_TAG_JOBSYSTEM      (1<<2)

/*
 * The SYSTRACE_ macros use SYSTRACE_TAG as a the TAG, which should be defined
 * before this file is included. If not, the SYSTRACE_TAG_ALWAYS tag will be used.
 */

#ifndef SYSTRACE_TAG
#define SYSTRACE_TAG (SYSTRACE_TAG_ALWAYS)
#endif

// Systrace on Apple platforms is fragile and adds overhead, should only be enabled in dev builds.
#ifndef DANTE_APPLE_SYSTRACE
#define DANTE_APPLE_SYSTRACE 0
#endif

#if defined(__ANDROID__)
#include <utils/android/Systrace.h>
#elif defined(__APPLE__) && DANTE_APPLE_SYSTRACE
#include <utils/darwin/Systrace.h>
#else

#define SYSTRACE_ENABLE()
#define SYSTRACE_DISABLE()
#define SYSTRACE_CONTEXT()
#define SYSTRACE_NAME(name)
#define SYSTRACE_FRAME_ID(frame)
#define SYSTRACE_NAME_BEGIN(name)
#define SYSTRACE_NAME_END()
#define SYSTRACE_CALL()
#define SYSTRACE_ASYNC_BEGIN(name, cookie)
#define SYSTRACE_ASYNC_END(name, cookie)
#define SYSTRACE_VALUE32(name, val)
#define SYSTRACE_VALUE64(name, val)

#endif // ANDROID

#endif // TNT_UTILS_SYSTRACE_H
