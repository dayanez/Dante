
#include "MaterialVariants.h"

#include <private/dante/EngineEnums.h>
#include <private/dante/Variant.h>

#include <dante/MaterialEnums.h>

#include <backend/DriverEnums.h>

#include <vector>

namespace filamat {

std::vector<Variant> determineSurfaceVariants(
        dante::UserVariantFilterMask userVariantFilter, bool isLit, bool shadowMultiplier) {
    std::vector<Variant> variants;
    for (size_t k = 0; k < dante::VARIANT_COUNT; k++) {
        dante::Variant const variant(k);
        if (dante::Variant::isReserved(variant)) {
            continue;
        }

        dante::Variant filteredVariant =
                dante::Variant::filterUserVariant(variant, userVariantFilter);

        // Remove variants for unlit materials
        filteredVariant = dante::Variant::filterVariant(
                filteredVariant, isLit || shadowMultiplier);

        auto const vertexVariant = dante::Variant::filterVariantVertex(filteredVariant);
        if (vertexVariant == variant) {
            variants.emplace_back(variant, dante::backend::ShaderStage::VERTEX);
        }

        auto const fragmentVariant = dante::Variant::filterVariantFragment(filteredVariant);
        if (fragmentVariant == variant) {
            variants.emplace_back(variant, dante::backend::ShaderStage::FRAGMENT);
        }
    }
    return variants;
}

std::vector<Variant> determinePostProcessVariants() {
    std::vector<Variant> variants;
    // TODO: add a way to filter out post-process variants (e.g., the transparent variant if only
    // opaque is needed)
    for (dante::Variant::type_t k = 0; k < dante::POST_PROCESS_VARIANT_COUNT; k++) {
        dante::Variant const variant(k);
        variants.emplace_back(variant, dante::backend::ShaderStage::VERTEX);
        variants.emplace_back(variant, dante::backend::ShaderStage::FRAGMENT);
    }
    return variants;
}

std::vector<Variant> determineComputeVariants() {
    // TODO: should we have variants for compute shaders?
    std::vector<Variant> variants;
    dante::Variant const variant(0);
    variants.emplace_back(variant, dante::backend::ShaderStage::COMPUTE);
    return variants;
}

} // namespace filamat
