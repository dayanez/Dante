
#include "MaterialBinaryChunk.h"

namespace filamat {

MaterialBinaryChunk::MaterialBinaryChunk(
        const std::vector<BinaryEntry>&& entries, ChunkType chunkType)
    : Chunk(chunkType), mEntries(entries) {}

void MaterialBinaryChunk::flatten(Flattener &f) {
    f.writeUint64(mEntries.size());
    for (const BinaryEntry& entry : mEntries) {
        f.writeUint8(uint8_t(entry.shaderModel));
        f.writeUint8(entry.variant.key);
        f.writeUint8(uint8_t(entry.stage));
        f.writeUint32(entry.dictionaryIndex);
    }
}

}  // namespace filamat
