#include <filamentapp/FilamentApp.h>

#include <filament/Engine.h>
#include <filament/Scene.h>
#include <filament/View.h>

using namespace filament;

// M1 checkpoint: prove the trimmed Filament source still links and runs end to end -
// open a window via the Vulkan backend and let FilamentApp clear the frame. Skybox,
// ground plane, camera, and the FPS overlay come in M2.
int main() {
    Config config;
    config.title = "Dante";
    config.backend = Engine::Backend::VULKAN;

    FilamentApp::get().run(
        config,
        [](Engine*, View*, Scene*) {},
        [](Engine*, View*, Scene*) {});

    return 0;
}
