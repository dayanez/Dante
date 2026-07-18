/*
 * Copyright (C) 2023 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_SPIRVFIXUP_H
#define TNT_SPIRVFIXUP_H

#include <string>

namespace filamat {

/**
 * Performs a "fixup" operation on SPIR-V disassembly text, decorating the filament_gl_ClipDistance
 * output as the canonical gl_ClipDistance built-in.
 *
 * glslang does not support the EXT_clip_cull_distance extension. Writing directly to
 * gl_ClipDistance results in an error.
 *
 * To get around this, an ES shader should write instead to filament_gl_ClipDistance. After
 * compiling to SPIR-V, this function will modify the SPIR_V disassembly and decorate
 * filament_gl_ClipDistance as if it were gl_ClipDistance.
 *
 * For example, the source GLSL:
 * ~~~~~~~~~~
 * #version 310 es
 *
 * // The location is required but does not matter and will be replaced.
 * layout(location = 100) out float filament_gl_ClipDistance[1];
 *
 * void main() {
 *     filament_gl_ClipDistance[0] = 0.0f;
 * }
 * ~~~~~~~~~~
 *
 * This should only be used in SPIR-V generated for an ES environment.
 *
 * @param spirvDisassembly a reference to the SPIR-V disassembly, will be modified
 * @return true if the replacement was successful, false otherwise
 */
bool fixupClipDistance(std::string& spirvDisassembly);

}

#endif  // TNT_SPIRVFIXUP_H
