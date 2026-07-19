
#include "fog.h"
#include "generated/resources/fog.h"

#include "materials/StaticMaterialInfo.h"

#include <utils/Slice.h>

#include <iterator>

#include <stddef.h>

namespace dante {

static const StaticMaterialInfo sMaterialList[] = {
        { "fog",                        MATERIAL(FOG, FOG) },
};

utils::Slice<const StaticMaterialInfo> getFogMaterialList() noexcept {
    return { std::begin(sMaterialList), std::end(sMaterialList) };
}

} // namespace dante
