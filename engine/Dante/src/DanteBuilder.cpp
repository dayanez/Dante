
#include <dante/DanteAPI.h>

#include <utils/ImmutableCString.h>

#include <algorithm>

namespace dante {

void builderMakeName(utils::ImmutableCString& outName, const char* name, size_t const len) noexcept {
    if (!name) {
        return;
    }
    size_t const length = std::min(len, size_t { 128u });
    outName = utils::ImmutableCString(name, length);
}

} // namespace dante
