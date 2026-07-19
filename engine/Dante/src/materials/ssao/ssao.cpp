
#include "generated/resources/ssao.h"
#include "ssao.h"

#include "materials/StaticMaterialInfo.h"

#include <utils/Slice.h>

#include <iterator>

#include <stddef.h>

namespace dante {

static const StaticMaterialInfo sMaterialList[] = {
    { "bilateralBlur",              MATERIAL(SSAO, BILATERALBLUR) },
    { "bilateralBlurBentNormals",   MATERIAL(SSAO, BILATERALBLURBENTNORMALS) },
    { "mipmapDepth",                MATERIAL(SSAO, MIPMAPDEPTH) },
    { "sao",                        MATERIAL(SSAO, SAO) },
    { "saoBentNormals",             MATERIAL(SSAO, SAOBENTNORMALS) },
#ifndef DANTE_DISABLE_GTAO
    { "gtao",                       MATERIAL(SSAO, GTAO) },
    { "gtaoBentNormals",            MATERIAL(SSAO, GTAOBENTNORMALS) },
#endif
};

utils::Slice<const StaticMaterialInfo> getSsaoMaterialList() noexcept {
    return { std::begin(sMaterialList), std::end(sMaterialList) };
}

} // namespace dante
