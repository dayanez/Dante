
#include "DFG.h"

#include "ZstdHelper.h"

#include "details/Engine.h"
#include "details/Texture.h"

#include "generated/resources/dfg.h"

#include <dante/Texture.h>

#include <backend/DriverEnums.h>

#include <utils/compiler.h>
#include <utils/debug.h>
#include <utils/Panic.h>

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <utility>

namespace dante {

void DFG::init(FEngine& engine) {
    constexpr size_t fp16Count = DFG_LUT_SIZE * DFG_LUT_SIZE * 3;
    constexpr size_t byteCount = fp16Count * sizeof(uint16_t);

    Texture::Builder builder = Texture::Builder()
            .width(DFG_LUT_SIZE)
            .height(DFG_LUT_SIZE)
            .format(backend::TextureFormat::RGB16F);

    if (ZstdHelper::isCompressed(DFG_PACKAGE, DFG_DFG_SIZE)) {
        const size_t decodedSize = ZstdHelper::getDecodedSize(DFG_PACKAGE, byteCount);
        assert_invariant(decodedSize == byteCount);

        // If the LUT is Zstd compressed, decompress it.
        void* decodedData = malloc(decodedSize);

        DANTE_CHECK_POSTCONDITION(decodedData)
                << "Couldn't allocate " << decodedSize << " bytes for DFG LUT decompression.";

        if (UTILS_LIKELY(decodedData)) {
            bool const success = ZstdHelper::decompress(
                    decodedData, decodedSize, DFG_PACKAGE, byteCount);

            DANTE_CHECK_POSTCONDITION(success) << "Couldn't decompress DFG LUT.";

            // Use decodedData for the texture
            Texture* lut = builder.build(engine);
            lut->setImage(engine, 0, { decodedData, decodedSize,
                    Texture::Format::RGB, Texture::Type::HALF,
                    +[](void* buffer, size_t, void*) { free(buffer); } });
            mLUT = downcast(lut);
            return;
        }
    }

    assert_invariant(DFG_DFG_SIZE == byteCount);
    Texture* lut = builder.build(engine);
    lut->setImage(engine, 0, { DFG_PACKAGE, byteCount, Texture::Format::RGB, Texture::Type::HALF });
    mLUT = downcast(lut);
}

void DFG::terminate(FEngine& engine) noexcept {
    if (mLUT) {
        engine.destroy(mLUT);
    }
}

} // namespace dante
