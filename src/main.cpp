 #include <danteapp/DanteApp.h>
#include <dante/Box.h>
#include <dante/Camera.h>
#include <dante/ColorGrading.h>
#include <dante/Engine.h>
#include <dante/IndirectLight.h>
#include <dante/LightManager.h>
#include <dante/RenderableManager.h>
#include <dante/Scene.h>
#include <dante/ToneMapper.h>
#include <dante/TransformManager.h>
#include <dante/View.h>
#include <dante/Viewport.h>
#include <backend/BufferDescriptor.h>
#include <danteapp/Cube.h>
#include <gltfio/Animator.h>
#include <gltfio/AssetLoader.h>
#include <gltfio/DanteAsset.h>
#include <gltfio/DanteInstance.h>
#include <gltfio/MaterialProvider.h>
#include <gltfio/ResourceLoader.h>
#include <gltfio/TextureProvider.h>
#include <utils/EntityManager.h>
#include <utils/Path.h>
#include <imgui.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <conio.h>
#define NOMINMAX
#include <windows.h>

using namespace dante;
using namespace dante::math;
using namespace std;
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
//
// position/rotationDeg/scale are the live, editable TRS - the Scene panel and the
// viewport click-drag/scroll handling both just mutate these fields and call
// applyTransform(), so this struct is the single source of truth for where a model sits.
struct GltfModel {
    gltfio::AssetLoader* loader = nullptr;
    gltfio::MaterialProvider* materials = nullptr;
    gltfio::TextureProvider* textureDecoder = nullptr;
    gltfio::ResourceLoader* resourceLoader = nullptr;
    gltfio::DanteAsset* asset = nullptr;
    gltfio::Animator* animator = nullptr;

    std::string name;
    utils::Path sourcePath;
    float3 position{0, 0, 0};
    float3 rotationDeg{0, 0, 0};
    float scale = 1.0f;

