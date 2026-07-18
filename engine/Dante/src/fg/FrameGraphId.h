/*
 * Copyright (C) 2021 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_FG_FRAMEGRAPHID_H
#define TNT_FILAMENT_FG_FRAMEGRAPHID_H

#include <limits>
#include <utility>

#include <stdint.h>

namespace filament {

template<typename T>
class FrameGraphId;

class Blackboard;
class FrameGraph;
class FrameGraphResources;
class PassNode;
class ResourceNode;

/** A handle on a resource */
class FrameGraphHandle {
public:
    using Index = uint16_t;
    using Version = uint16_t;

private:
    template<typename T>
    friend class FrameGraphId;

    friend class Blackboard;
    friend class FrameGraph;
    friend class FrameGraphResources;
    friend class PassNode;
    friend class ResourceNode;

    // private ctor -- this cannot be constructed by users
    FrameGraphHandle() noexcept = default;
    explicit FrameGraphHandle(Index const index) noexcept : index(index) {}

    // index to the resource handle
    static constexpr uint16_t UNINITIALIZED = std::numeric_limits<Index>::max();
    uint16_t index = UNINITIALIZED;     // index to a ResourceSlot
    Version version = 0;

public:
    FrameGraphHandle(FrameGraphHandle const& rhs) noexcept = default;

    FrameGraphHandle& operator=(FrameGraphHandle const& rhs) noexcept = default;

    bool isInitialized() const noexcept { return index != UNINITIALIZED; }

    uint16_t getIndex() const noexcept { return index; }

    operator bool() const noexcept { return isInitialized(); }

    void clear() noexcept { index = UNINITIALIZED; version = 0; }

    bool operator < (const FrameGraphHandle& rhs) const noexcept {
        return index < rhs.index;
    }

    bool operator == (const FrameGraphHandle& rhs) const noexcept {
        return (index == rhs.index);
    }

    bool operator != (const FrameGraphHandle& rhs) const noexcept {
        return !operator==(rhs);
    }
};

/** A typed handle on a resource */
template<typename RESOURCE>
class FrameGraphId : public FrameGraphHandle {
public:
    using FrameGraphHandle::FrameGraphHandle;
    FrameGraphId() noexcept = default;
    explicit FrameGraphId(FrameGraphHandle const r) : FrameGraphHandle(r) { }
};

} // namespace filament

#endif //TNT_FILAMENT_FG_FRAMEGRAPHID_H
