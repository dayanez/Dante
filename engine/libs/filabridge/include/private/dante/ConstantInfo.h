
#ifndef TNT_DANTE_CONSTANTINFO_H
#define TNT_DANTE_CONSTANTINFO_H

#include <backend/DriverEnums.h>

#include <utils/CString.h>

namespace dante {

struct MaterialConstant {
    using ConstantType = backend::ConstantType;
    using ConstantValue = backend::ConstantValue;

    utils::CString name;
    ConstantType type;
    ConstantValue defaultValue;

    MaterialConstant() = default;
    MaterialConstant(utils::CString name, ConstantType type, ConstantValue defaultValue)
            : name(std::move(name)), type(type), defaultValue(defaultValue) {}
};

}

#endif  // TNT_DANTE_CONSTANTINFO_H
