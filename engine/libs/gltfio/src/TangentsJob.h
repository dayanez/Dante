/*
 * Copyright (C) 2021 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <math/vec4.h>

#include <cgltf.h>

namespace filament {

class VertexBuffer;
class MorphTargetBuffer;

}

namespace filament::gltfio {

/**
 * Internal helper that examines a cgltf primitive and generates data suitable for Filament's
 * TANGENTS attribute. This has been designed to be run as a JobSystem job, but clients are not
 * required to do so.
 */
struct TangentsJob {
    static constexpr int kMorphTargetUnused = -1;

    // The inputs to the procedure. The prim is owned by the client, which should ensure that it
    // stays alive for the duration of the procedure.
    struct InputParams {
        const cgltf_primitive* prim;
        const int morphTargetIndex = kMorphTargetUnused;
    };

    // The context of the procedure. These fields are not used by the procedure but are provided as
    // a convenience to clients. You can think of this as a scratch space for clients.
    struct Context {
        VertexBuffer* const vb;
        MorphTargetBuffer* const tb;
        uint32_t const offset;
        const uint8_t slot;
    };

    // The outputs of the procedure. The results array gets malloc'd by the procedure, so clients
    // should remember to free it.
    struct OutputParams {
        cgltf_size vertexCount;
        math::short4* results;
    };

    // Clients might want to track the jobs in an array, so the arguments are bundled into a struct.
    struct Params {
        InputParams in;
        Context context;
        OutputParams out;
    };

    // Performs tangents generation synchronously. This can be invoked from inside a job if desired.
    // The parameters structure is owned by the client.
    static void run(Params* params);
};

} // namespace filament::gltfio
