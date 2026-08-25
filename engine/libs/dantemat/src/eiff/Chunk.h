
#ifndef TNT_DANTEMAT_CHUNK_H
#define TNT_DANTEMAT_CHUNK_H

#include "Flattener.h"

#include <dante/MaterialChunkType.h>

#include <vector>

namespace dantemat {

class Chunk{
public:
    virtual ~Chunk();

    ChunkType getType() const noexcept {
        return mType;
    }

    virtual void flatten(Flattener &f) = 0;

protected:
    explicit Chunk(ChunkType type) : mType(type) {
    }

private:
    ChunkType mType;
};

} // namespace dantemat
#endif // TNT_DANTEMAT_CHUNK_H