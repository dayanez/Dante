#ifndef TNT_DANTE_PROGRAMSPECIALIZATION_H
#define TNT_DANTE_PROGRAMSPECIALIZATION_H

#include "DynamicSpecConstKey.h"

#include <private/dante/Variant.h>

#include <backend/Program.h>

#include <utils/FixedCapacityVector.h>

#include <cstdint>

namespace dante {

// A program specialization is a collection of all properties which could yield a different compiled
// program object.
struct ProgramSpecialization {
    // TODO(exv): We would like to use the cache ID of the material instead of the CRC32. The cache
    // ID is supposed to uniquely identify shader programs, which may be shared across materials
    // (especially in the case of depth variants). However, due to runtime code generation, in
    // practice, these shader programs can end up differing significantly across materials.
    //
    // The proper fix for this problem is to identify every single concern that could cause a shader
    // program to possibly change at runtime and add those variables to ProgramSpecialization. Only
    // then can we switch from the CRC32 back to the cache ID and gain savings across material files
    // with identical shader programs.
    uint32_t materialCrc32;
    Variant variant;
    DynamicSpecConstKey specKey;
    utils::Slice<const backend::Program::SpecializationConstant> specializationConstants;

    size_t hash() const noexcept;
    bool operator==(ProgramSpecialization const& rhs) const noexcept;
};

} // namespace dante

namespace std {
template<>
struct hash<dante::ProgramSpecialization> {
    inline size_t operator()(const dante::ProgramSpecialization& lhs) const noexcept {
        return lhs.hash();
    }
};
} // namespace std

#endif  // TNT_DANTE_PROGRAMSPECIALIZATION_H
