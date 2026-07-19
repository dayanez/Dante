
#include <danteapp/NativeWindowHelper.h>

#include <utils/Panic.h>

#include <SDL_syswm.h>

void* getNativeWindowFromSDL(SDL_Window* sdlWindow) {
    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    DANTE_CHECK_POSTCONDITION(SDL_GetWindowWMInfo(sdlWindow, &wmi))
            << "SDL version unsupported!";
    if (wmi.subsystem == SDL_SYSWM_X11) {
#if defined(DANTE_SUPPORTS_X11)
        Window win = (Window) wmi.info.x11.window;
        return (void*) win;
#endif
    } else if (wmi.subsystem == SDL_SYSWM_WAYLAND) {
#if defined(DANTE_SUPPORTS_WAYLAND)
        int width = 0;
        int height = 0;
        SDL_GetWindowSize(sdlWindow, &width, &height);

        // Static is used here to allocate the struct pointer for the lifetime of the program.
        // Without static the valid struct quickly goes out of scope, and ends with seemingly
        // random segfaults. We must update the values on each call.
        static struct {
            struct wl_display* display;
            struct wl_surface* surface;
            uint32_t width;
            uint32_t height;
        } wayland;
        wayland.display = wmi.info.wl.display;
        wayland.surface = wmi.info.wl.surface;
        wayland.width = static_cast<uint32_t>(width);
        wayland.height = static_cast<uint32_t>(height);
        return (void*) &wayland;
#endif
    }
    return nullptr;
}
