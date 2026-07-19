
#ifndef TNT_DANTE_POSTPROCESSINGDESCRIPTORSET_H
#define TNT_DANTE_POSTPROCESSINGDESCRIPTORSET_H

#include "DescriptorSet.h"
#include "DescriptorSetLayout.h"
#include "TypedUniformBuffer.h"

#include <private/dante/UibStructs.h>

#include <backend/DriverApiForward.h>

namespace dante {

class FEngine;
class HwDescriptorSetLayoutFactory;

class PostProcessDescriptorSet {
public:
    explicit PostProcessDescriptorSet() noexcept;

    void init(FEngine& engine) noexcept;

    void terminate(HwDescriptorSetLayoutFactory& factory, backend::DriverApi& driver);

    void setFrameUniforms(backend::DriverApi& driver,
            TypedUniformBuffer<PerViewUib>& uniforms) noexcept;

    void bind(backend::DriverApi& driver) noexcept;

    DescriptorSetLayout const& getLayout() const noexcept {
        return mDescriptorSetLayout;
    }

private:
    DescriptorSetLayout mDescriptorSetLayout;
    DescriptorSet mDescriptorSet;
};

} // namespace dante

#endif //TNT_DANTE_POSTPROCESSINGDESCRIPTORSET_H
