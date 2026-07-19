
#ifndef TNT_DANTE_DESCRIPTORSETS_H
#define TNT_DANTE_DESCRIPTORSETS_H

#include <private/dante/EngineEnums.h>
#include <private/dante/Variant.h>

#include <dante/MaterialEnums.h>

#include <backend/DriverEnums.h>

#include <utils/CString.h>

namespace dante::descriptor_sets {

backend::DescriptorSetLayout const& getDepthVariantLayout() noexcept;
backend::DescriptorSetLayout const& getSsrVariantLayout() noexcept;

backend::DescriptorSetLayout const& getPerRenderableLayout() noexcept;

backend::DescriptorSetLayout getPerViewDescriptorSetLayout(
        MaterialDomain domain,
        bool isLit, bool isSSR, bool hasFog,
        bool isShadowSampler2D) noexcept;

backend::DescriptorSetLayout getPerViewDescriptorSetLayoutWithVariant(
        Variant variant,
        MaterialDomain domain,
        bool isLit, bool isSSR, bool hasFog) noexcept;

utils::CString getDescriptorName(
        DescriptorSetBindingPoints set,
        backend::descriptor_binding_t binding) noexcept;

backend::DescriptorType getDescriptorType(backend::SamplerType type, backend::SamplerFormat format);

} // namespace dante::descriptor_sets


#endif //TNT_DANTE_DESCRIPTORSETS_H
