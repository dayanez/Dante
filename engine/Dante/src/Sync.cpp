
#include "details/Sync.h"

#include <backend/Platform.h>

namespace dante {

void Sync::getExternalHandle(Sync::CallbackHandler* handler, Sync::Callback callback,
        void* userData) noexcept {
    downcast(this)->getExternalHandle(handler, callback, userData);
}

} // namespace dante
