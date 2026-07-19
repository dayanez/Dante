
#ifndef TNT_DANTE_BACKEND_SYSTRACEPROFILE_H
#define TNT_DANTE_BACKEND_SYSTRACEPROFILE_H

#include <private/utils/Tracing.h>

#define PROFILE_SCOPE(marker)       SYSTRACE_NAME(marker)

#define PROFILE_NAME_BEGINFRAME    "backend::beginFrame"
#define PROFILE_NAME_ENDFRAME      "backend::endFrame"

#endif // TNT_DANTE_BACKEND_SYSTRACEPROFILE_H

