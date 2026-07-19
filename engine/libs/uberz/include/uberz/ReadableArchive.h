
#ifndef UBERZ_READABLE_ARCHIVE_H
#define UBERZ_READABLE_ARCHIVE_H

#include <stdint.h>

#include <uberz/ArchiveEnums.h>

#include <dante/MaterialEnums.h>

namespace dante::uberz {

// ArchiveSpec is a parse-free binary format. The client simply casts a word-aligned content blob
// into a ReadableArchive struct pointer, then calls the following function to convert all the
// offset fields into pointers.
bool convertOffsetsToPointers(struct ReadableArchive* archive, size_t archiveSize);

UTILS_WARNING_PUSH
UTILS_WARNING_ENABLE_PADDED

// Precompiled set of materials bundled with a list of features flags that each material supports.
// This is the readable counterpart to WriteableArchive.
// Used by gltfio; users do not need to access this class directly.
struct ReadableArchive {
    uint32_t magic;
    uint32_t version;
    uint64_t specsCount;
    union {
        struct ArchiveSpec* specs;
        uint64_t specsOffset;
    };
};

static constexpr Shading INVALID_SHADING_MODEL = (Shading) 0xff;
static constexpr BlendingMode INVALID_BLENDING = (BlendingMode) 0xff;

struct ArchiveSpec {
    Shading shadingModel;
    BlendingMode blendingMode;
    uint16_t flagsCount;
    uint32_t packageByteCount;
    union {
        struct ArchiveFlag* flags;
        uint64_t flagsOffset;
    };
    union {
        uint8_t* package;
        uint64_t packageOffset;
    };
};

struct ArchiveFlag {
    union {
        const char* name;
        uint64_t nameOffset;
    };
    ArchiveFeature value;
};

UTILS_WARNING_POP

} // namespace dante::uberz

#endif // UBERZ_READABLE_ARCHIVE_H
