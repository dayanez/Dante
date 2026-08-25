
#ifndef TNT_DANTEFLAT_DICTIONARY_READER_H
#define TNT_DANTEFLAT_DICTIONARY_READER_H

#include <danteflat/ChunkContainer.h>

namespace danteflat {

struct DictionaryReader {
    static bool unflatten(ChunkContainer const& container,
            ChunkContainer::Type dictionaryTag,
            BlobDictionary& dictionary);
};

} // namespace danteflat

#endif // TNT_DANTEFLAT_DICTIONARY_READER_H
