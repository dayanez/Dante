
#include "fsr.h"
#include "generated/resources/fsr.h"

#include "materials/StaticMaterialInfo.h"

#include <utils/Slice.h>

#include <iterator>

#include <stddef.h>

namespace dante {

static const StaticMaterialInfo sMaterialList[] = {
    { "fsr_easu",                   MATERIAL(FSR, FSR_EASU) },
    { "fsr_easu_mobile",            MATERIAL(FSR, FSR_EASU_MOBILE) },
    { "fsr_easu_mobileF",           MATERIAL(FSR, FSR_EASU_MOBILEF) },
    { "fsr_rcas",                   MATERIAL(FSR, FSR_RCAS) },
};

utils::Slice<const StaticMaterialInfo> getFsrMaterialList() noexcept {
    return { std::begin(sMaterialList), std::end(sMaterialList) };
}

} // namespace dante
