
#ifndef TNT_DANTE_FG_DETAILS_UTILITIES_H
#define TNT_DANTE_FG_DETAILS_UTILITIES_H

#include "Allocators.h"

#include <memory>
#include <vector>

namespace dante {

class FrameGraph;

template<typename T, typename ARENA>
struct Deleter {
    ARENA* arena = nullptr;
    Deleter(ARENA& arena) noexcept: arena(&arena) {} // NOLINT
    void operator()(T* object) noexcept { arena->destroy(object); }
};

template<typename T, typename ARENA> using UniquePtr = std::unique_ptr<T, Deleter<T, ARENA>>;
template<typename T> using Allocator = utils::STLAllocator<T, LinearAllocatorArena>;
template<typename T> using Vector = std::vector<T, Allocator<T>>; // 32 bytes

} // namespace dante

#endif // TNT_DANTE_FG_DETAILS_UTILITIES_H