    // scale is uniform and applied around the model's own origin, before translation -
    // source glTF/GLB files vary wildly in real-world unit scale (e.g. cm vs m export
    // settings), so there's no universally-correct default; check the bbox this prints
    // on load against your other scene models and adjust.
    void create(Engine& engine, Scene& scene, utils::Path const& gltfPath, float3 initialPosition,
            float initialScale = 1.0f) {
        name = gltfPath.getNameWithoutExtension();
        sourcePath = gltfPath;
        position = initialPosition;
        scale = initialScale;

        materials = gltfio::createJitShaderProvider(&engine);
        loader = gltfio::AssetLoader::create({ &engine, materials });

        std::vector<uint8_t> gltfContent = readFile(gltfPath);
        asset = loader->createAsset(gltfContent.data(), (uint32_t)gltfContent.size());
        if (!asset) {
            fprintf(stderr, "[Dante] failed to parse glTF: %s\n", gltfPath.c_str());
            // materials/loader are already allocated at this point - clean them up rather
            // than leaking them on a bad path (this is reachable from the panel now, not
            // just hardcoded startup paths).
            materials->destroyMaterials();
            delete materials;
            materials = nullptr;
            gltfio::AssetLoader::destroy(&loader);
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

        applyTransform(engine);

        animator = asset->getInstance()->getAnimator();

        fprintf(stderr,
                "[Dante] loaded %s: %zu entities, %zu animations, "
                "bbox min=(%.3f,%.3f,%.3f) max=(%.3f,%.3f,%.3f)\n",
                gltfPath.c_str(), asset->getEntityCount(), animator->getAnimationCount(),
                asset->getBoundingBox().min.x, asset->getBoundingBox().min.y, asset->getBoundingBox().min.z,
                asset->getBoundingBox().max.x, asset->getBoundingBox().max.y, asset->getBoundingBox().max.z);
    }

    void applyTransform(Engine& engine) {
        if (!asset) return;
        auto& tm = engine.getTransformManager();
        auto root = tm.getInstance(asset->getRoot());
        constexpr float kDegToRad = 0.017453292519943295f;
        mat4f const rotation = mat4f::eulerZYX(
                rotationDeg.y * kDegToRad,
                rotationDeg.x * kDegToRad,
                rotationDeg.z * kDegToRad);
        tm.setTransform(root, mat4f::translation(position) * rotation * mat4f::scaling(float3{scale}));
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
        // Null everything out - unloadModel() only ever calls this once per model today,
        // but leaving stale non-null pointers after a delete is a double-free waiting to
        // happen the moment that assumption changes.
        asset = nullptr;
        materials = nullptr;
        textureDecoder = nullptr;
        resourceLoader = nullptr;
        loader = nullptr;
        animator = nullptr;
    }
};

// A practical point light (bulb, lamp, etc). Kept alongside its own last-set values so the
// Lights panel can show/edit them - LightManager itself is set-only per parameter, no getters.
struct PointLight {
    utils::Entity entity;
    std::string name;
    float3 position{0, 1.5f, 0};
    float3 color{1.0f, 0.85f, 0.6f};
    float intensity = 4000.0f; // lumens
    float falloff = 3.0f;      // meters
};

// All currently-loaded models, and a lookup from any of their glTF sub-entities back to
// the owning GltfModel - a model can be dozens to hundreds of renderable entities, but
// picking/selection always operates on the whole model as one unit.
std::vector<std::unique_ptr<GltfModel>> g_models;
std::unordered_map<uint32_t, GltfModel*> g_entityToModel;
GltfModel* g_selectedModel = nullptr;
Scene* g_scene = nullptr;

// The setup callback's View* is the real 3D scene view. The ImGui callback's View*
// parameter is NOT the same view - DanteApp constructs its ImGuiHelper against a separate,
// dedicated UI-compositing view (window->mUiView, see DanteApp.cpp) that has its own empty
// scene, its own camera, and post-processing explicitly disabled. Every setXOptions/pick/
// getCamera call made against the ImGui callback's own view parameter is a silent no-op on
// the real scene - g_mainView, captured here during setup, is the one to use instead.
View* g_mainView = nullptr;

std::vector<std::unique_ptr<PointLight>> g_lights;
PointLight* g_selectedLight = nullptr;

// Edit-mode state: while active, the free-fly camera's mouse look is suppressed (see the
// WantCaptureMouse override at the bottom of the ImGui callback) and left-click instead
// drives picking/dragging.
bool g_editMode = false;
bool g_dragging = false;
float g_dragDistance = 1.0f;
utils::Entity g_pendingPickEntity;
bool g_pendingPickValid = false;

// Post-processing state, mirrored here because View's setters are write-only (no getters) -
// these are the single source of truth the Post Processing panel reads/writes and reapplies
// every frame.
BloomOptions g_bloomOptions;
AmbientOcclusionOptions g_aoOptions;
VignetteOptions g_vignetteOptions;
FogOptions g_fogOptions;
DepthOfFieldOptions g_dofOptions;
TemporalAntiAliasingOptions g_taaOptions;
float g_iblIntensity = 4000.0f;
std::string g_currentSkyboxPath;

// Camera exposure (aperture f-stop, shutter seconds, ISO). Photographic exposure is
// extremely non-linear (each doubling of any one of these roughly doubles/halves overall
// brightness) - these are a rough starting guess for the dim/interior scene, not a
// verified-correct value, so they're exposed live below rather than only hardcoded.
float g_aperture = 11.0f;
float g_shutterSpeed = 1.0f / 60.0f;
float g_sensitivity = 100.0f;

std::unique_ptr<Cube> g_selectionCube;

GltfModel* loadModelIntoScene(Engine& engine, Scene& scene, utils::Path const& path,
        float3 position, float scale = 1.0f) {
    auto model = std::make_unique<GltfModel>();
    model->create(engine, scene, path, position, scale);
    if (!model->asset) {
        return nullptr;
    }
    GltfModel* ptr = model.get();
    for (size_t i = 0, n = ptr->asset->getEntityCount(); i < n; i++) {
        g_entityToModel[ptr->asset->getEntities()[i].getId()] = ptr;
    }
    g_models.push_back(std::move(model));
    return ptr;
}

void unloadModel(Engine& engine, Scene& scene, GltfModel* target) {
    if (!target) return;
    if (target->asset) {
        for (size_t i = 0, n = target->asset->getEntityCount(); i < n; i++) {
            g_entityToModel.erase(target->asset->getEntities()[i].getId());
        }
    }
    target->destroy(engine, scene);
    if (g_selectedModel == target) {
        g_selectedModel = nullptr;
    }
    g_models.erase(std::remove_if(g_models.begin(), g_models.end(),
            [target](std::unique_ptr<GltfModel> const& m) { return m.get() == target; }),
            g_models.end());
}

GltfModel* reloadModel(Engine& engine, Scene& scene, GltfModel* target) {
    if (!target) return nullptr;
    utils::Path const path = target->sourcePath;
    float3 const pos = target->position;
    float3 const rot = target->rotationDeg;
    float const scl = target->scale;
    unloadModel(engine, scene, target); // invalidates target
    GltfModel* fresh = loadModelIntoScene(engine, scene, path, pos, scl);
    if (fresh) {
        fresh->rotationDeg = rot;
        fresh->applyTransform(engine);
        g_selectedModel = fresh;
    }
    return fresh;
}

GltfModel* duplicateModel(Engine& engine, Scene& scene, GltfModel* source) {
    if (!source) return nullptr;
    GltfModel* copy = loadModelIntoScene(engine, scene, source->sourcePath,
            source->position + float3{0.5f, 0.0f, 0.5f}, source->scale);
    if (copy) {
        copy->rotationDeg = source->rotationDeg;
        copy->applyTransform(engine);
    }
    return copy;
}

PointLight* addLight(Engine& engine, Scene& scene, std::string const& name, float3 position,
        float3 color = {1.0f, 0.85f, 0.6f}, float intensity = 4000.0f, float falloff = 3.0f) {
    auto light = std::make_unique<PointLight>();
    light->entity = utils::EntityManager::get().create();
    light->name = name;
    light->position = position;
    light->color = color;
    light->intensity = intensity;
    light->falloff = falloff;
    LightManager::Builder(LightManager::Type::POINT)
            .position(position)
            .color(color)
            .intensity(intensity)
            .falloff(falloff)
            .castShadows(true)
            .build(engine, light->entity);
    scene.addEntity(light->entity);
    PointLight* ptr = light.get();
    g_lights.push_back(std::move(light));
    return ptr;
}

void applyLight(Engine& engine, PointLight* light) {
    if (!light) return;
    auto& lm = engine.getLightManager();
    auto instance = lm.getInstance(light->entity);
    if (!instance) return;
    lm.setPosition(instance, light->position);
    lm.setColor(instance, light->color);
    lm.setIntensity(instance, light->intensity);
    lm.setFalloff(instance, light->falloff);
}

void removeLight(Engine& engine, Scene& scene, PointLight* target) {
    if (!target) return;
    scene.remove(target->entity);
    engine.destroy(target->entity);
    utils::EntityManager::get().destroy(target->entity);
    if (g_selectedLight == target) {
        g_selectedLight = nullptr;
    }
    g_lights.erase(std::remove_if(g_lights.begin(), g_lights.end(),
            [target](std::unique_ptr<PointLight> const& l) { return l.get() == target; }),
            g_lights.end());
}

void applyIblIntensity() {
    IBL* ibl = DanteApp::get().getIBL();
    if (ibl && ibl->getIndirectLight()) {
        ibl->getIndirectLight()->setIntensity(g_iblIntensity);
    }
}

void clearScene(Engine& engine, Scene& scene) {
    while (!g_models.empty()) {
        unloadModel(engine, scene, g_models.back().get());
    }
    while (!g_lights.empty()) {
        removeLight(engine, scene, g_lights.back().get());
    }
}

// Recursively finds files by extension (no dot, e.g. "glb") under dir - backs the Scene
// panel's asset lists so dropping a new .glb/.exr into assets/ is enough to use it, no
// code changes or recompiling required.
std::vector<utils::Path> findFilesByExtension(utils::Path const& dir, char const* ext) {
    std::vector<utils::Path> result;
    if (!dir.exists() || !dir.isDirectory()) {
        return result;
    }
    for (utils::Path const& entry : dir.listContents()) {
        if (entry.isDirectory()) {
            std::vector<utils::Path> nested = findFilesByExtension(entry, ext);
            result.insert(result.end(), nested.begin(), nested.end());
        } else if (entry.getExtension() == ext) {
            result.push_back(entry);
        }
    }
    return result;
}

utils::Path resolveAssetsDir() {
    utils::Path portable = utils::Path::getCurrentExecutable().getParent() + "assets";
    return portable.exists() ? portable : utils::Path(DANTE_ASSETS_DIR);
}

utils::Path sceneFilePath(utils::Path const& assetsDir) {
    return assetsDir + "scene.txt";
}

// Plain whitespace-tokenized save format - simple to read/write without a JSON dependency.
// Known limitation: asset paths and light names can't contain spaces (this project's own
// paths don't), and paths are stored absolute, so a saved scene is tied to this machine.
void saveScene(utils::Path const& file) {
    std::ofstream out(file.getPath());
    if (!out) {
        fprintf(stderr, "[Dante] failed to write scene file %s\n", file.c_str());
        return;
    }
    out << "skybox " << g_currentSkyboxPath << "\n";
    out << "iblintensity " << g_iblIntensity << "\n";
    for (auto const& m : g_models) {
        out << "model " << m->sourcePath.getAbsolutePath().c_str() << " "
            << m->position.x << " " << m->position.y << " " << m->position.z << " "
            << m->rotationDeg.x << " " << m->rotationDeg.y << " " << m->rotationDeg.z << " "
            << m->scale << "\n";
    }
    for (auto const& l : g_lights) {
        std::string safeName = l->name;
        std::replace(safeName.begin(), safeName.end(), ' ', '_');
        out << "light " << safeName << " "
            << l->position.x << " " << l->position.y << " " << l->position.z << " "
            << l->color.x << " " << l->color.y << " " << l->color.z << " "
            << l->intensity << " " << l->falloff << "\n";
    }
    fprintf(stderr, "[Dante] saved scene to %s\n", file.c_str());
}

bool loadScene(Engine& engine, Scene& scene, utils::Path const& file) {
    std::ifstream in(file.getPath());
    if (!in) {
        return false;
    }
    clearScene(engine, scene);
    std::string tag;
    while (in >> tag) {
        if (tag == "skybox") {
            std::string path;
            in >> path;
            g_currentSkyboxPath = path;
            DanteApp::get().loadIBL(path);
            applyIblIntensity();
        } else if (tag == "iblintensity") {
            in >> g_iblIntensity;
            applyIblIntensity();
        } else if (tag == "model") {
            std::string path;
            float3 pos, rot;
            float scl;
            in >> path >> pos.x >> pos.y >> pos.z >> rot.x >> rot.y >> rot.z >> scl;
            GltfModel* m = loadModelIntoScene(engine, scene, utils::Path(path), pos, scl);
            if (m) {
                m->rotationDeg = rot;
                m->applyTransform(engine);
            }
        } else if (tag == "light") {
            std::string name;
            float3 pos, color;
            float intensity, falloff;
            in >> name >> pos.x >> pos.y >> pos.z >> color.x >> color.y >> color.z >> intensity >> falloff;
            addLight(engine, scene, name, pos, color, intensity, falloff);
        }
    }
    fprintf(stderr, "[Dante] loaded scene from %s\n", file.c_str());
    return true;
}

} // namespace

