
#ifndef TNT_DANTE_BACKEND_PRIVATE_ACQUIREDIMAGE_H
#define TNT_DANTE_BACKEND_PRIVATE_ACQUIREDIMAGE_H

#include <backend/DriverEnums.h>

#include <math/mat3.h>

namespace dante::backend {

class CallbackHandler;

// This lightweight POD allows us to bundle the state required to process an ACQUIRED stream.
// Since these types of external images need to be moved around and queued up, an encapsulation is
// very useful.

struct AcquiredImage {
    void* image = nullptr;
    backend::StreamCallback callback = nullptr;
    void* userData = nullptr;
    CallbackHandler* handler = nullptr;
};

} // namespace dante::backend

#endif // TNT_DANTE_BACKEND_PRIVATE_ACQUIREDIMAGE_H
