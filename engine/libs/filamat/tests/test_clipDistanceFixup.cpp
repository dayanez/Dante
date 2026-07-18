/*
 * Copyright (C) 2023 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "SpirvFixup.h"

#include <gtest/gtest.h>

TEST(ClipDistanceFixup, NoReplacement) {
    std::string disassembly =
            "       OpDecorate %gl_PerVertex Block\n"
            "       %void = OpTypeVoid\n"
            "       %3 = OpTypeFunction %void\n";
    std::string expected =
            "       OpDecorate %gl_PerVertex Block\n"
            "       %void = OpTypeVoid\n"
            "       %3 = OpTypeFunction %void\n";
    EXPECT_EQ(filamat::fixupClipDistance(disassembly), false);
    EXPECT_EQ(disassembly, expected);
}

TEST(ClipDistanceFixup, BasicReplacement) {
    std::string disassembly =
            "       OpDecorate %gl_PerVertex Block\n"
            "       OpDecorate %filament_gl_ClipDistance Location 100\n"
            "       %void = OpTypeVoid\n"
            "       %3 = OpTypeFunction %void\n";
    std::string expected =
            "       OpDecorate %gl_PerVertex Block\n"
            "       OpDecorate %filament_gl_ClipDistance BuiltIn ClipDistance\n"
            "       %void = OpTypeVoid\n"
            "       %3 = OpTypeFunction %void\n";
    EXPECT_EQ(filamat::fixupClipDistance(disassembly), true);
    EXPECT_EQ(disassembly, expected);
}

TEST(ClipDistanceFixup, NoNewline) {
    std::string disassembly =
            "       OpDecorate %gl_PerVertex Block\n"
            "       OpDecorate %filament_gl_ClipDistance Location 100";
    std::string expected =
            "       OpDecorate %gl_PerVertex Block\n"
            "       OpDecorate %filament_gl_ClipDistance BuiltIn ClipDistance";
    EXPECT_EQ(filamat::fixupClipDistance(disassembly), true);
    EXPECT_EQ(disassembly, expected);
}
