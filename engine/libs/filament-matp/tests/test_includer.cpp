/*
 * Copyright (C) 2019 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <gtest/gtest.h>

#include "DirIncluder.h"

using namespace utils;
using namespace matp;

const utils::Path root = utils::Path(__FILE__).getParent();

// TODO: these tests are disabled as they fail on CI, which needs investigation.

TEST(DirIncluder, DISABLED_IncludeNonexistent) {
    DirIncluder includer;
    {
        IncludeResult i {
            .includeName = CString("nonexistent.h")
        };
        bool success = includer(CString(""), i);
        EXPECT_FALSE(success);
    }
    {
        utils::CString includerFile((root + "Foo.h").getPath().c_str());
        IncludeResult i {
            .includeName = CString("nonexistent.h")
        };
        bool success = includer(includerFile, i);
        EXPECT_FALSE(success);
    }
}

TEST(DirIncluder, DISABLED_IncludeFile) {
    DirIncluder includer;
    includer.setIncludeDirectory(root);

    IncludeResult result {
        .includeName = CString("Foo.h")
    };
    bool success = includer(CString(""), result);

    EXPECT_TRUE(success);

    // The result's source should be set to the contents of the includer file.
    EXPECT_STREQ("// test include file", result.text.c_str());

    // The result's name should be set to the full path to the header file.
    EXPECT_STREQ((root + "Foo.h").c_str(), result.name.c_str());
}

TEST(DirIncluder, DISABLED_IncludeFileFromIncluder) {
    DirIncluder includer;
    includer.setIncludeDirectory(root);

    utils::CString includerFile((root + "Dir/Baz.h").c_str());

    IncludeResult result {
        .includeName = CString("Bar.h")
    };
    bool success = includer(includerFile, result);

    EXPECT_STREQ("// Bar.h", result.text.c_str());

    EXPECT_STREQ((root + "Dir/Bar.h").c_str(), result.name.c_str());
}
