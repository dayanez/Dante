
#include "BlobDictionary.h"

#include <assert.h>

namespace filamat {

size_t BlobDictionary::addBlob(const std::vector<uint8_t>& vblob) noexcept {
    std::string_view blob((char*) vblob.data(), vblob.size());
    auto iter = mBlobIndices.find(blob);
    if (iter != mBlobIndices.end()) {
        return iter->second;
    }
    mBlobs.emplace_back(std::make_unique<std::string>(blob));
    mBlobIndices.emplace(*mBlobs.back(), mBlobs.size() - 1);
    return mBlobs.size() - 1;
}

} // namespace filamat
