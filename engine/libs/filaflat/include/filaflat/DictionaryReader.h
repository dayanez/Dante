
#ifndef TNT_FILAFLAT_DICTIONARY_READER_H
#define TNT_FILAFLAT_DICTIONARY_READER_H

#include <filaflat/ChunkContainer.h>

namespace filaflat {

struct DictionaryReader {
    static bool unflatten(ChunkContainer const& container,
            ChunkContainer::Type dictionaryTag,
            BlobDictionary& dictionary);
};

} // namespace filaflat

#endif // TNT_FILAFLAT_DICTIONARY_READER_H
