/*
 * Copyright (C) 2016 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_DETAILS_ALLOCATORS_H
#define TNT_FILAMENT_DETAILS_ALLOCATORS_H

#include <private/backend/BackendUtils.h>

#include <utils/Allocator.h>

namespace filament {

#ifndef NDEBUG

// on Debug builds, HeapAllocatorArena needs LockingPolicy::Mutex because it uses a
// TrackingPolicy, which needs to be synchronized.
using HeapAllocatorArena = utils::Arena<
        utils::HeapAllocator,
        utils::LockingPolicy::Mutex,
        utils::TrackingPolicy::DebugAndHighWatermark,
        utils::AreaPolicy::NullArea>;

using LinearAllocatorArena = utils::Arena<
        utils::LinearAllocator,
        utils::LockingPolicy::NoLock,
        utils::TrackingPolicy::DebugAndHighWatermark>;

#else

// on Release builds, HeapAllocatorArena doesn't need a LockingPolicy because HeapAllocator is
// intrinsically synchronized as it relies on heap allocations (i.e.: malloc/free)
using HeapAllocatorArena = utils::Arena<
        utils::HeapAllocator,
        utils::LockingPolicy::NoLock,
        utils::TrackingPolicy::Untracked,
        utils::AreaPolicy::NullArea>;

using LinearAllocatorArena = utils::Arena<
        utils::LinearAllocator,
        utils::LockingPolicy::NoLock>;

#endif

using RootArenaScope = utils::ArenaScope<LinearAllocatorArena>;

} // namespace filament

#endif // TNT_FILAMENT_DETAILS_ALLOCATORS_H
