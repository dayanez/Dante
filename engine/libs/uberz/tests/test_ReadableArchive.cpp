/*
 * Copyright (C) 2026 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <uberz/ReadableArchive.h>

#include <gtest/gtest.h>

#include <array>
#include <string>

using filament::uberz::ArchiveFeature;
using filament::uberz::ArchiveFlag;
using filament::uberz::ArchiveSpec;
using filament::uberz::ReadableArchive;
using filament::uberz::convertOffsetsToPointers;

namespace {

TEST(ReadableArchiveTest, RejectsNullArchivePointer) {
    EXPECT_FALSE(convertOffsetsToPointers(nullptr, 64));
}

TEST(ReadableArchiveTest, RejectsSpecsOffsetOutsideBuffer) {
    alignas(8) std::array<uint8_t, 64> storage {};
    auto* archive = reinterpret_cast<ReadableArchive*>(storage.data());
    archive->magic = 'UBER';
    archive->version = 0;
    archive->specsCount = 1;
    archive->specsOffset = storage.size();

    EXPECT_FALSE(convertOffsetsToPointers(archive, storage.size()));
}

TEST(ReadableArchiveTest, RejectsFlagNamesOutsideBuffer) {
    alignas(8) std::array<uint8_t, 96> storage {};
    auto* archive = reinterpret_cast<ReadableArchive*>(storage.data());
    archive->magic = 'UBER';
    archive->version = 0;

    archive->specsCount = 1;
    archive->specsOffset = sizeof(ReadableArchive);

    auto* spec = reinterpret_cast<ArchiveSpec*>(storage.data() + archive->specsOffset);
    *spec = {};
    spec->flagsCount = 1;
    spec->flagsOffset = archive->specsOffset + sizeof(ArchiveSpec);
    spec->packageOffset = storage.size() - 1;

    auto* flag = reinterpret_cast<ArchiveFlag*>(storage.data() + spec->flagsOffset);
    flag->nameOffset = storage.size();
    flag->value = ArchiveFeature::OPTIONAL;

    EXPECT_FALSE(convertOffsetsToPointers(archive, storage.size()));
}

} // namespace

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
