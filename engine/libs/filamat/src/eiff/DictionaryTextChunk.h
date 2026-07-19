
#ifndef TNT_FILAMAT_DIC_TEXT_CHUNK_H
#define TNT_FILAMAT_DIC_TEXT_CHUNK_H

#include "Chunk.h"
#include "Flattener.h"
#include "LineDictionary.h"

#include <vector>

#include <stdint.h>

namespace filamat {

class DictionaryTextChunk final : public Chunk {
public:
    DictionaryTextChunk(LineDictionary&& dictionary, ChunkType chunkType);
    ~DictionaryTextChunk() = default;

    const LineDictionary& getDictionary() const noexcept { return mDictionary; }

private:
    void flatten(Flattener& f) override;

    const LineDictionary mDictionary;
};

} // namespace filamat

#endif // TNT_FILAMAT_DIC_TEXT_CHUNK_H
