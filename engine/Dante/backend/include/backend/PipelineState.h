/*
 * Copyright (C) 2019 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_BACKEND_PIPELINESTATE_H
#define TNT_FILAMENT_BACKEND_PIPELINESTATE_H

#include <backend/DriverEnums.h>
#include <backend/Handle.h>

#include <array>

#include <stdint.h>

namespace utils::io {
class ostream;
} // namespace utils::io

namespace filament::backend {

//! \privatesection

struct PipelineLayout {
    using SetLayout = std::array<Handle<HwDescriptorSetLayout>, MAX_DESCRIPTOR_SET_COUNT>;
    SetLayout setLayout;      // 16
};

struct PipelineState {
    Handle<HwProgram> program;                                              //  4
    Handle<HwVertexBufferInfo> vertexBufferInfo;                            //  4
    PipelineLayout pipelineLayout;                                          // 16
    RasterState rasterState;                                                //  4
    StencilState stencilState;                                              // 12
    PolygonOffset polygonOffset;                                            //  8
    PrimitiveType primitiveType = PrimitiveType::TRIANGLES;                 //  1
    uint8_t padding[3] = {};                                                //  3
};

} // namespace filament::backend

#if !defined(NDEBUG)
utils::io::ostream& operator<<(utils::io::ostream& out, const filament::backend::PipelineState& ps);
#endif

#endif //TNT_FILAMENT_BACKEND_PIPELINESTATE_H
