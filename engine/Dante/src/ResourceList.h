
#ifndef TNT_DANTE_RESOURCELIST_H
#define TNT_DANTE_RESOURCELIST_H

#include <utils/compiler.h>

#include <tsl/robin_set.h>

#include <stdint.h>

namespace dante {

class ResourceListBase {
public:
    using iterator = typename tsl::robin_set<void*>::iterator;
    using const_iterator = typename tsl::robin_set<void*>::const_iterator;

    explicit ResourceListBase(const char* typeName);
    ResourceListBase(ResourceListBase&& rhs) noexcept = default;

    ~ResourceListBase() noexcept;

    void insert(void* item);

    bool remove(void const* item);

    iterator find(void const* item);

    void clear() noexcept;

    bool empty() const noexcept {
        return mList.empty();
    }

    size_t size() const noexcept {
        return mList.size();
    }

    iterator begin() noexcept {
        return mList.begin();
    }

    iterator end() noexcept {
        return mList.end();
    }

    const_iterator begin() const noexcept {
        return mList.begin();
    }

    const_iterator end() const noexcept {
        return mList.end();
    }

protected:
    void forEach(void(*f)(void* user, void *p), void* user) const noexcept;
    tsl::robin_set<void*> mList;
#ifndef NDEBUG
private:
    // removing this saves 8-bytes because of padding of derived classes
    const char* const mTypeName;
#endif
};

// The split ResourceListBase / ResourceList allows us to reduce code size by keeping
// common code (operating on void*) separate.
//
template<typename T>
class ResourceList : private ResourceListBase {
public:
    using ResourceListBase::ResourceListBase;
    using ResourceListBase::forEach;
    using ResourceListBase::insert;
    using ResourceListBase::remove;
    using ResourceListBase::find;
    using ResourceListBase::empty;
    using ResourceListBase::size;
    using ResourceListBase::clear;
    using ResourceListBase::begin;
    using ResourceListBase::end;

    explicit ResourceList(const char* name) noexcept: ResourceListBase(name) {}

    ResourceList(ResourceList&& rhs) noexcept = default;

    ~ResourceList() noexcept = default;

    template<typename F>
    inline void forEach(F func) const noexcept {
        // turn closure into function pointer call, we do this to reduce code size.
        this->forEach(+[](void* user, void* p) {
            ((F*)user)->operator()((T*)p);
        }, &func);
    }
};

} // namespace dante

#endif // TNT_DANTE_RESOURCELIST_H
