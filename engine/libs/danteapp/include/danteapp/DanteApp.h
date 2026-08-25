
#ifndef TNT_DANTE_SAMPLE_DANTEAPP_H
#define TNT_DANTE_SAMPLE_DANTEAPP_H

#include "Config.h"
#include "Cube.h"
#include "Grid.h"
#include "IBL.h"

#include <dante/Engine.h>
#include <dante/Viewport.h>

#include <camutils/Manipulator.h>

#include <utils/Entity.h>
#include <utils/Path.h>

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace dante {
class Renderer;
class Scene;
class SwapChain;
class View;
} // namespace dante

namespace dantegui {
class ImGuiHelper;
} // namespace dantegui

class IBL;
class MeshAssimp;

// For customizing the vulkan backend
namespace dante::backend {
#if defined(DANTE_DRIVER_SUPPORTS_VULKAN)
class VulkanPlatform;
#endif

#if defined(DANTE_SUPPORTS_WEBGPU)
class WebGPUPlatform;
#endif

}

namespace dante::app {
class DisplayManager;
enum class AppKey : uint32_t;
} // namespace dante::app

class DanteApp {
public:
    using SetupCallback = std::function<void(dante::Engine*, dante::View*, dante::Scene*)>;
    using CleanupCallback =
            std::function<void(dante::Engine*, dante::View*, dante::Scene*)>;
    using PreRenderCallback = std::function<void(dante::Engine*, dante::View*,
            dante::Scene*, dante::Renderer*)>;
    using PostRenderCallback = std::function<void(dante::Engine*, dante::View*,
            dante::Scene*, dante::Renderer*)>;
    using ImGuiCallback = std::function<void(dante::Engine*, dante::View*)>;
    using AnimCallback = std::function<void(dante::Engine*, dante::View*, double now)>;
    using ResizeCallback = std::function<void(dante::Engine*, dante::View*)>;
    using DropCallback = std::function<void(std::string_view)>;

    static DanteApp& get();

    ~DanteApp();

    void animate(AnimCallback animation) { mAnimation = animation; }

    void resize(ResizeCallback resize) { mResize = resize; }

    void setDropHandler(DropCallback handler) { mDropHandler = handler; }

    void run(const Config& config, SetupCallback setup, CleanupCallback cleanup,
            ImGuiCallback imgui = ImGuiCallback(),
            PreRenderCallback preRender = PreRenderCallback(),
            PostRenderCallback postRender = PostRenderCallback(), size_t width = 1024,
            size_t height = 640);

    void reconfigureCameras() { mReconfigureCameras = true; }

    dante::Material const* getDefaultMaterial() const noexcept { return mDefaultMaterial; }
    dante::Material const* getTransparentMaterial() const noexcept {
        return mTransparentMaterial;
    }
    IBL* getIBL() const noexcept { return mIBL.get(); }
    dante::Texture* getDirtTexture() const noexcept { return mDirt; }
    dante::View* getGuiView() const noexcept;
    dante::SwapChain* getPrimarySwapChain() const noexcept { return mPrimarySwapChain; }

    void close() { mClosed = true; }

    void setSidebarWidth(int width) {
        mCameraParams.sidebarWidth = width;
        mReconfigureCameras = true;
    }

    void setWindowTitle(const char* title) { mWindowTitle = title; }

    void setCameraFocalLength(float focalLength) {
        mCameraParams.focalLength = focalLength;
        mReconfigureCameras = true;
    }

    void setCameraNearFar(float near, float far) {
        mCameraParams.near = near;
        mCameraParams.far = far;
        mReconfigureCameras = true;
    }

    void addOffscreenView(dante::View* view) { mOffscreenViews.push_back(view); }

    size_t getSkippedFrameCount() const { return mSkippedFrames; }

    void loadIBL(std::string_view path);

    // debugging: enable/disable the froxel grid
    void setCameraFrustumEnabled(bool enabled) noexcept;
    void setDirectionalShadowFrustumEnabled(bool enabled) noexcept;
    void setFroxelGridEnabled(bool enabled) noexcept;
    bool isCameraFrustumEnabled() const noexcept;
    bool isDirectionalShadowFrustumEnabled() const noexcept;
    bool isFroxelGridEnabled() const noexcept;

    DanteApp(const DanteApp& rhs) = delete;
    DanteApp(DanteApp&& rhs) = delete;
    DanteApp& operator=(const DanteApp& rhs) = delete;
    DanteApp& operator=(DanteApp&& rhs) = delete;

    /**
     * Returns the path to the Dante root for loading assets. This is determined from the
     * executable folder, which allows users to launch samples from any folder.
     *
     * This takes into account multi-configuration CMake generators, like Visual Studio or Xcode,
     * that have different executable paths compared to single-configuration generators, like Ninja.
     */
    static const utils::Path& getRootAssetsPath();

private:
    DanteApp();

    using CameraManipulator = dante::camutils::Manipulator<float>;

    static bool manipulatorKeyFromKeycode(dante::app::AppKey scancode,
            CameraManipulator::Key& key);

    class CView {
    public:
        CView(dante::Renderer& renderer, std::string name);
        virtual ~CView();

        void setCameraManipulator(CameraManipulator* cm);
        void setViewport(dante::Viewport const& viewport);
        void setCamera(dante::Camera* camera);
        bool intersects(ssize_t x, ssize_t y);

        virtual void mouseDown(int button, ssize_t x, ssize_t y);
        virtual void mouseUp(ssize_t x, ssize_t y);
        virtual void mouseMoved(ssize_t x, ssize_t y);
        virtual void mouseWheel(ssize_t x);
        virtual void keyDown(dante::app::AppKey scancode);
        virtual void keyUp(dante::app::AppKey scancode);

