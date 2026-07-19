
#ifndef TNT_DANTE_BACKEND_OPENGL_BINDINGMAP_H
#define TNT_DANTE_BACKEND_OPENGL_BINDINGMAP_H

#include "gl_headers.h"

#include <backend/DriverEnums.h>

#include <utils/bitset.h>
#include <utils/debug.h>

#include <new>

#include <stddef.h>
#include <stdint.h>
#include <string.h>

namespace dante::backend {

class BindingMap {
    struct CompressedBinding {
        // this is in fact a GLuint, but we only want 8-bits
        uint8_t binding : 7;
        uint8_t sampler : 1;
    };

    CompressedBinding (*mStorage)[MAX_DESCRIPTOR_COUNT];

    utils::bitset64 mActiveDescriptors[MAX_DESCRIPTOR_SET_COUNT];

public:
    BindingMap() noexcept
            : mStorage(new (std::nothrow) CompressedBinding[MAX_DESCRIPTOR_SET_COUNT][MAX_DESCRIPTOR_COUNT]) {
#ifndef NDEBUG
        memset(mStorage, 0xFF, sizeof(CompressedBinding[MAX_DESCRIPTOR_SET_COUNT][MAX_DESCRIPTOR_COUNT]));
#endif
    }

    ~BindingMap() noexcept {
        delete [] mStorage;
    }

    BindingMap(BindingMap const&) noexcept = delete;
    BindingMap(BindingMap&&) noexcept = delete;
    BindingMap& operator=(BindingMap const&) noexcept = delete;
    BindingMap& operator=(BindingMap&&) noexcept = delete;

    struct Binding {
        GLuint binding;
        DescriptorType type;
    };

    void insert(descriptor_set_t set, descriptor_binding_t binding, Binding entry) noexcept {
        assert_invariant(set < MAX_DESCRIPTOR_SET_COUNT);
        assert_invariant(binding < MAX_DESCRIPTOR_COUNT);
        assert_invariant(entry.binding < 128); // we reserve 1 bit for the type right now
        mStorage[set][binding] = { uint8_t(entry.binding),
                                   DescriptorSetLayoutDescriptor::isSampler(entry.type) };
        mActiveDescriptors[set].set(binding);
    }

    GLuint get(descriptor_set_t set, descriptor_binding_t binding) const noexcept {
        assert_invariant(set < MAX_DESCRIPTOR_SET_COUNT);
        assert_invariant(binding < MAX_DESCRIPTOR_COUNT);
        return mStorage[set][binding].binding;
    }

    utils::bitset64 getActiveDescriptors(descriptor_set_t set) const noexcept {
        return mActiveDescriptors[set];
    }
};

} // namespace dante::backend

#endif //TNT_DANTE_BACKEND_OPENGL_BINDINGMAP_H
