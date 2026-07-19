
#ifndef TNT_FILAMAT_SIMPLE_FIELD_CHUNK_H
#define TNT_FILAMAT_SIMPLE_FIELD_CHUNK_H

#include "Chunk.h"
#include "Flattener.h"

namespace filamat {

template <class T>
class SimpleFieldChunk final : public Chunk {
public:
    SimpleFieldChunk(ChunkType type, T value) : Chunk(type), t(value) {}
    ~SimpleFieldChunk() = default;

private:
    void flatten(Flattener &f) override;

    T t;
};

} // namespace filamat
#endif // TNT_FILAMAT_SIMPLE_FIELD_CHUNK_H