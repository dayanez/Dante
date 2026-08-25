
#ifndef TNT_ENUMMANAGER_H
#define TNT_ENUMMANAGER_H

#include <dantemat/MaterialBuilder.h>

#include <algorithm>
#include <string_view>
#include <unordered_map>

namespace dantemat {

using Property = MaterialBuilder::Property;
using UniformType = MaterialBuilder::UniformType;
using SamplerType = MaterialBuilder::SamplerType;
using SubpassType = MaterialBuilder::SubpassType;
using SamplerFormat = MaterialBuilder::SamplerFormat;
using ParameterPrecision = MaterialBuilder::ParameterPrecision;
using OutputTarget = MaterialBuilder::OutputTarget;
using OutputQualifier = MaterialBuilder::VariableQualifier;
using OutputType = MaterialBuilder::OutputType;
using ConstantType = MaterialBuilder::ConstantType;
using ShaderStageType = MaterialBuilder::ShaderStageFlags;

// Convenience methods to convert std::string_view to Enum and also iterate over Enum values.
class Enums {
public:

    // Returns true if string "s" is a valid string representation of an element of enum T.
    template<typename T>
    static bool isValid(const std::string_view& s) noexcept {
        std::unordered_map<std::string_view, T>& map = getMap<T>();
        return map.find(s) != map.end();
    }

    // Return enum matching its string representation. Returns undefined if s is not a valid enum T
    // value. You should always call isValid() first to validate a string before calling toEnum().
    template<typename T>
    static T toEnum(const std::string_view& s) noexcept {
        std::unordered_map<std::string_view, T>& map = getMap<T>();
        return map.at(s);
    }

    template<typename T>
    static std::string_view toString(T t) noexcept;

    // Return a map of all values in an enum with their string representation.
    template<typename T>
    static std::unordered_map<std::string_view, T>& map() noexcept {
        auto& map = getMap<T>();
        return map;
    };

private:
    template<typename T>
    static std::unordered_map<std::string_view, T>& getMap() noexcept;

    static std::unordered_map<std::string_view, Property> mStringToProperty;
    static std::unordered_map<std::string_view, UniformType> mStringToUniformType;
    static std::unordered_map<std::string_view, SamplerType> mStringToSamplerType;
    static std::unordered_map<std::string_view, SubpassType> mStringToSubpassType;
    static std::unordered_map<std::string_view, SamplerFormat> mStringToSamplerFormat;
    static std::unordered_map<std::string_view, ParameterPrecision> mStringToSamplerPrecision;
    static std::unordered_map<std::string_view, OutputTarget> mStringToOutputTarget;
    static std::unordered_map<std::string_view, OutputQualifier> mStringToOutputQualifier;
    static std::unordered_map<std::string_view, OutputType> mStringToOutputType;
    static std::unordered_map<std::string_view, ConstantType> mStringToConstantType;
    static std::unordered_map<std::string_view, ShaderStageType> mStringToShaderStageType;
};

template<typename T>
std::string_view Enums::toString(T t) noexcept {
    auto& map = getMap<T>();
    auto result = std::find_if(map.begin(), map.end(), [t](auto& pair) {
        return pair.second == t;
    });
    if (result != map.end()) {
        return result->first;
    }
    return "";
}

} // namespace dantemat

#endif //TNT_ENUMMANAGER_H
