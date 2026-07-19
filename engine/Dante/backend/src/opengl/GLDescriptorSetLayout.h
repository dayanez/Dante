
#ifndef TNT_DANTE_BACKEND_OPENGL_GLDESCRIPTORSETLAYOUT_H
#define TNT_DANTE_BACKEND_OPENGL_GLDESCRIPTORSETLAYOUT_H

#include "DriverBase.h"

#include <backend/DriverEnums.h>

#include <algorithm>
#include <utility>

#include <stdint.h>

namespace dante::backend {

struct GLDescriptorSetLayout : public HwDescriptorSetLayout, public DescriptorSetLayout {
    using HwDescriptorSetLayout::HwDescriptorSetLayout;
    explicit GLDescriptorSetLayout(DescriptorSetLayout&& layout) noexcept
            : DescriptorSetLayout(std::move(layout)) {

        std::sort(descriptors.begin(), descriptors.end(),
                [](auto&& lhs, auto&& rhs){
            return lhs.binding < rhs.binding;
        });

        auto p = std::max_element(descriptors.cbegin(), descriptors.cend(),
                [](auto const& lhs, auto const& rhs) {
            return lhs.binding < rhs.binding;
        });
        maxDescriptorBinding = p->binding;
    }
    uint8_t maxDescriptorBinding = 0;
};

} // namespace dante::backend

#endif //TNT_DANTE_BACKEND_OPENGL_GLDESCRIPTORSETLAYOUT_H
