
#include "fxaa.h"
#include "generated/resources/fxaa.h"

#include "materials/StaticMaterialInfo.h"

#include <utils/Slice.h>

#include <iterator>

#include <stddef.h>

namespace dante {

static const StaticMaterialInfo sMaterialList[] = {
        { "fxaa",                       MATERIAL(FXAA, FXAA) },
};

utils::Slice<const StaticMaterialInfo> getFxaaMaterialList() noexcept {
    return { std::begin(sMaterialList), std::end(sMaterialList) };
}

} // namespace dante
