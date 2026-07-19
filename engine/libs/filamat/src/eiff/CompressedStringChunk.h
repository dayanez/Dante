
#ifndef TNT_COMPRESSEDSTRINGCHUNK_H
#define TNT_COMPRESSEDSTRINGCHUNK_H

#include "Chunk.h"
#include "Flattener.h"

#include <utils/CString.h>

namespace filamat {

class CompressedStringChunk final : public Chunk {
public:
    enum class CompressionLevel { MIN, MAX, DEFAULT };
    CompressedStringChunk(
            ChunkType type, std::string_view string, CompressionLevel compressionLevel)
            : Chunk(type),
              mString(utils::CString(string.data(), string.size())),
              mCompressionLevel(compressionLevel) {}
    ~CompressedStringChunk() override = default;

private:
    void flatten(Flattener& f) override;
    utils::CString mString;
    CompressionLevel mCompressionLevel;
};

} // namespace filamat


#endif // TNT_COMPRESSEDSTRINGCHUNK_H