// M4: a data-driven scene (see GltfModel / g_models, PointLight / g_lights) under a
// swappable IBL skybox, with an in-engine Scene panel for loading models/skyboxes, editing
// lighting and post-processing live, and picking/dragging models around with the mouse.
int main() {
    utils::Path const assetsDir = resolveAssetsDir();

    Config config;
    config.title = "DanteEngine";
#if defined(__APPLE__)
    config.backend = Engine::Backend::METAL;
#else
    config.backend = Engine::Backend::OPENGL;
#endif
    config.iblDirectory = (assetsDir + "environments/qwantani_night_puresky_4k.exr").getAbsolutePath();
    config.cameraMode = camutils::Mode::FREE_FLIGHT;
    config.cameraMoveSpeed = 24.0f; // camutils' own default is 10
    g_currentSkyboxPath = config.iblDirectory;

    DanteApp::get().run(
        config,
        [assetsDir](Engine* engine, View* view, Scene* scene) {
            g_scene = scene;
            g_mainView = view;

            view->setAntiAliasing(AntiAliasing::NONE);
            g_taaOptions.enabled = false;
            view->setTemporalAntiAliasingOptions(g_taaOptions);

            g_bloomOptions.enabled = true;
            g_bloomOptions.strength = 0.04f;
            view->setBloomOptions(g_bloomOptions);

            // GTAO (not the cheaper SAO default) - contact shadows in corners are what make
            // darkness read as "something could be hiding there" instead of flat black.
            g_aoOptions.enabled = true;
            g_aoOptions.aoType = AmbientOcclusionOptions::AmbientOcclusionType::GTAO;
            g_aoOptions.radius = 0.5f;
            g_aoOptions.intensity = 1.0f;
            view->setAmbientOcclusionOptions(g_aoOptions);

            // Rough starting exposure for the dimmer interior scene - replaces the old
            // sunny-16 settings that were tuned for a 100,000 lux midday sun. Tune live via
            // the Post Processing panel's Camera Exposure sliders; this is a guess, not a
            // verified-correct value.
            view->getCamera().setExposure(g_aperture, g_shutterSpeed, g_sensitivity);

            // The sun is now cold, weak moonlight leaking through a window rather than
            // midday sun - the room's actual lighting comes from the practical point lights
            // below instead.
            g_sun = utils::EntityManager::get().create();
            LightManager::Builder(LightManager::Type::SUN)
                    .color({0.65f, 0.75f, 1.0f})
                    .intensity(600.0f) // lux
                    .direction(normalize(float3{-0.5f, -0.8f, -0.65f}))
                    .castShadows(true)
                    .sunAngularRadius(0.53f) // real moon's angular size is close to this too
                    .build(*engine, g_sun);
            scene->addEntity(g_sun);

            ACESToneMapper toneMapper;
            g_colorGrading = ColorGrading::Builder()
                    .toneMapper(&toneMapper)
                    .shadowsMidtonesHighlights(
                            {0.75f, 0.85f, 0.82f, 1.0f},  // shadows: cool, slightly desaturated green
                            {1.0f, 1.0f, 1.0f, 0.0f},     // midtones: neutral (weight 0 = no effect)
                            {1.15f, 0.95f, 0.75f, 1.0f},  // highlights: warm, sickly amber
                            {0.0f, 0.3f, 0.7f, 1.0f})
                    .build(*engine);
            view->setColorGrading(g_colorGrading);

            view->getCamera().setFocusDistance(3.0f);
            g_dofOptions.enabled = false;
            g_dofOptions.cocScale = 2.0f / 1.6f;
            view->setDepthOfFieldOptions(g_dofOptions);

            // Subtle near-field haze around light sources rather than a heavy blanket.
            g_fogOptions.enabled = true;
            g_fogOptions.distance = 2.0f;
            g_fogOptions.density = 0.03f;
            g_fogOptions.height = 0.0f;
            g_fogOptions.heightFalloff = 1.5f;
            g_fogOptions.fogColorFromIbl = true;
            view->setFogOptions(g_fogOptions);

            g_vignetteOptions.enabled = true;
            g_vignetteOptions.midPoint = 0.5f;
            g_vignetteOptions.roundness = 0.6f;
            g_vignetteOptions.feather = 0.6f;
            view->setVignetteOptions(g_vignetteOptions);

            // Skybox and indirect light both come from config.iblDirectory - DanteApp loads
            // and sets them on the scene before this callback runs, at its own hardcoded
            // intensity, so it's re-applied here to the dimmer interior-appropriate value.
            applyIblIntensity();

            utils::Path const savedScene = sceneFilePath(assetsDir);
            if (savedScene.exists()) {
                loadScene(*engine, *scene, savedScene);
            } else {
                // Note: DanteApp::doFrame() re-derives Camera::lookAt() from the camera
                // manipulator every frame, so a one-time Camera::lookAt() call here would be
                // overwritten on the next frame - the manipulator's own state is what
                // actually needs to change, which Config doesn't expose.
                loadModelIntoScene(*engine, *scene, assetsDir + "models/character/ch15_firing.glb",
                        float3{0.2f, -1.0f, 1.7f});
                // Bathroom's local bbox (from its own load log) is roughly x:[-4.25,7.27]
                // y:[-0.08,4.18] z:[-3.78,18.67], floor near y=-0.08 - same y/z translation
                // as the character lines its floor up with the character's feet.
                loadModelIntoScene(*engine, *scene, assetsDir + "models/bathroom/the_bathroom_free.glb",
                        float3{0, -1.0f, -2});
                // Placed elsewhere in the room, away from the character. Source bbox was
                // ~99x117x127 units (see load log) - some other unit scale entirely, not
                // meters like the rest of the scene - so it's scaled down to roughly a
                // 3.5-unit-tall "big monster" to match the room.
                loadModelIntoScene(*engine, *scene, assetsDir + "models/smiley_monster/smily_horror_monster.glb",
                        float3{3.0f, -1.0f, 4.0f}, 0.03f);

                // Practical lights: small falloff so each is an isolated pool of light
                // rather than filling the room - the actual RE7-style look mostly comes
                // from this, not code. Starting points; tune live via the Lights panel.
                addLight(*engine, *scene, "Bathroom Bulb", float3{0.5f, 2.0f, -1.5f},
                        float3{1.0f, 0.85f, 0.6f}, 4000.0f, 3.0f);
                addLight(*engine, *scene, "Hall Lamp", float3{2.0f, 1.6f, 2.5f},
                        float3{1.0f, 0.7f, 0.45f}, 3000.0f, 2.5f);
            }

            // Advance every model's animation each frame (a model with no animations is a
            // no-op via the animationCount() == 0 check).
            DanteApp::get().animate([](Engine*, View*, double now) {
                static double startTime = now;
                for (auto& model : g_models) {
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
            clearScene(*engine, *scene);
            scene->remove(g_sun);
            engine->destroy(g_sun);
            utils::EntityManager::get().destroy(g_sun);
            engine->destroy(g_colorGrading);
            g_selectionCube.reset();
        },
        [assetsDir](Engine* engine, View* uiView) {
            // DanteApp hands this callback its dedicated UI-compositing view, not the real
            // scene view (see the g_mainView comment above) - every line below that touches
            // the 3D scene needs the real one.
            View* view = g_mainView ? g_mainView : uiView;
            ImGui::SetNextWindowPos(ImVec2(10,10), ImGuiCond_Always);
            ImGui::SetNextWindowBgAlpha(0.35f);
            ImGui::Begin("##fps", nullptr,
                    ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                    ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
                    ImGuiWindowFlags_NoNav | ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::Text("%.0f FPS", ImGui::GetIO().Framerate);
            ImGui::End();

            // -------- Scene panel: load models/skyboxes, edit lights/models/post-fx --------
            static std::vector<utils::Path> glbFiles;
            static std::vector<utils::Path> exrFiles;
            static bool scanned = false;
            if (!scanned) {
                // Both packaged (.glb) and unpacked (.gltf + .bin + loose textures) glTF
                // layouts are common export formats (e.g. Sketchfab offers either) -
                // AssetLoader::createAsset() handles both transparently, so both need to be
                // findable here too.
                glbFiles = findFilesByExtension(assetsDir + "models", "glb");
                std::vector<utils::Path> const gltfFiles = findFilesByExtension(assetsDir + "models", "gltf");
                glbFiles.insert(glbFiles.end(), gltfFiles.begin(), gltfFiles.end());
                exrFiles = findFilesByExtension(assetsDir + "environments", "exr");
                scanned = true;
            }

            if (ImGui::IsKeyPressed(ImGuiKey_E, false)) {
                g_editMode = !g_editMode;
            }

            ImGui::SetNextWindowPos(ImVec2(10, 40), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(360, 0), ImGuiCond_FirstUseEver);
            ImGui::Begin("Scene");
            ImGui::Checkbox("Edit Mode (E)", &g_editMode);
            if (g_editMode) {
                ImGui::TextWrapped("Click a model to select it, drag to move it, scroll to "
                        "push/pull. Camera mouse-look is disabled while editing.");
            }

            if (ImGui::Button("Rescan assets/")) {
                scanned = false;
            }
            ImGui::Text("Found %zu model file(s), %zu environment(s) under assets/",
                    glbFiles.size(), exrFiles.size());
            ImGui::SameLine();
            if (ImGui::Button("Save Scene")) {
                saveScene(sceneFilePath(assetsDir));
            }
            ImGui::SameLine();
            if (ImGui::Button("Load Scene") && g_scene) {
                loadScene(*engine, *g_scene, sceneFilePath(assetsDir));
            }

            if (ImGui::CollapsingHeader("Environments")) {
                for (utils::Path const& path : exrFiles) {
                    ImGui::PushID(path.c_str());
                    if (ImGui::Button(path.getNameWithoutExtension().c_str())) {
                        DanteApp::get().loadIBL(path.getAbsolutePath().c_str());
                        g_currentSkyboxPath = path.getAbsolutePath().c_str();
                        applyIblIntensity();
                    }
                    ImGui::PopID();
                }
            }

            if (ImGui::CollapsingHeader("Add model", ImGuiTreeNodeFlags_DefaultOpen) && g_scene) {
                for (utils::Path const& path : glbFiles) {
                    ImGui::PushID(path.c_str());
                    if (ImGui::Button(path.getNameWithoutExtension().c_str())) {
                        float3 const camPos = float3(view->getCamera().getPosition());
                        float3 const fwd = view->getCamera().getForwardVector();
                        g_selectedModel = loadModelIntoScene(*engine, *g_scene, path, camPos + fwd * 3.0f);
                    }
                    ImGui::PopID();
                }
            }

            if (ImGui::CollapsingHeader("Loaded models", ImGuiTreeNodeFlags_DefaultOpen)) {
                GltfModel* toRemove = nullptr;
                GltfModel* toReload = nullptr;
                GltfModel* toDuplicate = nullptr;
                for (auto& model : g_models) {
                    ImGui::PushID(model.get());
                    bool const isSelected = g_selectedModel == model.get();
                    if (ImGui::Selectable(model->name.c_str(), isSelected)) {
                        g_selectedModel = model.get();
                    }
                    if (isSelected) {
                        bool changed = false;
                        changed |= ImGui::DragFloat3("Position", &model->position.x, 0.02f);
                        changed |= ImGui::DragFloat3("Rotation", &model->rotationDeg.x, 0.5f);
                        changed |= ImGui::DragFloat("Scale", &model->scale, 0.01f, 0.001f, 100.0f);
                        if (changed) {
                            model->applyTransform(*engine);
                        }
                        if (ImGui::Button("Remove") && g_scene) {
                            toRemove = model.get();
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Reload") && g_scene) {
                            toReload = model.get();
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Duplicate") && g_scene) {
                            toDuplicate = model.get();
                        }
                    }
                    ImGui::PopID();
                }
                if (toRemove && g_scene) {
                    unloadModel(*engine, *g_scene, toRemove);
                }
                if (toReload && g_scene) {
                    reloadModel(*engine, *g_scene, toReload);
                }
                if (toDuplicate && g_scene) {
                    g_selectedModel = duplicateModel(*engine, *g_scene, toDuplicate);
                }
            }

            if (ImGui::CollapsingHeader("Lights", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ImGui::Button("Add Light Here") && g_scene) {
                    float3 const camPos = float3(view->getCamera().getPosition());
                    float3 const fwd = view->getCamera().getForwardVector();
                    g_selectedLight = addLight(*engine, *g_scene, "New Light", camPos + fwd * 2.0f);
                }
                PointLight* toRemoveLight = nullptr;
                for (auto& light : g_lights) {
                    ImGui::PushID(light.get());
                    bool const isSelected = g_selectedLight == light.get();
                    if (ImGui::Selectable(light->name.c_str(), isSelected)) {
                        g_selectedLight = light.get();
                    }
                    if (isSelected) {
                        bool changed = false;
                        changed |= ImGui::DragFloat3("Position##light", &light->position.x, 0.02f);
                        changed |= ImGui::ColorEdit3("Color##light", &light->color.x);
                        changed |= ImGui::DragFloat("Intensity (lm)##light", &light->intensity, 25.0f, 0.0f, 200000.0f);
                        changed |= ImGui::DragFloat("Falloff (m)##light", &light->falloff, 0.05f, 0.1f, 50.0f);
                        if (changed) {
                            applyLight(*engine, light.get());
                        }
                        if (ImGui::Button("Remove##light") && g_scene) {
                            toRemoveLight = light.get();
                        }
                    }
                    ImGui::PopID();
                }
                if (toRemoveLight && g_scene) {
                    removeLight(*engine, *g_scene, toRemoveLight);
                }
            }

            if (ImGui::CollapsingHeader("Post Processing")) {
                bool ppChanged = false;

                ImGui::Checkbox("Bloom##pp", &g_bloomOptions.enabled);
                ImGui::SliderFloat("Bloom strength##pp", &g_bloomOptions.strength, 0.0f, 1.0f);
                ppChanged = true;

                ImGui::Separator();
                ImGui::Checkbox("Ambient Occlusion##pp", &g_aoOptions.enabled);
                bool useGtao = g_aoOptions.aoType == AmbientOcclusionOptions::AmbientOcclusionType::GTAO;
                if (ImGui::Checkbox("Use GTAO##pp", &useGtao)) {
                    g_aoOptions.aoType = useGtao ? AmbientOcclusionOptions::AmbientOcclusionType::GTAO
                                                  : AmbientOcclusionOptions::AmbientOcclusionType::SAO;
                }
                ImGui::SliderFloat("AO radius##pp", &g_aoOptions.radius, 0.05f, 5.0f);
                ImGui::SliderFloat("AO strength##pp", &g_aoOptions.intensity, 0.0f, 4.0f);

                ImGui::Separator();
                ImGui::Checkbox("Vignette##pp", &g_vignetteOptions.enabled);
                ImGui::SliderFloat("Vignette midpoint##pp", &g_vignetteOptions.midPoint, 0.0f, 1.0f);
                ImGui::SliderFloat("Vignette feather##pp", &g_vignetteOptions.feather, 0.0f, 1.0f);

                ImGui::Separator();
                ImGui::Checkbox("Fog##pp", &g_fogOptions.enabled);
                ImGui::SliderFloat("Fog density##pp", &g_fogOptions.density, 0.0f, 0.5f);
                ImGui::SliderFloat("Fog height falloff##pp", &g_fogOptions.heightFalloff, 0.0f, 5.0f);

                ImGui::Separator();
                ImGui::Checkbox("Depth of Field##pp", &g_dofOptions.enabled);
                ImGui::SliderFloat("DoF blur (CoC scale)##pp", &g_dofOptions.cocScale, 0.1f, 5.0f);

                ImGui::Separator();
                ImGui::Checkbox("Temporal AA##pp", &g_taaOptions.enabled);

                ImGui::Separator();
                if (ImGui::SliderFloat("IBL / ambient intensity", &g_iblIntensity, 0.0f, 50000.0f)) {
                    applyIblIntensity();
                }

                ImGui::Separator();
                ImGui::TextUnformatted("Camera Exposure (photographic - each slider is highly "
                        "non-linear, small moves change brightness a lot)");
                bool exposureChanged = false;
                exposureChanged |= ImGui::SliderFloat("Aperture (f-stop)##exp", &g_aperture, 1.0f, 22.0f);
                exposureChanged |= ImGui::SliderFloat("Shutter (s)##exp", &g_shutterSpeed, 1.0f / 1000.0f, 1.0f / 10.0f, "%.4f");
                exposureChanged |= ImGui::SliderFloat("ISO##exp", &g_sensitivity, 25.0f, 6400.0f);
                if (exposureChanged) {
                    view->getCamera().setExposure(g_aperture, g_shutterSpeed, g_sensitivity);
                }

                if (ppChanged) {
                    view->setBloomOptions(g_bloomOptions);
                    view->setAmbientOcclusionOptions(g_aoOptions);
                    view->setVignetteOptions(g_vignetteOptions);
                    view->setFogOptions(g_fogOptions);
                    view->setDepthOfFieldOptions(g_dofOptions);
                    view->setTemporalAntiAliasingOptions(g_taaOptions);
                }
            }
            ImGui::End();

            // -------- selection highlight: wireframe box around the selected model --------
            if (g_selectedModel && g_selectedModel->asset && g_scene) {
                if (!g_selectionCube) {
                    // Cube unconditionally sets a "color" uniform on its material instance -
                    // the default material doesn't declare one (that's the crash this was
                    // hitting); DanteApp's own internal debug Cube uses the transparent
                    // material for exactly this reason, so it's the known-good choice here.
                    g_selectionCube = std::make_unique<Cube>(*engine, DanteApp::get().getTransparentMaterial(),
                            float3{1.0f, 0.9f, 0.2f});
                    utils::Entity const wire = g_selectionCube->getWireFrameRenderable();
                    auto& rm = engine->getRenderableManager();
                    auto inst = rm.getInstance(wire);
                    if (inst) {
                        rm.setLayerMask(inst, 0x4, 0x4);
                        rm.setCulling(inst, false);
                    }
                    g_scene->addEntity(wire);
                }
                auto const localBox = g_selectedModel->asset->getBoundingBox();
                float3 const localCenter = (localBox.min + localBox.max) * 0.5f;
                float3 const localHalf = (localBox.max - localBox.min) * 0.5f;
                Box worldBox;
                worldBox.center = g_selectedModel->position + localCenter * g_selectedModel->scale;
                worldBox.halfExtent = localHalf * g_selectedModel->scale + float3{0.02f};
                g_selectionCube->mapAabb(*engine, worldBox);
            } else if (g_selectionCube && g_scene) {
                g_scene->remove(g_selectionCube->getWireFrameRenderable());
                g_selectionCube.reset();
            }

            // -------- viewport picking / dragging --------
            ImGuiIO& io = ImGui::GetIO();
            bool const mouseOverPanel = io.WantCaptureMouse;

            // Picking is async (View::pick's result lands 1-2 frames after the query, not
            // the same frame as the click that triggered it) - so dragging is (re-)armed
            // here, once the result actually arrives, rather than only at click time. That
            // way a single click-and-hold gesture works on the very first click, instead of
            // needing an already-selected model from a prior click.
            if (g_pendingPickValid) {
                g_pendingPickValid = false;
                auto it = g_entityToModel.find(g_pendingPickEntity.getId());
                g_selectedModel = (it != g_entityToModel.end()) ? it->second : nullptr;
                if (g_selectedModel && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                    g_dragging = true;
                    float3 const camPos = float3(view->getCamera().getPosition());
                    g_dragDistance = std::max(0.1f, dot(g_selectedModel->position - camPos,
                            view->getCamera().getForwardVector()));
                } else {
                    g_dragging = false;
                }
            }

            if (g_editMode && !mouseOverPanel) {
                Viewport const& vp = view->getViewport();

                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    uint32_t const px = (uint32_t)std::max(0.0f, io.MousePos.x);
                    uint32_t const py = (uint32_t)std::max(0.0f, (float)vp.height - io.MousePos.y);
                    view->pick(px, py, [](View::PickingQueryResult const& result) {
                        g_pendingPickEntity = result.renderable;
                        g_pendingPickValid = true;
                    });
                    // If something was already selected before this click, start dragging
                    // immediately instead of waiting on the new pick result - handles
                    // "click-drag on the already-selected model" with zero latency.
                    if (g_selectedModel) {
                        g_dragging = true;
                        float3 const camPos = float3(view->getCamera().getPosition());
                        g_dragDistance = std::max(0.1f, dot(g_selectedModel->position - camPos,
                                view->getCamera().getForwardVector()));
                    }
                }

                if (g_dragging && g_selectedModel && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                    float3 const left = view->getCamera().getLeftVector();
                    float3 const up = view->getCamera().getUpVector();
                    float const worldPerPixel = g_dragDistance * 0.0016f;
                    g_selectedModel->position += (-left) * (io.MouseDelta.x * worldPerPixel)
                            + up * (-io.MouseDelta.y * worldPerPixel);
                    g_selectedModel->applyTransform(*engine);
                }

                if (g_selectedModel && io.MouseWheel != 0.0f) {
                    float3 const fwd = view->getCamera().getForwardVector();
                    g_selectedModel->position += fwd * io.MouseWheel * g_dragDistance * 0.08f;
                    g_selectedModel->applyTransform(*engine);
                }
            }

            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                g_dragging = false;
            }

            // DanteApp gates all mouse dispatch to the free-fly camera manipulator on
            // !io.WantCaptureMouse (see DanteApp.cpp's event loop) - forcing this is the
            // only hook available to reclaim the mouse for picking/dragging above without
            // touching DanteApp itself. One-frame-late (this only affects next frame's
            // event dispatch, since event polling for this frame already happened before
            // this callback ran), same as ImGui's own hover state normally behaves.
            if (g_editMode) {
                io.WantCaptureMouse = true;
            }

            // Temporary debug readout for diagnosing edit-mode/drag issues - shows the
            // actual per-frame state so a report like "dragging does nothing" can be
            // matched against what the code thinks is happening, instead of guessing blind.
            ImGui::SetNextWindowPos(ImVec2(10, 700), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowBgAlpha(0.6f);
            ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::Text("Edit Mode: %s", g_editMode ? "ON" : "off");
            ImGui::Text("Selected model: %s", g_selectedModel ? g_selectedModel->name.c_str() : "(none)");
            ImGui::Text("Dragging: %s", g_dragging ? "yes" : "no");
            ImGui::Text("WantCaptureMouse: %s", io.WantCaptureMouse ? "true" : "false");
            ImGui::Text("Mouse pos: %.0f, %.0f  delta: %.1f, %.1f",
                    io.MousePos.x, io.MousePos.y, io.MouseDelta.x, io.MouseDelta.y);
            ImGui::Text("Mouse down (L): %s   clicked (L): %s",
                    ImGui::IsMouseDown(ImGuiMouseButton_Left) ? "yes" : "no",
                    ImGui::IsMouseClicked(ImGuiMouseButton_Left) ? "yes" : "no");
            if (g_selectedModel) {
                ImGui::Text("Model position: %.3f, %.3f, %.3f", g_selectedModel->position.x,
                        g_selectedModel->position.y, g_selectedModel->position.z);
            }
            ImGui::End();
        },
        DanteApp::PreRenderCallback(), DanteApp::PostRenderCallback(),
        // Default window was 1024x640 - too short to fit the Scene panel once Environments/
        // Add model/Loaded models/Lights/Post Processing are all visible at once (the
        // "can't find it" reports were very likely this, not a scan bug - the panel content
        // was probably just overflowing off the bottom of the window).
        1920, 1080);

    return 0;
}
