
#ifndef TNT_DANTEMAT_DIC_METAL_LIBRARY_CHUNK_H
#define TNT_DANTEMAT_DIC_METAL_LIBRARY_CHUNK_H

#include "BlobDictionary.h"
#include "Chunk.h"
#include "Flattener.h"

#include <vector>

#include <stdint.h>

namespace dantemat {

class DictionaryMetalLibraryChunk final : public Chunk {
public:
    explicit DictionaryMetalLibraryChunk(BlobDictionary&& dictionary);
    ~DictionaryMetalLibraryChunk() = default;

private:
    void flatten(Flattener& f) override;

    BlobDictionary mDictionary;
    bool mStripDebugInfo;
};

} // namespace dantemat

#endif // TNT_DANTEMAT_DIC_METAL_LIBRARY_CHUNK_H
