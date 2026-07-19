
#include "PostProcessDescriptorSet.h"

#include "HwDescriptorSetLayoutFactory.h"
#include "TypedUniformBuffer.h"

#include "details/Engine.h"

#include <private/dante/DescriptorSets.h>
#include <private/dante/EngineEnums.h>
#include <private/dante/UibStructs.h>

#include <backend/DriverEnums.h>

namespace dante {

using namespace backend;
using namespace math;

PostProcessDescriptorSet::PostProcessDescriptorSet() noexcept = default;

void PostProcessDescriptorSet::init(FEngine& engine) noexcept {

    // create the descriptor-set layout
    mDescriptorSetLayout = dante::DescriptorSetLayout{
            engine.getDescriptorSetLayoutFactory(),
            engine.getDriverApi(), descriptor_sets::getDepthVariantLayout() };

    // create the descriptor-set from the layout
    mDescriptorSet = DescriptorSet{ "PostProcessDescriptorSet", mDescriptorSetLayout };
}

void PostProcessDescriptorSet::terminate(HwDescriptorSetLayoutFactory& factory, DriverApi& driver) {
    mDescriptorSet.terminate(driver);
    mDescriptorSetLayout.terminate(factory, driver);
}

void PostProcessDescriptorSet::setFrameUniforms(DriverApi& driver,
        TypedUniformBuffer<PerViewUib>& uniforms) noexcept {
    // initialize the descriptor-set
    mDescriptorSet.setBuffer(mDescriptorSetLayout,
            +PerViewBindingPoints::FRAME_UNIFORMS,
            uniforms.getUboHandle(), 0, uniforms.getSize());

    mDescriptorSet.commit(mDescriptorSetLayout, driver);
}

void PostProcessDescriptorSet::bind(DriverApi& driver) noexcept {
    mDescriptorSet.bind(driver, DescriptorSetBindingPoints::PER_VIEW);
}

} // namespace dante

