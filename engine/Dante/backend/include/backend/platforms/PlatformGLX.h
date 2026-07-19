
#ifndef TNT_DANTE_BACKEND_OPENGL_OPENGL_PLATFORM_GLX_H
#define TNT_DANTE_BACKEND_OPENGL_OPENGL_PLATFORM_GLX_H

#include <backend/DriverEnums.h>
#include <backend/platforms/OpenGLPlatform.h>

#include <bluegl/BlueGL.h>

#include <GL/glx.h>

#include <shared_mutex>
#include <thread>
#include <unordered_map>
#include <vector>

#include <stdint.h>

namespace dante::backend {

/**
 * A concrete implementation of OpenGLPlatform that supports GLX.
 */
class UTILS_SHARED_LINKING PlatformGLX : public OpenGLPlatform {
protected:
    // --------------------------------------------------------------------------------------------
    // Platform Interface

    Driver* createDriver(void* sharedGLContext,
            const DriverConfig& driverConfig) override;

    int getOSVersion() const noexcept final override { return 0; }

    // --------------------------------------------------------------------------------------------
    // OpenGLPlatform Interface

    void terminate() noexcept override;

    bool isExtraContextSupported() const noexcept override;
    void createContext(bool shared) override;
    void releaseContext() noexcept override;

    SwapChain* createSwapChain(void* nativewindow, uint64_t flags) noexcept override;
    SwapChain* createSwapChain(uint32_t width, uint32_t height, uint64_t flags) noexcept override;
    void destroySwapChain(SwapChain* swapChain) noexcept override;
    bool makeCurrent(ContextType type, SwapChain* drawSwapChain, SwapChain* readSwapChain) override;
    void commit(SwapChain* swapChain) noexcept override;

private:
    Display* mGLXDisplay;
    GLXContext mGLXContext{};
    GLXFBConfig mGLXConfig{};
    GLXPbuffer mDummySurface;
    std::vector<GLXPbuffer> mPBuffers;

    // Variables for shared contexts
    std::unordered_map<std::thread::id, GLXContext> mAdditionalContexts;
    std::shared_mutex mAdditionalContextsLock;
};

} // namespace dante::backend

#endif // TNT_DANTE_BACKEND_OPENGL_OPENGL_PLATFORM_GLX_H
