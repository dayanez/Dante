/*
 * Copyright (C) 2017 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TNT_FILAMENT_NATIVE_WINDOW_HELPER_H
#define TNT_FILAMENT_NATIVE_WINDOW_HELPER_H

struct SDL_Window;

extern "C" void* getNativeWindowFromSDL(SDL_Window* sdlWindow);

#if defined(__APPLE__)
// Add a backing CAMetalLayer to the NSView and return the layer.
extern "C" void* setUpMetalLayer(void* nativeWindow);
// Setup the window the way Filament expects (color space, etc.).
extern "C" void prepareNativeWindow(SDL_Window* sdlWindow);
// Resize the backing CAMetalLayer's drawable to match the new view's size. Returns the layer.
extern "C" void* resizeMetalLayer(void* nativeView);
#endif

#endif // TNT_FILAMENT_NATIVE_WINDOW_HELPER_H
