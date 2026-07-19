
#include "DictionarySpirvChunk.h"

#include <smolv.h>

namespace filamat {

DictionarySpirvChunk::DictionarySpirvChunk(BlobDictionary&& dictionary, bool stripDebugInfo) :
        Chunk(ChunkType::DictionarySpirv), mDictionary(std::move(dictionary)), mStripDebugInfo(stripDebugInfo) {
}

void DictionarySpirvChunk::flatten(Flattener& f) {

    // For now, 1 is the only acceptable compression scheme.
    f.writeUint32(1);

    uint32_t flags = 0;
    if (mStripDebugInfo) {
        flags |= smolv::kEncodeFlagStripDebugInfo;
    }

    f.writeUint32(mDictionary.getBlobCount());
    for (size_t i = 0 ; i < mDictionary.getBlobCount() ; i++) {
        std::string_view spirv = mDictionary.getBlob(i);
        smolv::ByteArray compressed;
        if (!smolv::Encode(spirv.data(), spirv.size(), compressed, flags)) {
            utils::slog.e << "Error with SPIRV compression" << utils::io::endl;
        }

        f.writeAlignmentPadding();
        f.writeBlob((const char*) compressed.data(), compressed.size());
    }
}

} // namespace filamat
