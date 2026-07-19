
#ifndef TNT_DANTE_HWDESCRIPTORSETLAYOUTFACTORY_H
#define TNT_DANTE_HWDESCRIPTORSETLAYOUTFACTORY_H

#include "Bimap.h"

#include <backend/DriverApiForward.h>
#include <backend/DriverEnums.h>
#include <backend/Handle.h>

#include <utils/Allocator.h>

#include <functional>

#include <stddef.h>
#include <stdint.h>

namespace dante {

class FEngine;

class HwDescriptorSetLayoutFactory {
public:
    using Handle = backend::DescriptorSetLayoutHandle;

    HwDescriptorSetLayoutFactory();
    ~HwDescriptorSetLayoutFactory() noexcept;

    HwDescriptorSetLayoutFactory(HwDescriptorSetLayoutFactory const& rhs) = delete;
    HwDescriptorSetLayoutFactory(HwDescriptorSetLayoutFactory&& rhs) noexcept = delete;
    HwDescriptorSetLayoutFactory& operator=(HwDescriptorSetLayoutFactory const& rhs) = delete;
    HwDescriptorSetLayoutFactory& operator=(HwDescriptorSetLayoutFactory&& rhs) noexcept = delete;

    void terminate(backend::DriverApi& driver) noexcept;

    struct Parameters { // 16 bytes + heap allocations
        backend::DescriptorSetLayout dsl;
        size_t hash() const noexcept;
    };

    friend bool operator==(Parameters const& lhs, Parameters const& rhs) noexcept;

    Handle create(backend::DriverApi& driver, backend::DescriptorSetLayout dsl) noexcept;

    void destroy(backend::DriverApi& driver, Handle handle) noexcept;

private:
    struct Key { // 24 bytes
        // The key should not be copyable, unfortunately due to how the Bimap works we have
        // to copy-construct it once.
        Key(Key const&) = default;
        Key& operator=(Key const&) = delete;
        Key& operator=(Key&&) noexcept = delete;
        explicit Key(Parameters const& params) : params(params), refs(1) { }
        Parameters params;
        mutable uint32_t refs;  // 4 bytes
        bool operator==(Key const& rhs) const noexcept {
            return params == rhs.params;
        }
    };

    struct KeyHasher {
        size_t operator()(Key const& p) const noexcept {
            return p.params.hash();
        }
    };

    struct Value { // 4 bytes
        Handle handle;
    };

    struct ValueHasher {
        size_t operator()(Value const v) const noexcept {
            return std::hash<Handle::HandleId>()(v.handle.getId());
        }
    };

    friend bool operator==(Value const lhs, Value const rhs) noexcept {
        return lhs.handle == rhs.handle;
    }

    // Size of the arena used for the "set" part of the bimap
    // about ~1K entries before fall back to heap
    static constexpr size_t SET_ARENA_SIZE = 24 * 1024;

    // Arena for the set<>, using a pool allocator inside a heap area.
    using PoolAllocatorArena = utils::Arena<
            utils::PoolAllocatorWithFallback<sizeof(Key)>,
            utils::LockingPolicy::NoLock,
            utils::TrackingPolicy::Untracked,
            utils::AreaPolicy::HeapArea>;


    // Arena where the set memory is allocated
    PoolAllocatorArena mArena;

    // The special Bimap
    Bimap<Key, Value, KeyHasher, ValueHasher,
            utils::STLAllocator<Key, PoolAllocatorArena>> mBimap;
};

} // namespace dante

#endif // TNT_DANTE_HWDESCRIPTORSETLAYOUTFACTORY_H
