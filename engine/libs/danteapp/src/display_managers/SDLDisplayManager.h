
#ifndef TNT_DANTE_DANTEAPP_SDL_DISPLAY_MANAGER_H
#define TNT_DANTE_DANTEAPP_SDL_DISPLAY_MANAGER_H

#include "../DisplayManager.h"

#include <SDL.h>

#include <unordered_map>

namespace dante::app {

class SDLDisplayManager : public DisplayManager {
public:
    SDLDisplayManager();
    ~SDLDisplayManager() override;

    bool init(const Config& config) override;
    void terminate() override;

    DanteApp::Window::Handle createWindow(const char* title, uint32_t w, uint32_t h,
            bool resizable, bool headless) override;
    void destroyWindow(DanteApp::Window::Handle window) override;

    void* getNativeWindow(DanteApp::Window::Handle window) const override;

    void setWindowTitle(DanteApp::Window::Handle window, const char* title) override;
    void getWindowSize(DanteApp::Window::Handle window, uint32_t* w, uint32_t* h) const override;
    void getDrawableSize(DanteApp::Window::Handle window, uint32_t* w,
            uint32_t* h) const override;

    uint32_t getMouseState(int* x, int* y) const override;
    bool isWindowFocused(DanteApp::Window::Handle window) const override;

    void pollEvents(std::vector<AppEvent>& events) override;

    void onWindowResized(DanteApp::Window::Handle window) override;

    double getTime() const override;

    void startRendering(std::function<bool()> doFrame) override;

private:
    Config mConfig;
    mutable std::unordered_map<DanteApp::Window::Handle, void*> mNativeWindowMap;
    static AppKey mapKey(SDL_Scancode scancode);
    static uint16_t getModifiers();
};

} // namespace dante::app

#endif // TNT_DANTE_DANTEAPP_SDL_DISPLAY_MANAGER_H
