
#ifndef TNT_DANTE_BACKEND_OPENGL_OPENGLDRIVERBASE_H
#define TNT_DANTE_BACKEND_OPENGL_OPENGLDRIVERBASE_H

#include "DriverBase.h"

#include <backend/Platform.h>

#include <utils/CString.h>

namespace dante::backend {

class OpenGLDriverBase : public DriverBase {
protected:
    explicit OpenGLDriverBase(const Platform::DriverConfig& driverConfig)
        : DriverBase(driverConfig) {}
    ~OpenGLDriverBase() override;

public:
    virtual utils::CString getVendorString() const noexcept = 0;
    virtual utils::CString getRendererString() const noexcept = 0;
    virtual utils::CString getVersionString() const noexcept = 0;
};

} // dante::backend

#endif //TNT_DANTE_BACKEND_OPENGL_OPENGLDRIVERBASE_H
