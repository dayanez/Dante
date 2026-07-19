
#include "details/DebugRegistry.h"

#include <math/vec2.h>
#include <math/vec3.h>
#include <math/vec4.h>

namespace dante {

using namespace math;

bool DebugRegistry::hasProperty(const char* name) const noexcept {
    return downcast(this)->hasProperty(name);
}

bool DebugRegistry::setProperty(const char* name, bool const v) noexcept {
    return downcast(this)->setProperty(name, v);
}

bool DebugRegistry::setProperty(const char* name, int const v) noexcept {
    return downcast(this)->setProperty(name, v);
}

bool DebugRegistry::setProperty(const char* name, float const v) noexcept {
    return downcast(this)->setProperty(name, v);
}

bool DebugRegistry::setProperty(const char* name, float2 const v) noexcept {
    return downcast(this)->setProperty(name, v);
}

bool DebugRegistry::setProperty(const char* name, float3 const v) noexcept {
    return downcast(this)->setProperty(name, v);
}

bool DebugRegistry::setProperty(const char* name, float4 const v) noexcept {
    return downcast(this)->setProperty(name, v);
}


bool DebugRegistry::getProperty(const char* name, bool* v) const noexcept {
    return downcast(this)->getProperty(name, v);
}

bool DebugRegistry::getProperty(const char* name, int* v) const noexcept {
    return downcast(this)->getProperty(name, v);
}

bool DebugRegistry::getProperty(const char* name, float* v) const noexcept {
    return downcast(this)->getProperty(name, v);
}

bool DebugRegistry::getProperty(const char* name, float2* v) const noexcept {
    return downcast(this)->getProperty(name, v);
}

bool DebugRegistry::getProperty(const char* name, float3* v) const noexcept {
    return downcast(this)->getProperty(name, v);
}

bool DebugRegistry::getProperty(const char* name, float4* v) const noexcept {
    return downcast(this)->getProperty(name, v);
}

void *DebugRegistry::getPropertyAddress(const char *name) {
    return  downcast(this)->getPropertyAddress(name);
}

void const *DebugRegistry::getPropertyAddress(const char *name) const noexcept {
    return  downcast(this)->getPropertyAddress(name);
}

DebugRegistry::DataSource DebugRegistry::getDataSource(const char* name) const noexcept {
    return  downcast(this)->getDataSource(name);
}


} // namespace dante

