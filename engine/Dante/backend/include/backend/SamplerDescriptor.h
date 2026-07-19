
//! \file

#ifndef TNT_DANTE_BACKEND_SAMPLERDESCRIPTOR_H
#define TNT_DANTE_BACKEND_SAMPLERDESCRIPTOR_H

#include <backend/DriverEnums.h>
#include <backend/Handle.h>

#include <utils/compiler.h>

namespace dante::backend {

struct UTILS_PUBLIC SamplerDescriptor {
    Handle<HwTexture> t;
    SamplerParams s{};
};

} // namespace dante::backend

#endif // TNT_DANTE_BACKEND_SAMPLERDESCRIPTOR_H
