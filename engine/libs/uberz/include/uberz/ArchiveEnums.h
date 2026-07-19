
#ifndef UBERZ_ARCHIVE_ENUMS_H
#define UBERZ_ARCHIVE_ENUMS_H

#include <stdint.h>

namespace dante::uberz {

    enum class ArchiveFeature : uint64_t {
        UNSUPPORTED,
        OPTIONAL,
        REQUIRED,
    };

} // namespace dante::uberz

#endif // UBERZ_ARCHIVE_ENUMS_H
