
#ifndef TNT_DANTEMAT_SIMPLE_FIELD_CHUNK_H
#define TNT_DANTEMAT_SIMPLE_FIELD_CHUNK_H

#include "Chunk.h"
#include "Flattener.h"

namespace dantemat {

template <class T>
class SimpleFieldChunk final : public Chunk {
public:
    SimpleFieldChunk(ChunkType type, T value) : Chunk(type), t(value) {}
    ~SimpleFieldChunk() = default;

private:
    void flatten(Flattener &f) override;

    T t;
};

} // namespace dantemat
#endif // TNT_DANTEMAT_SIMPLE_FIELD_CHUNK_H