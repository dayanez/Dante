
#ifndef TNT_DANTEMAT_PUSH_CONSTANT_DEFINTITIONS_H
#define TNT_DANTEMAT_PUSH_CONSTANT_DEFINTITIONS_H

#include <private/dante/EngineEnums.h>
#include <private/dante/PushConstantInfo.h>

#include <utils/FixedCapacityVector.h>

#include <tuple>

namespace dantemat {

constexpr char PUSH_CONSTANT_STRUCT_VAR_NAME[] = "pushConstants";

utils::FixedCapacityVector<dante::MaterialPushConstant> const PUSH_CONSTANTS = {
    {
        "morphingBufferOffset",
        dante::backend::ConstantType::INT,
        dante::backend::ShaderStage::VERTEX,
    },
};

// Make sure that the indices defined in dantebridge match the actual array indices defined here.
static_assert(static_cast<uint8_t>(dante::PushConstantIds::MORPHING_BUFFER_OFFSET) == 0u);

}// namespace dantemat

#endif
