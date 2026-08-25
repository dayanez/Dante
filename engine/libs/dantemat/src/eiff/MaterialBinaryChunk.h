
#ifndef TNT_DANTEMAT_MATERIAL_BINARY_CHUNK_H
#define TNT_DANTEMAT_MATERIAL_BINARY_CHUNK_H

#include "Chunk.h"
#include "ShaderEntry.h"

#include <vector>

namespace dantemat {

class MaterialBinaryChunk final : public Chunk {
public:
    explicit MaterialBinaryChunk(const std::vector<BinaryEntry>&& entries, ChunkType type);
    ~MaterialBinaryChunk() = default;

private:
    void flatten(Flattener& f) override;

    const std::vector<BinaryEntry> mEntries;
};

} // namespace dantemat

#endif // TNT_DANTEMAT_MATERIAL_BINARY_CHUNK_H
