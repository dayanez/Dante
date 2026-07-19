
#ifndef TNT_DANTE_DETAILS_DFG_H
#define TNT_DANTE_DETAILS_DFG_H

#include "details/Texture.h"

#include <backend/Handle.h>

#include <utils/compiler.h>

#include <cstddef>
#include <cstdint>

namespace dante {

class FEngine;

#if !defined(DANTE_DFG_LUT_SIZE)
#define DANTE_DFG_LUT_SIZE 128
#endif

class DFG {
public:
    explicit DFG() noexcept = default;

    DFG(DFG const& rhs) = delete;
    DFG(DFG&& rhs) = delete;
    DFG& operator=(DFG const& rhs) = delete;
    DFG& operator=(DFG&& rhs) = delete;

    void init(FEngine& engine);

    size_t getLutSize() const noexcept {
        return DFG_LUT_SIZE;
    }

    bool isValid() const noexcept {
        return mLUT != nullptr;
    }

    backend::Handle<backend::HwTexture> getTexture() const noexcept {
        return mLUT->getHwHandle();
    }

    void terminate(FEngine& engine) noexcept;

private:
    FTexture* mLUT = nullptr;

    // make sure to use the right size here
    static constexpr size_t DFG_LUT_SIZE = DANTE_DFG_LUT_SIZE;
};

#undef DANTE_DFG_LUT_SIZE

} // namespace dante

#endif // TNT_DANTE_DETAILS_DFG_H