        dante::View const* getView() const { return view; }
        dante::View* getView() { return view; }
        CameraManipulator* getCameraManipulator() { return mCameraManipulator; }

    private:
        enum class Mode : uint8_t {
            NONE, ROTATE, TRACK
        };

        dante::Engine& engine;
        dante::Viewport mViewport;
        dante::View* view = nullptr;
        CameraManipulator* mCameraManipulator = nullptr;
        std::string mName;
    };

    class GodView : public CView {
    public:
        using CView::CView;
        void setGodCamera(dante::Camera* camera);
    };

    struct WindowCameraParams {
        int sidebarWidth = 0;
        float focalLength = 28.0f;
        float near = 0.1f;
        float far = 100.0f;

        bool operator==(WindowCameraParams const& params) const noexcept {
            return sidebarWidth == params.sidebarWidth && focalLength == params.focalLength &&
                   near == params.near && far == params.far;
        }

        bool operator!=(WindowCameraParams const& params) const noexcept {
            return !(*this == params);
        }
    };

public:
    class Window {
        friend class DanteApp;
    public:
        using Handle = void*;
        virtual ~Window();

    private:
        Window(DanteApp* danteApp, const Config& config, std::string title,
                WindowCameraParams const& cameraParams, size_t w, size_t h);

        void mouseDown(int button, ssize_t x, ssize_t y);
        void mouseUp(ssize_t x, ssize_t y);
        void mouseMoved(ssize_t x, ssize_t y);
        void mouseWheel(ssize_t x);
        void keyDown(dante::app::AppKey scancode);
        void keyUp(dante::app::AppKey scancode);
        void resize(WindowCameraParams const& cameraParams);

        dante::Renderer* getRenderer() { return mRenderer; }
        dante::SwapChain* getSwapChain() { return mSwapChain; }

        void configureCamerasForWindow(WindowCameraParams const& camera);
        void fixupMouseCoordinatesForHdpi(ssize_t& x, ssize_t& y) const;

        dante::app::DisplayManager* const mDisplayManager = nullptr;
        dante::Engine* const mEngine = nullptr;
        Config mConfig;
        const bool mIsHeadless;

        Handle mWindow = nullptr;
        dante::Renderer* mRenderer = nullptr;
        dante::Engine::Backend mBackend;

        CameraManipulator* mMainCameraMan;
        CameraManipulator* mDebugCameraMan;
        dante::SwapChain* mSwapChain = nullptr;

        utils::Entity mCameraEntities[3];
        dante::Camera* mCameras[3] = { nullptr };
        dante::Camera* mMainCamera;
        dante::Camera* mDebugCamera;
        dante::Camera* mOrthoCamera;

        std::vector<std::unique_ptr<CView>> mViews;
        CView* mMainView;   // well, the main view
        CView* mUiView;     // the imgui ui
        CView* mDepthView;
        GodView* mGodView;  // the debug view with "god" camera
        CView* mOrthoView;  // directional shadow map view

        size_t mWidth = 0;
        size_t mHeight = 0;
        ssize_t mLastX = 0;
        ssize_t mLastY = 0;

        CView* mMouseEventTarget = nullptr;

        // Keep track of which view should receive a key's keyUp event.
        std::unordered_map<dante::app::AppKey, CView*> mKeyEventTarget;
    };

private:
    friend class Window;

    void loadIBL(const Config& config);
    void loadDirt(const Config& config);

    bool doFrame();
    void shutdown();

    dante::Engine* mEngine = nullptr;
    dante::Scene* mScene = nullptr;
    std::unique_ptr<IBL> mIBL;
    dante::SwapChain* mPrimarySwapChain = nullptr;
    dante::Texture* mDirt = nullptr;
    bool mClosed = false;
    double mTime = 0;

    dante::Material const* mDefaultMaterial = nullptr;
    dante::Material const* mTransparentMaterial = nullptr;
    dante::Material const* mDepthMaterial = nullptr;
    dante::MaterialInstance* mDepthMI = nullptr;
    std::unique_ptr<dantegui::ImGuiHelper> mImGuiHelper;
    AnimCallback mAnimation;
    ResizeCallback mResize;
    DropCallback mDropHandler;
    size_t mSkippedFrames = 0;
    std::string mWindowTitle;
    std::vector<dante::View*> mOffscreenViews;
    WindowCameraParams mCameraParams{};
    bool mReconfigureCameras = false;
    uint8_t mFroxelInfoAge = 0x42;
    uint8_t mFroxelGridEnabled = 0;
    uint8_t mDirectionalShadowFrustumEnabled = 0x2;
    uint8_t mCameraFrustumEnabled = 0x2;

    dante::app::DisplayManager* mDisplayManager = nullptr;

    dante::backend::Platform* mVulkanPlatform = nullptr;
    dante::backend::Platform* mWebGPUPlatform = nullptr;

    std::unique_ptr<Window> mWindow;
    CleanupCallback mCleanupCallback;
    ImGuiCallback mImguiCallback {};
    PreRenderCallback mPreRender {};
    PostRenderCallback mPostRender {};
    bool mMousePressed[3] = { false };
    bool mIsSplitView = false;

    std::unique_ptr<Cube> mCameraCube;
    std::unique_ptr<Grid> mCameraGrid;

    // we can't cull the light-frustum because it's not applied a rigid transform
    // and currently, dante assumes that for culling
    std::vector<Cube> mLightmapCubes;
};

#endif // TNT_DANTE_SAMPLE_DANTEAPP_H
