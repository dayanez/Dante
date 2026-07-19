
#include "DictionaryMetalLibraryChunk.h"

namespace filamat {

DictionaryMetalLibraryChunk::DictionaryMetalLibraryChunk(BlobDictionary&& dictionary)
    : Chunk(ChunkType::DictionaryMetalLibrary), mDictionary(std::move(dictionary)) {}

void DictionaryMetalLibraryChunk::flatten(Flattener& f) {
    f.writeUint32(mDictionary.getBlobCount());
    for (size_t i = 0 ; i < mDictionary.getBlobCount() ; i++) {
        std::string_view blob = mDictionary.getBlob(i);
        f.writeAlignmentPadding();
        f.writeBlob((const char*) blob.data(), blob.size());
    }
}

} // namespace filamat
