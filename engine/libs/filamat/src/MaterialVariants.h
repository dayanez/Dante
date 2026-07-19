
#ifndef TNT_FILAMAT_MATERIAL_VARIANTS_H
#define TNT_FILAMAT_MATERIAL_VARIANTS_H

#include <private/dante/Variant.h>

#include <backend/DriverEnums.h>

#include <vector>

namespace filamat {

struct Variant {
    using Stage = dante::backend::ShaderStage;
    Variant(dante::Variant v, Stage s) noexcept : variant(v), stage(s) {}
    dante::Variant variant;
    Stage stage;
};

std::vector<Variant> determineSurfaceVariants(
        dante::UserVariantFilterMask, bool isLit, bool shadowMultiplier);

std::vector<Variant> determinePostProcessVariants();

std::vector<Variant> determineComputeVariants();

} // namespace filamat

#endif
