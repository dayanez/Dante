
#ifndef TNT_FILAMAT_MATERIAL_TEXT_CHUNK_H
#define TNT_FILAMAT_MATERIAL_TEXT_CHUNK_H

#include "Chunk.h"
#include "LineDictionary.h"
#include "ShaderEntry.h"

#include <vector>

namespace filamat {

class MaterialTextChunk final : public Chunk {
public:
    MaterialTextChunk(std::vector<TextEntry>&& entries, const LineDictionary& dictionary,
            ChunkType type) : Chunk(type), mEntries(std::move(entries)), mDictionary(dictionary) {
    }
    ~MaterialTextChunk() override = default;

private:
    void flatten(Flattener& f) override;

    void writeEntryAttributes(size_t entryIndex, Flattener& f) const noexcept;

    // Structure to keep track of duplicates.
    struct ShaderMapping {
        bool isDup = false;
        size_t dupOfIndex = 0;
    };
    std::vector<ShaderMapping> mDuplicateMap;

    const std::vector<TextEntry> mEntries;
    const LineDictionary& mDictionary;
};

} // namespace filamat
#endif // TNT_FILAMAT_TEXT_CHUNK_H
