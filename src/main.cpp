#include <filamentapp/FilamentApp.h>
#include <filament/Camera.h>
#include <filament/ColorGrading.h>
#include <filament/Engine.h>
#include <filament/LightManager.h>
#include <filament/RenderableManager.h>
#include <filament/Scene.h>
#include <filament/ToneMapper.h>
#include <filament/TransformManager.h>
#include <filament/View.h>
#include <backend/BufferDescriptor.h>
#include <gltfio/Animator.h>
#include <gltfio/AssetLoader.h>
#include <gltfio/FilamentAsset.h>
#include <gltfio/FilamentInstance.h>
#include <gltfio/MaterialProvider.h>
#include <gltfio/ResourceLoader.h>
#include <gltfio/TextureProvider.h>
#include <utils/EntityManager.h>
#include <utils/Path.h>
#include <imgui.h>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <vector>


//TODO DONT FORGET TO FIX Y AXIS FOR NIBBA 
//ALSO CHANGE LIGHTING || MAKE NEG LIGHTING -100

using namespace filament;
using namespace filament::math;

namespace {

std::vector<uint8_t> readFile(utils::Path const& path) {
    std::ifstream file(path.getAbsolutePath(), std::ios::binary | std::ios::ate);
    if (!file) {
        fprintf(stderr, "[Dante] failed to open %s\n", path.getAbsolutePath().c_str());
        return {};
    }
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<uint8_t> data(size);
    file.read(reinterpret_cast<char*>(data.data()), size);
    return data;
}

} // namespace

namespace {

utils::Entity g_sun;
ColorGrading* g_colorGrading = nullptr;

// A glTF model loaded via gltfio - drives a looping animation if the source has one
// (getAnimationCount() == 0 is a no-op everywhere below), otherwise just sits static.
// Models are expected to already be glTF/GLB (e.g. straight off Sketchfab).
struct GltfModel {
    gltfio::AssetLoader* loader = nullptr;
    gltfio::MaterialProvider* materials = nullptr;
    gltfio::TextureProvider* textureDecoder = nullptr;
    gltfio::ResourceLoader* resourceLoader = nullptr;
    gltfio::FilamentAsset* asset = nullptr;
    gltfio::Animator* animator = nullptr;

    // scale is uniform and applied around the model's own origin, before translation -
    // source glTF/GLB files vary wildly in real-world unit scale (e.g. cm vs m export
    // settings), so there's no universally-correct default; check the bbox this prints
    // on load against your other scene models and adjust.
    void create(Engine& engine, Scene& scene, utils::Path const& gltfPath, float3 position,
            float scale = 1.0f) {
        materials = gltfio::createJitShaderProvider(&engine);
        loader = gltfio::AssetLoader::create({ &engine, materials });

        std::vector<uint8_t> gltfContent = readFile(gltfPath);
        asset = loader->createAsset(gltfContent.data(), (uint32_t)gltfContent.size());
        if (!asset) {
            fprintf(stderr, "[Dante] failed to parse glTF: %s\n", gltfPath.c_str());
            return;
        }

        textureDecoder = gltfio::createStbProvider(&engine);
        gltfio::ResourceConfiguration resourceConfig{};
        resourceConfig.engine = &engine;
        resourceConfig.normalizeSkinningWeights = true;
        resourceConfig.gltfPath = gltfPath.c_str();
        resourceLoader = new gltfio::ResourceLoader(resourceConfig);
        resourceLoader->addTextureProvider("image/jpeg", textureDecoder);
        resourceLoader->addTextureProvider("image/png", textureDecoder);

        utils::Path baseDir = gltfPath.getParent();
        size_t uriCount = asset->getResourceUriCount();
        const char* const* uris = asset->getResourceUris();
        for (size_t i = 0; i < uriCount; i++) {
            std::vector<uint8_t> data = readFile(baseDir + uris[i]);
            auto* buf = new uint8_t[data.size()];
            memcpy(buf, data.data(), data.size());
            resourceLoader->addResourceData(uris[i],
                    backend::BufferDescriptor(buf, data.size(),
                            [](void* b, size_t, void*) { delete[] static_cast<uint8_t*>(b); }));
        }

        resourceLoader->loadResources(asset);
        asset->releaseSourceData();

        auto& rm = engine.getRenderableManager();
        for (size_t i = 0, n = asset->getEntityCount(); i < n; i++) {
            auto instance = rm.getInstance(asset->getEntities()[i]);
            if (instance) {
                rm.setLayerMask(instance, 0x4, 0x4);
                rm.setCulling(instance, false);
            }
        }

        scene.addEntities(asset->getEntities(), asset->getEntityCount());

        auto& tm = engine.getTransformManager();
        auto root = tm.getInstance(asset->getRoot());
        tm.setTransform(root, mat4f::translation(position) * mat4f::scaling(float3{scale}));

        animator = asset->getInstance()->getAnimator();

        fprintf(stderr,
                "[Dante] loaded %s: %zu entities, %zu animations, "
                "bbox min=(%.3f,%.3f,%.3f) max=(%.3f,%.3f,%.3f)\n",
                gltfPath.c_str(), asset->getEntityCount(), animator->getAnimationCount(),
                asset->getBoundingBox().min.x, asset->getBoundingBox().min.y, asset->getBoundingBox().min.z,
                asset->getBoundingBox().max.x, asset->getBoundingBox().max.y, asset->getBoundingBox().max.z);
    }

