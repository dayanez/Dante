
#include <utils/compiler.h>
#include <utils/PagedArenaBitset.h>
#include <utils/PagedArenaBitsetPool.h>

#include <memory>

namespace utils {

// ============================================================================
// PagedArenaBitsetPool
// ============================================================================

PagedArenaBitsetPool::PagedArenaBitsetPool() noexcept = default;

PagedArenaBitsetPool::~PagedArenaBitsetPool() noexcept = default;

PagedArenaBitsetPool::ScopedBitset PagedArenaBitsetPool::get() {
    if (UTILS_UNLIKELY(mFreeList.empty())) {
        mFreeList.push_back(std::make_unique<PagedArenaBitset>());
    }
    PagedArenaBitset* bitset = mFreeList.back().release();
    mFreeList.pop_back();
    return ScopedBitset(*bitset, *this);
}

void PagedArenaBitsetPool::release(PagedArenaBitset& bitset) {
    bitset.clear();
    mFreeList.push_back(std::unique_ptr<PagedArenaBitset>(&bitset));
}

void PagedArenaBitsetPool::clear() noexcept {
    mFreeList.clear();
}

// ============================================================================
// PagedArenaBitsetPool::ScopedBitset
// ============================================================================

PagedArenaBitsetPool::ScopedBitset::~ScopedBitset() noexcept {
    if (UTILS_LIKELY(mBitset && mPool)) {
        mPool->release(*mBitset);
    }
}

PagedArenaBitsetPool::ScopedBitset& PagedArenaBitsetPool::ScopedBitset::operator=(ScopedBitset&& other) noexcept {
    if (UTILS_LIKELY(this != &other)) {
        if (UTILS_LIKELY(mBitset && mPool)) {
            mPool->release(*mBitset);
        }
        mBitset = other.mBitset;
        mPool = other.mPool;
        other.mBitset = nullptr;
        other.mPool = nullptr;
    }
    return *this;
}

} // namespace utils
