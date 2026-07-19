
#pragma once

#include "materials/StaticMaterialInfo.h"

#include <utils/Slice.h>

namespace dante {

utils::Slice<const StaticMaterialInfo> getSsaoMaterialList() noexcept;

} // namespace dante