    void destroy(Engine& engine, Scene& scene) {
        if (!asset) return;
        scene.removeEntities(asset->getEntities(), asset->getEntityCount());
        loader->destroyAsset(asset);
        materials->destroyMaterials();
        delete materials;
        delete textureDecoder;
        delete resourceLoader;
        gltfio::AssetLoader::destroy(&loader);
    }
};


//Loading models 
GltfModel g_character;
GltfModel g_bathroom;
GltfModel g_smileyMonster;

// DANTE_ASSETS_DIR is baked in at compile time as this machine's source tree path, which
// only exists on the machine that built it - convenient in dev (edit assets, relaunch, no
// rebuild) but useless for a binary handed to someone else. If an "assets" folder sits next
// to the executable instead (i.e. Dante.exe was shipped alongside one, see README), prefer
// that; otherwise fall back to the baked dev path.
utils::Path resolveAssetsDir() {
    utils::Path portable = utils::Path::getCurrentExecutable().getParent() + "assets";
    return portable.exists() ? portable : utils::Path(DANTE_ASSETS_DIR);
}

} // namespace

// M4: three glTF models (see GltfModel) under a night-sky IBL skybox, with no ground
// geometry - camera/FPS overlay.
int main() {
    utils::Path const assetsDir = resolveAssetsDir();

    Config config;
    config.title = "Dante";
#if defined(__APPLE__)
    config.backend = Engine::Backend::METAL;
#else
    config.backend = Engine::Backend::OPENGL;
#endif
    config.iblDirectory = (assetsDir + "environments/qwantani_night_puresky_4k.exr").getAbsolutePath();
    config.cameraMode = camutils::Mode::FREE_FLIGHT;

    FilamentApp::get().run(
        config,
        [assetsDir](Engine* engine, View* view, Scene* scene) {
            // FXAA (View's default) softens the whole frame, not just jagged edges - noticeable
            // on the HDRI skybox. Temporal AA smooths the character's geometry edges without
            // that softening cost.
            view->setAntiAliasing(AntiAliasing::NONE);
            TemporalAntiAliasingOptions taaOptions;
            taaOptions.enabled = true;
            view->setTemporalAntiAliasingOptions(taaOptions);

            // Subtle bloom on overbright highlights (the sky/IBL reflections) - strength is
            // deliberately low so it reads as a glow, not a haze over the whole frame.
            BloomOptions bloomOptions;
            bloomOptions.enabled = true;
            bloomOptions.strength = 0.02f;
            view->setBloomOptions(bloomOptions);

            // Screen-space ambient occlusion: self-shadows the character's own geometry
            // (limb/cloth crevices) that the single IBL alone doesn't provide.
            AmbientOcclusionOptions aoOptions;
            aoOptions.enabled = true;
            view->setAmbientOcclusionOptions(aoOptions);

            //sunlight
            view->getCamera().setExposure(16.0f, 1.0f / 125.0f, 100.0f);                                             //starts here 

                                               
            g_sun = utils::EntityManager::get().create();
            LightManager::Builder(LightManager::Type::SUN)
                    .color({1.0f, 0.985f, 0.95f})
                    .intensity(100000.0f) // lux - real midday sun, matches the sunny-16 exposure above
                    .direction(normalize(float3{-0.5f, -0.8f, -0.65f}))
                    .castShadows(true)
                    .sunAngularRadius(0.53f) // real sun's angular size in degrees                                  //ends here |need to adjust this so it isn't bright af|
                     .build(*engine, g_sun); 
            scene->addEntity(g_sun);

            ACESToneMapper toneMapper;
            g_colorGrading = ColorGrading::Builder()
                    .toneMapper(&toneMapper)
                    .build(*engine);
            view->setColorGrading(g_colorGrading);

            
            view->getCamera().setFocusDistance(3.0f);
            DepthOfFieldOptions dofOptions;
            dofOptions.enabled = false;
            dofOptions.cocScale = 2.0f / 1.6f;
            view->setDepthOfFieldOptions(dofOptions);

            //Fog
            FogOptions fogOptions;
            fogOptions.enabled = false;
            fogOptions.distance = 5.0f;
            fogOptions.density = 0.02f;
            fogOptions.fogColorFromIbl = true;
            view->setFogOptions(fogOptions);

            //Vigshit
            VignetteOptions vignetteOptions;
            vignetteOptions.enabled = false; //Disabling because right now, it looks ugly no point in having it. 
            view->setVignetteOptions(vignetteOptions);

            // Skybox and indirect light both come from config.iblDirectory - FilamentApp
            // loads and sets them on the scene before this callback runs (see
            // FilamentApp::loadIBL), so the night-sky EXR is already visible here.

            // Note: FilamentApp::doFrame() re-derives Camera::lookAt() from the camera
            // manipulator every frame, so a one-time Camera::lookAt() call here would be
            // overwritten on the next frame - the manipulator's own state is what actually
            // needs to change, which Config doesn't expose.
            g_character.create(*engine, *scene, assetsDir + "models/character/ch15_firing.glb",
                    float3{0, -1.0f, -2});
            // Bathroom's local bbox (from its own load log) is roughly x:[-4.25,7.27]
            // y:[-0.08,4.18] z:[-3.78,18.67], floor near y=-0.08 - same y/z translation as
            // the character lines its floor up with the character's feet.
            g_bathroom.create(*engine, *scene, assetsDir + "models/bathroom/the_bathroom_free.glb",
                    float3{0, -1.0f, -2});
            // Placed elsewhere in the room, away from the character. Source bbox was
            // ~99x117x127 units (see load log) - some other unit scale entirely, not meters
            // like the rest of the scene - so it's scaled down to roughly a 3.5-unit-tall
            // "big monster" to match the room. Untested exact spot; adjust once you've seen it.
            g_smileyMonster.create(*engine, *scene, assetsDir + "models/smiley_monster/smily_horror_monster.glb",
                    float3{3.0f, -1.0f, 4.0f}, 0.03f);

            // Advance every model's animation each frame (a model with no animations is a
            // no-op via the animationCount() == 0 check).
            FilamentApp::get().animate([](Engine*, View*, double now) {
                static double startTime = now;
                for (GltfModel* model : { &g_character, &g_bathroom, &g_smileyMonster }) {
                    if (!model->animator || model->animator->getAnimationCount() == 0) {
                        continue;
                    }
                    float duration = model->animator->getAnimationDuration(0);
                    float elapsed = duration > 0.0f ? fmodf((float)(now - startTime), duration) : 0.0f;
                    model->animator->applyAnimation(0, elapsed);
                    model->animator->updateBoneMatrices();
                }
            });
        },
        [](Engine* engine, View*, Scene* scene) {
            g_character.destroy(*engine, *scene);
            g_bathroom.destroy(*engine, *scene);
            g_smileyMonster.destroy(*engine, *scene);
            scene->remove(g_sun);
            engine->destroy(g_sun);
            utils::EntityManager::get().destroy(g_sun);
            engine->destroy(g_colorGrading);
        },
        [](Engine*, View*) {
            ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
            ImGui::SetNextWindowBgAlpha(0.35f);
            ImGui::Begin("##fps", nullptr,
                    ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                    ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
                    ImGuiWindowFlags_NoNav | ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::Text("%.0f FPS", ImGui::GetIO().Framerate);
            ImGui::End();
        });

    return 0;
}
