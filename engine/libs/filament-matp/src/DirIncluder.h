/*
 * Copyright (C) 2019 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_DIRINCLUDER_H_
#define TNT_DIRINCLUDER_H_

#include "IncludeCallback.h"

#include <utils/Path.h>

namespace matp {

// Functor callback handler used to resolve includes relative to a root include directory.
class DirIncluder {
public:
    void setIncludeDirectory(utils::Path dir) noexcept {
        assert(dir.isDirectory());
        mIncludeDirectory = dir;
    }

    bool operator()(const utils::CString& includedBy, IncludeResult& result);

private:
    utils::Path mIncludeDirectory;

};

} // namespace matp

#endif
