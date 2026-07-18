/*
 * Copyright (C) 2023 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "SpirvFixup.h"

namespace filamat {

bool fixupClipDistance(std::string& spirvDisassembly) {
    size_t p = spirvDisassembly.find("OpDecorate %filament_gl_ClipDistance Location");
    if (p == std::string::npos) {
        return false;
    }
    size_t lineEnd = spirvDisassembly.find('\n', p);
    if (lineEnd == std::string::npos) {
        lineEnd = spirvDisassembly.size();
    }
    spirvDisassembly.replace(p, lineEnd - p,
            "OpDecorate %filament_gl_ClipDistance BuiltIn ClipDistance");
    return true;
}

} // namespace filamat
