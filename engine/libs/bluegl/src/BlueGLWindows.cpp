/*
 * Copyright (C) 2018 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <windows.h>

namespace bluegl {

static HMODULE module = nullptr;

bool initBinder() {
    module = LoadLibraryA("opengl32.dll");
    return module != nullptr;
}

void* loadFunction(const char* name) {
    void *p = (void *) wglGetProcAddress(name);
    if (p == 0 || (p == (void*) 0x1) || (p == (void*) 0x2)
            || (p == (void*) 0x3) || (p == (void*) -1)) {
        p = (void *) GetProcAddress(module, name);
    }
    return p;
}

void shutdownBinder() {
}

} // namespace bluegl
