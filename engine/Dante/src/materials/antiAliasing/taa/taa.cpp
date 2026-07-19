
#include "generated/resources/taa.h"
#include "taa.h"

#include "materials/StaticMaterialInfo.h"

#include <utils/Slice.h>

#include <iterator>

#include <stddef.h>

namespace dante {

static const StaticMaterialInfo sMaterialList[] = {
        { "taa",                        MATERIAL(TAA, TAA) },
};

utils::Slice<const StaticMaterialInfo> getTaaMaterialList() noexcept {
    return { std::begin(sMaterialList), std::end(sMaterialList) };
}

} // namespace dante
