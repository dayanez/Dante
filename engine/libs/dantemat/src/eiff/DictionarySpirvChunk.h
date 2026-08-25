
#ifndef TNT_DANTEMAT_DIC_SPIRV_CHUNK_H
#define TNT_DANTEMAT_DIC_SPIRV_CHUNK_H

#include "BlobDictionary.h"
#include "Chunk.h"
#include "Flattener.h"

#include <vector>

#include <stdint.h>

namespace dantemat {

class DictionarySpirvChunk final : public Chunk {
public:
    explicit DictionarySpirvChunk(BlobDictionary&& dictionary, bool stripDebugInfo);
    ~DictionarySpirvChunk() = default;

private:
    void flatten(Flattener& f) override;

    BlobDictionary mDictionary;
    bool mStripDebugInfo;
};

} // namespace dantemat

#endif // TNT_DANTEMAT_DIC_SPIRV_CHUNK_H
