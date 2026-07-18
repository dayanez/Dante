/*
 * Copyright (C) 2026 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_FILAMENTAPP_SDL_DISPLAY_MANAGER_H
#define TNT_FILAMENT_FILAMENTAPP_SDL_DISPLAY_MANAGER_H

#include "../DisplayManager.h"

#include <SDL.h>

#include <unordered_map>

namespace filament::app {

class SDLDisplayManager : public DisplayManager {
public:
    SDLDisplayManager();
    ~SDLDisplayManager() override;

    bool init(const Config& config) override;
    void terminate() override;

    FilamentApp::Window::Handle createWindow(const char* title, uint32_t w, uint32_t h,
            bool resizable, bool headless) override;
    void destroyWindow(FilamentApp::Window::Handle window) override;

    void* getNativeWindow(FilamentApp::Window::Handle window) const override;

    void setWindowTitle(FilamentApp::Window::Handle window, const char* title) override;
    void getWindowSize(FilamentApp::Window::Handle window, uint32_t* w, uint32_t* h) const override;
    void getDrawableSize(FilamentApp::Window::Handle window, uint32_t* w,
            uint32_t* h) const override;

    uint32_t getMouseState(int* x, int* y) const override;
    bool isWindowFocused(FilamentApp::Window::Handle window) const override;

    void pollEvents(std::vector<AppEvent>& events) override;

    void onWindowResized(FilamentApp::Window::Handle window) override;

    double getTime() const override;

    void startRendering(std::function<bool()> doFrame) override;

private:
    Config mConfig;
    mutable std::unordered_map<FilamentApp::Window::Handle, void*> mNativeWindowMap;
    static AppKey mapKey(SDL_Scancode scancode);
    static uint16_t getModifiers();
};

} // namespace filament::app

#endif // TNT_FILAMENT_FILAMENTAPP_SDL_DISPLAY_MANAGER_H
