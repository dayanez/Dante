
#ifndef TNT_DANTEBRIDGE_SIBGENERATOR_H
#define TNT_DANTEBRIDGE_SIBGENERATOR_H

#include <private/dante/EngineEnums.h>
#include <private/dante/Variant.h>

#include <stddef.h>
#include <stdint.h>

namespace dante {

class SamplerInterfaceBlock;

class SibGenerator {
public:
    static SamplerInterfaceBlock const& getPerViewSib(Variant variant) noexcept;
    static SamplerInterfaceBlock const& getPerRenderableSib(Variant variant) noexcept;
    static SamplerInterfaceBlock const* getSib(dante::DescriptorSetBindingPoints bindingPoint, Variant variant) noexcept;
    // When adding a sampler block here, make sure to also update
    //      FMaterial::getSurfaceProgramSlow and FMaterial::getPostProcessProgramSlow if needed
};

} // namespace dante

#endif // TNT_DANTEBRIDGE_SIBGENERATOR_H
