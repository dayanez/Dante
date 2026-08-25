
#include "DictionaryTextChunk.h"

#include "Chunk.h"
#include "Flattener.h"
#include "LineDictionary.h"

#include <dante/MaterialChunkType.h>

#include <cstddef>
#include <utility>

namespace dantemat {

DictionaryTextChunk::DictionaryTextChunk(LineDictionary&& dictionary, ChunkType const chunkType) :
        Chunk(chunkType), mDictionary(std::move(dictionary)) {
}

void DictionaryTextChunk::flatten(Flattener& f) {
    // NumStrings
    f.writeUint32(mDictionary.getDictionaryLineCount());

    // Strings
    for (LineDictionary::index_t i = 0, c = mDictionary.getDictionaryLineCount() ; i < c ; i++) {
        f.writeString(mDictionary.getString(i).data());
    }
}

} // namespace dantemat
