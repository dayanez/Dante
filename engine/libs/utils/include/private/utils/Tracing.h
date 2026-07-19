
#ifndef TNT_UTILS_DANTE_TRACING_H
#define TNT_UTILS_DANTE_TRACING_H

#ifndef DANTE_TRACING_ENABLED
#    define DANTE_TRACING_ENABLED true
#endif

#define DANTE_TRACING_CATEGORY_DANTE        "dante/dante"
#define DANTE_TRACING_CATEGORY_JOBSYSTEM       "dante/jobsystem"
#define DANTE_TRACING_CATEGORY_GLTFIO          "dante/gltfio"

// Systrace on Apple platforms is fragile and adds overhead, should only be enabled in dev builds.
#ifndef DANTE_APPLE_SYSTRACE
#   define DANTE_APPLE_SYSTRACE 0
#endif

#if defined(__ANDROID__) && DANTE_TRACING_ENABLED
#    include <private/utils/android/Tracing.h>
#elif defined(__APPLE__) && DANTE_APPLE_SYSTRACE && DANTE_TRACING_ENABLED
#    include <private/utils/darwin/Tracing.h>
#else

#define DANTE_TRACING_ENABLE(category)
#define DANTE_TRACING_CONTEXT(category)
#define DANTE_TRACING_NAME(category, name)
#define DANTE_TRACING_FRAME_ID(category, frame)
#define DANTE_TRACING_NAME_BEGIN(category, name)
#define DANTE_TRACING_NAME_END(category)
#define DANTE_TRACING_CALL(category, ...)
#define DANTE_TRACING_ASYNC_BEGIN(category, name, cookie)
#define DANTE_TRACING_ASYNC_END(category, name, cookie)
#define DANTE_TRACING_VALUE(category, name, val)
#define DANTE_TRACING_EVENT(category, name, ...)

#endif

#endif // TNT_UTILS_DANTE_TRACING_H
