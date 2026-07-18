/*
 * Copyright (C) 2018 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_BACKEND_OPENGL_OPENGL_PLATFORM_WGL_H
#define TNT_FILAMENT_BACKEND_OPENGL_OPENGL_PLATFORM_WGL_H

#include <utils/unwindows.h>

#include <backend/DriverEnums.h>
#include <backend/platforms/OpenGLPlatform.h>

#include <utils/Mutex.h>

#include <windows.h>

#include <vector>

#include <stdint.h>

namespace filament::backend {

/**
 * A concrete implementation of OpenGLPlatform that supports WGL.
 */
class PlatformWGL : public OpenGLPlatform {
protected:
    // --------------------------------------------------------------------------------------------
    // Platform Interface

    Driver* createDriver(void* sharedGLContext,
            const Platform::DriverConfig& driverConfig) override;

    int getOSVersion() const noexcept final override { return 0; }

    // --------------------------------------------------------------------------------------------
    // OpenGLPlatform Interface

    void terminate() noexcept override;

    bool isExtraContextSupported() const noexcept override;
    void createContext(bool shared) override;

    SwapChain* createSwapChain(void* nativewindow, uint64_t flags) noexcept override;
    SwapChain* createSwapChain(uint32_t width, uint32_t height, uint64_t flags) noexcept override;
    void destroySwapChain(SwapChain* swapChain) noexcept override;
    bool makeCurrent(ContextType type, SwapChain* drawSwapChain, SwapChain* readSwapChain) override;
    void commit(SwapChain* swapChain) noexcept override;

protected:
    HGLRC mContext = NULL;
    HWND mHWnd = NULL;
    HDC mWhdc = NULL;
    PIXELFORMATDESCRIPTOR mPfd = {};
    std::vector<int> mAttribs;

    // For shared contexts
    static constexpr int SHARED_CONTEXT_NUM = 2;
    mutable utils::Mutex mAdditionalContextsLock;
    std::vector<HGLRC> mAdditionalContexts UTILS_GUARDED_BY(mAdditionalContextsLock);
    std::atomic<int> mNextFreeSharedContextIndex{0};
};

} // namespace filament::backend

#endif // TNT_FILAMENT_BACKEND_OPENGL_OPENGL_PLATFORM_GLX_H
