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
#include <imgui_internal.h> // DockBuilder* - used to lay out the default panel arrangement
#include <ImGuizmo.h>
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
#include "Performance.h"
#include <chrono> 
#include <thread> 

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
    // Stable identity for the undo/redo system (see "undo/redo" section below) - a raw
    // GltfModel* dangles the moment the model is removed and later un-removed by an undo,
    // since that destroys and reallocates the object at a new address. Commands look models
    // up by id instead, which stays valid (and gets deliberately reused, not reassigned)
    // across a remove/undo-remove or a Reload.
    uint64_t id = 0;

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
        // Composed as RotX * RotY * RotZ (rather than Filament's eulerZYX helper) to match
        // ImGuizmo::RecomposeMatrixFromComponents exactly - the Rotate gizmo below edits this
        // same matrix and decomposes it back into rotationDeg via ImGuizmo's own Decompose, so
        // both paths need to agree on the Euler convention or values would drift every time
        // rotation is touched from one path after the other.
        mat4f const rotation = mat4f::rotation(rotationDeg.x * kDegToRad, float3{1, 0, 0})
                * mat4f::rotation(rotationDeg.y * kDegToRad, float3{0, 1, 0})
                * mat4f::rotation(rotationDeg.z * kDegToRad, float3{0, 0, 1});
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
    uint64_t id = 0; // see the matching comment on GltfModel::id
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

// Set once in the setup callback below, alongside g_scene/g_mainView - undo/redo commands
// (see further down) need an Engine& to replay onto and only ever run from the same UI
// callback this was captured in, so a stashed raw pointer is as safe as g_scene/g_mainView
// already are.
Engine* g_engine = nullptr;

uint64_t g_nextObjectId = 1; // 0 is reserved as "no id"

GltfModel* findModelById(uint64_t id) {
    for (auto& m : g_models) {
        if (m->id == id) return m.get();
    }
    return nullptr;
}

// The setup callback's View* is the real 3D scene view. The ImGui callback's View*
// parameter is NOT the same view - DanteApp constructs its ImGuiHelper against a separate,
// dedicated UI-compositing view (window->mUiView, see DanteApp.cpp) that has its own empty
// scene, its own camera, and post-processing explicitly disabled. Every setXOptions/pick/
// getCamera call made against the ImGui callback's own view parameter is a silent no-op on
// the real scene - g_mainView, captured here during setup, is the one to use instead.
View* g_mainView = nullptr;

std::vector<std::unique_ptr<PointLight>> g_lights;
PointLight* g_selectedLight = nullptr;

PointLight* findLightById(uint64_t id) {
    for (auto& l : g_lights) {
        if (l->id == id) return l.get();
    }
    return nullptr;
}

// Edit-mode state: while active, the free-fly camera's mouse look is suppressed (see the
// WantCaptureMouse override at the bottom of the ImGui callback) and left-click instead
// drives picking/dragging.
bool g_editMode = false;
bool g_dragging = false;
float g_dragDistance = 1.0f;
utils::Entity g_pendingPickEntity;
bool g_pendingPickValid = false;

// Current gizmo tool for the selected model. SCALEU (universal/uniform scale) rather than
// SCALE (independent per-axis handles) because GltfModel only stores a single uniform `scale`
// float - a non-uniform scale gizmo would have nowhere to put its x/y/z result.
ImGuizmo::OPERATION g_gizmoOperation = ImGuizmo::TRANSLATE;

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

// FREE_FLIGHT camera max move speed, world units/sec (camutils' own default is 10).
// Exposed live via the Camera Speed slider, applied through DanteApp::setCameraMoveSpeed.
float g_cameraMoveSpeed = 24.0f;

std::unique_ptr<Cube> g_selectionCube;

// reuseId lets a caller force a specific id instead of minting a fresh one - used when
// undo/reload recreate "the same" logical model, so undo commands recorded before the
// destroy/recreate cycle can still find it via findModelById().
GltfModel* loadModelIntoScene(Engine& engine, Scene& scene, utils::Path const& path,
        float3 position, float scale = 1.0f, uint64_t reuseId = 0) {
    auto model = std::make_unique<GltfModel>();
    model->create(engine, scene, path, position, scale);
    if (!model->asset) {
        return nullptr;
    }
    model->id = reuseId != 0 ? reuseId : g_nextObjectId++;
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
    uint64_t const id = target->id;
    unloadModel(engine, scene, target); // invalidates target
    // Reusing the id keeps this "the same" model as far as any undo/redo command already
    // recorded against it is concerned - Reload re-imports from disk but isn't itself a
    // scene edit worth an undo step (see the "undo/redo" section further down).
    GltfModel* fresh = loadModelIntoScene(engine, scene, path, pos, scl, id);
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

// reuseId: see the matching comment on loadModelIntoScene() above.
PointLight* addLight(Engine& engine, Scene& scene, std::string const& name, float3 position,
        float3 color = {1.0f, 0.85f, 0.6f}, float intensity = 4000.0f, float falloff = 3.0f,
        uint64_t reuseId = 0) {
    auto light = std::make_unique<PointLight>();
    light->id = reuseId != 0 ? reuseId : g_nextObjectId++;
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

// -------- undo/redo --------
// A single stack with a cursor rather than separate undo/redo stacks: g_undoCursor is the
// number of entries in g_undoStack currently "applied" (undo-able); anything from the cursor
// onward is a redo-able tail. Recording a new command drops that tail, same as any other
// undo system - you can't redo into a future a fresh edit just overwrote.
//
// Commands reference models/lights by the stable id added above rather than a cached raw
// pointer - remove+undo (or Reload) destroys and reallocates the underlying object at a new
// address, so a pointer captured before that would dangle. Post-processing/skybox/exposure
// changes are deliberately not covered - lower value to undo, and would double the number of
// command types for it.
struct Command {
    virtual ~Command() = default;
    virtual void undo() = 0;
    virtual void redo() = 0;
};

std::vector<std::unique_ptr<Command>> g_undoStack;
size_t g_undoCursor = 0;

void pushCommand(std::unique_ptr<Command> cmd) {
    g_undoStack.resize(g_undoCursor); // drop any redo tail a new edit just invalidated
    g_undoStack.push_back(std::move(cmd));
    g_undoCursor = g_undoStack.size();
}

void performUndo() {
    if (g_undoCursor == 0) return;
    g_undoStack[--g_undoCursor]->undo();
}

void performRedo() {
    if (g_undoCursor >= g_undoStack.size()) return;
    g_undoStack[g_undoCursor++]->redo();
}

struct ModelTransformCommand : Command {
    uint64_t modelId = 0;
    float3 beforePos{}, beforeRot{}, afterPos{}, afterRot{};
    float beforeScale = 1.0f, afterScale = 1.0f;

    void apply(float3 const& pos, float3 const& rot, float scale) {
        GltfModel* m = findModelById(modelId);
        if (!m) return;
        m->position = pos;
        m->rotationDeg = rot;
        m->scale = scale;
        m->applyTransform(*g_engine);
        g_selectedModel = m;
        g_selectedLight = nullptr;
    }
    void undo() override { apply(beforePos, beforeRot, beforeScale); }
    void redo() override { apply(afterPos, afterRot, afterScale); }
};

struct LightParamsCommand : Command {
    uint64_t lightId = 0;
    float3 beforePos{}, beforeColor{}, afterPos{}, afterColor{};
    float beforeIntensity = 0, beforeFalloff = 0, afterIntensity = 0, afterFalloff = 0;

    void apply(float3 const& pos, float3 const& color, float intensity, float falloff) {
        PointLight* l = findLightById(lightId);
        if (!l) return;
        l->position = pos;
        l->color = color;
        l->intensity = intensity;
        l->falloff = falloff;
        applyLight(*g_engine, l);
        g_selectedLight = l;
        g_selectedModel = nullptr;
    }
    void undo() override { apply(beforePos, beforeColor, beforeIntensity, beforeFalloff); }
    void redo() override { apply(afterPos, afterColor, afterIntensity, afterFalloff); }
};

// Covers both "this model was added" and "this model was removed" - undo/redo just pick
// which of create()/destroy() to run in which direction.
struct ModelExistenceCommand : Command {
    bool wasAdd = true;
    uint64_t modelId = 0;
    utils::Path path;
    float3 position{}, rotationDeg{};
    float scale = 1.0f;

    void create() {
        if (!g_scene || !g_engine) return;
        GltfModel* m = loadModelIntoScene(*g_engine, *g_scene, path, position, scale, modelId);
        if (m) {
            m->rotationDeg = rotationDeg;
            m->applyTransform(*g_engine);
            g_selectedModel = m;
            g_selectedLight = nullptr;
        }
    }
    void destroy() {
        GltfModel* m = findModelById(modelId);
        if (m && g_scene && g_engine) {
            unloadModel(*g_engine, *g_scene, m);
        }
    }
    void undo() override { wasAdd ? destroy() : create(); }
    void redo() override { wasAdd ? create() : destroy(); }
};

struct LightExistenceCommand : Command {
    bool wasAdd = true;
    uint64_t lightId = 0;
    std::string name;
    float3 position{}, color{};
    float intensity = 0, falloff = 0;

    void create() {
        if (!g_scene || !g_engine) return;
        PointLight* l = addLight(*g_engine, *g_scene, name, position, color, intensity, falloff, lightId);
        g_selectedLight = l;
        g_selectedModel = nullptr;
    }
    void destroy() {
        PointLight* l = findLightById(lightId);
        if (l && g_scene && g_engine) {
            removeLight(*g_engine, *g_scene, l);
        }
    }
    void undo() override { wasAdd ? destroy() : create(); }
    void redo() override { wasAdd ? create() : destroy(); }
};

void recordModelAdded(GltfModel* m) {
    if (!m) return;
    auto cmd = std::make_unique<ModelExistenceCommand>();
    cmd->wasAdd = true;
    cmd->modelId = m->id;
    cmd->path = m->sourcePath;
    cmd->position = m->position;
    cmd->rotationDeg = m->rotationDeg;
    cmd->scale = m->scale;
    pushCommand(std::move(cmd));
}

void recordModelWillBeRemoved(GltfModel* m) {
    if (!m) return;
    auto cmd = std::make_unique<ModelExistenceCommand>();
    cmd->wasAdd = false;
    cmd->modelId = m->id;
    cmd->path = m->sourcePath;
    cmd->position = m->position;
    cmd->rotationDeg = m->rotationDeg;
    cmd->scale = m->scale;
    pushCommand(std::move(cmd));
}

void recordLightAdded(PointLight* l) {
    if (!l) return;
    auto cmd = std::make_unique<LightExistenceCommand>();
    cmd->wasAdd = true;
    cmd->lightId = l->id;
    cmd->name = l->name;
    cmd->position = l->position;
    cmd->color = l->color;
    cmd->intensity = l->intensity;
    cmd->falloff = l->falloff;
    pushCommand(std::move(cmd));
}

void recordLightWillBeRemoved(PointLight* l) {
    if (!l) return;
    auto cmd = std::make_unique<LightExistenceCommand>();
    cmd->wasAdd = false;
    cmd->lightId = l->id;
    cmd->name = l->name;
    cmd->position = l->position;
    cmd->color = l->color;
    cmd->intensity = l->intensity;
    cmd->falloff = l->falloff;
    pushCommand(std::move(cmd));
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

// DANTE_ASSETS_DIR is passed unquoted from CMake (target_compile_definitions(Dante PRIVATE
// DANTE_ASSETS_DIR=${CMAKE_SOURCE_DIR}/assets)) and stringized here rather than passed as a
// pre-quoted string literal - CMake/Ninja on Windows with a non-MSVC compiler (e.g. zig cc)
// doesn't reliably preserve the escaped inner quotes of a `-DFOO="bar"`-style definition
// through to the compiler, silently dropping them.
#define DANTE_STRINGIZE_HELPER(x) #x
#define DANTE_STRINGIZE(x) DANTE_STRINGIZE_HELPER(x)

utils::Path resolveAssetsDir() {
    utils::Path portable = utils::Path::getCurrentExecutable().getParent() + "assets";
    return portable.exists() ? portable : utils::Path(DANTE_STRINGIZE(DANTE_ASSETS_DIR));
}

utils::Path sceneFilePath(utils::Path const& assetsDir) {
    return assetsDir + "scene.txt";
}

// Wraps a free-text field (asset path, light name) in quotes so it can contain spaces -
// common in glTF packs straight off Sketchfab, and even in the default "New Light" name.
// Everything else in the format is still plain whitespace-tokenized - simple to read/write
// without a JSON dependency - so only these fields need the quoting.
void writeQuoted(std::ofstream& out, std::string const& s) {
    out << '"' << s << '"';
}

// Reads one "..."-quoted field. Falls back to a plain whitespace-delimited token when the
// next non-whitespace character isn't a quote, so scene files saved before quoting was added
// (or hand-edited without it) still load correctly.
bool readQuoted(std::ifstream& in, std::string& out) {
    in >> std::ws;
    if (in.peek() != '"') {
        in >> out;
        return (bool)in;
    }
    in.get(); // consume the opening quote
    return (bool)std::getline(in, out, '"');
}

// Known limitation: paths are stored absolute, so a saved scene is tied to this machine.
void saveScene(utils::Path const& file) {
    std::ofstream out(file.getPath());
    if (!out) {
        fprintf(stderr, "[Dante] failed to write scene file %s\n", file.c_str());
        return;
    }
    out << "skybox ";
    writeQuoted(out, g_currentSkyboxPath);
    out << "\n";
    out << "iblintensity " << g_iblIntensity << "\n";
    for (auto const& m : g_models) {
        out << "model ";
        writeQuoted(out, m->sourcePath.getAbsolutePath().c_str());
        out << " "
            << m->position.x << " " << m->position.y << " " << m->position.z << " "
            << m->rotationDeg.x << " " << m->rotationDeg.y << " " << m->rotationDeg.z << " "
            << m->scale << "\n";
    }
    for (auto const& l : g_lights) {
        out << "light ";
        writeQuoted(out, l->name);
        out << " "
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
    // Every model/light id undo history could reference is about to be destroyed - old
    // commands would just silently no-op via findModelById()/findLightById() returning
    // null, but keeping them around serves no purpose once the whole scene has changed.
    g_undoStack.clear();
    g_undoCursor = 0;
    std::string tag;
    while (in >> tag) {
        if (tag == "skybox") {
            std::string path;
            readQuoted(in, path);
            g_currentSkyboxPath = path;
            DanteApp::get().loadIBL(path);
            applyIblIntensity();
        } else if (tag == "iblintensity") {
            in >> g_iblIntensity;
            applyIblIntensity();
        } else if (tag == "model") {
            std::string path;
            readQuoted(in, path);
            float3 pos, rot;
            float scl;
            in >> pos.x >> pos.y >> pos.z >> rot.x >> rot.y >> rot.z >> scl;
            GltfModel* m = loadModelIntoScene(engine, scene, utils::Path(path), pos, scl);
            if (m) {
                m->rotationDeg = rot;
                m->applyTransform(engine);
            }
        } else if (tag == "light") {
            std::string name;
            readQuoted(in, name);
            float3 pos, color;
            float intensity, falloff;
            in >> pos.x >> pos.y >> pos.z >> color.x >> color.y >> color.z >> intensity >> falloff;
            addLight(engine, scene, name, pos, color, intensity, falloff);
        }
    }
    fprintf(stderr, "[Dante] loaded scene from %s\n", file.c_str());
    return true;
}

// Blender-esque dark theme: neutral dark-gray panels, generous spacing, soft uniform rounding,
// and Blender's characteristic warm-orange accent on anything interactive/active - replacing
// ImGui's default (cramped spacing, square corners, purple accent) debug-overlay look.
void applyEditorStyle() {
    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowPadding     = ImVec2(10, 10);
    style.FramePadding      = ImVec2(8, 4);
    style.ItemSpacing       = ImVec2(8, 6);
    style.ItemInnerSpacing  = ImVec2(6, 4);
    style.IndentSpacing     = 18.0f;
    style.ScrollbarSize     = 14.0f;
    style.GrabMinSize       = 10.0f;

    style.WindowRounding    = 6.0f;
    style.ChildRounding     = 4.0f;
    style.FrameRounding     = 4.0f;
    style.PopupRounding     = 4.0f;
    style.ScrollbarRounding = 6.0f;
    style.GrabRounding      = 4.0f;
    style.TabRounding       = 4.0f;

    style.WindowBorderSize  = 1.0f;
    style.FrameBorderSize   = 0.0f;
    style.PopupBorderSize   = 1.0f;

    float3 const bgDark     = {0.11f, 0.11f, 0.11f};
    float3 const bgPanel    = {0.16f, 0.16f, 0.16f};
    float3 const bgWidget   = {0.22f, 0.22f, 0.22f};
    float3 const bgWidgetHi = {0.27f, 0.27f, 0.27f};
    float3 const accent     = {0.94f, 0.54f, 0.09f};
    float3 const border     = {0.06f, 0.06f, 0.06f};

    auto rgba = [](float3 c, float a) { return ImVec4(c.x, c.y, c.z, a); };

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text]                  = rgba({0.92f, 0.92f, 0.92f}, 1.00f);
    colors[ImGuiCol_TextDisabled]          = rgba({0.60f, 0.60f, 0.60f}, 1.00f);
    colors[ImGuiCol_WindowBg]              = rgba(bgPanel, 1.00f);
    colors[ImGuiCol_ChildBg]               = rgba(bgPanel, 0.00f);
    colors[ImGuiCol_PopupBg]               = rgba(bgPanel, 1.00f);
    colors[ImGuiCol_Border]                = rgba(border, 1.00f);
    colors[ImGuiCol_BorderShadow]          = rgba(border, 0.00f);
    colors[ImGuiCol_FrameBg]               = rgba(bgWidget, 1.00f);
    colors[ImGuiCol_FrameBgHovered]        = rgba(bgWidgetHi, 1.00f);
    colors[ImGuiCol_FrameBgActive]         = rgba(accent, 0.60f);
    colors[ImGuiCol_TitleBg]               = rgba(bgDark, 1.00f);
    colors[ImGuiCol_TitleBgActive]         = rgba(bgDark, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]      = rgba(bgDark, 1.00f);
    colors[ImGuiCol_MenuBarBg]             = rgba(bgDark, 1.00f);
    colors[ImGuiCol_ScrollbarBg]           = rgba(bgDark, 1.00f);
    colors[ImGuiCol_ScrollbarGrab]         = rgba(bgWidgetHi, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]  = rgba(accent, 0.60f);
    colors[ImGuiCol_ScrollbarGrabActive]   = rgba(accent, 1.00f);
    colors[ImGuiCol_CheckMark]             = rgba(accent, 1.00f);
    colors[ImGuiCol_SliderGrab]            = rgba(accent, 0.85f);
    colors[ImGuiCol_SliderGrabActive]      = rgba(accent, 1.00f);
    colors[ImGuiCol_Button]                = rgba(bgWidget, 1.00f);
    colors[ImGuiCol_ButtonHovered]         = rgba(bgWidgetHi, 1.00f);
    colors[ImGuiCol_ButtonActive]          = rgba(accent, 0.60f);
    colors[ImGuiCol_Header]                = rgba(bgWidget, 1.00f);
    colors[ImGuiCol_HeaderHovered]         = rgba(bgWidgetHi, 1.00f);
    colors[ImGuiCol_HeaderActive]          = rgba(accent, 0.60f);
    colors[ImGuiCol_Separator]             = rgba(border, 1.00f);
    colors[ImGuiCol_SeparatorHovered]      = rgba(accent, 0.60f);
    colors[ImGuiCol_SeparatorActive]       = rgba(accent, 1.00f);
    colors[ImGuiCol_ResizeGrip]            = rgba(accent, 0.00f);
    colors[ImGuiCol_ResizeGripHovered]     = rgba(accent, 0.60f);
    colors[ImGuiCol_ResizeGripActive]      = rgba(accent, 1.00f);
    colors[ImGuiCol_Tab]                   = rgba(bgDark, 1.00f);
    colors[ImGuiCol_TabHovered]            = rgba(bgWidgetHi, 1.00f);
    colors[ImGuiCol_TabSelected]           = rgba(bgWidget, 1.00f);
    colors[ImGuiCol_TabSelectedOverline]   = rgba(accent, 1.00f);
    colors[ImGuiCol_TabDimmed]             = rgba(bgDark, 1.00f);
    colors[ImGuiCol_TabDimmedSelected]     = rgba(bgWidget, 1.00f);
    colors[ImGuiCol_DockingPreview]        = rgba(accent, 0.35f);
    colors[ImGuiCol_DockingEmptyBg]        = rgba(bgDark, 1.00f);
    colors[ImGuiCol_NavCursor]             = rgba(accent, 1.00f);
    colors[ImGuiCol_PlotLines]             = rgba(accent, 1.00f);
    colors[ImGuiCol_PlotHistogram]         = rgba(accent, 1.00f);
}

// -------- editor panels --------
// Each panel is its own top-level ImGui::Begin/End window rather than a CollapsingHeader
// stacked inside one giant "Scene" window - they dock independently (see buildDefaultDockLayout
// below for the first-launch arrangement), so opening several at once no longer balloons a
// single window over the 3D viewport.

void drawMainMenuBar(Engine* engine, utils::Path const& assetsDir) {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save Scene")) {
                saveScene(sceneFilePath(assetsDir));
            }
            if (ImGui::MenuItem("Load Scene") && g_scene) {
                loadScene(*engine, *g_scene, sceneFilePath(assetsDir));
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "Ctrl+Z", false, g_undoCursor > 0)) {
                performUndo();
            }
            if (ImGui::MenuItem("Redo", "Ctrl+Y", false, g_undoCursor < g_undoStack.size())) {
                performRedo();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

// Blender-style header strip: mode + active tool + the one camera setting worth reaching for
// constantly, all visible regardless of which side panel currently has focus.
void drawToolbarPanel() {
    ImGui::Begin("Toolbar", nullptr, ImGuiWindowFlags_NoScrollbar);
    ImGui::Checkbox("Edit Mode (E)", &g_editMode);
    ImGui::SameLine();
    ImGui::TextUnformatted("Gizmo:");
    ImGui::SameLine();
    if (ImGui::RadioButton("Translate (1)", g_gizmoOperation == ImGuizmo::TRANSLATE)) {
        g_gizmoOperation = ImGuizmo::TRANSLATE;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate (2)", g_gizmoOperation == ImGuizmo::ROTATE)) {
        g_gizmoOperation = ImGuizmo::ROTATE;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale (3)", g_gizmoOperation == ImGuizmo::SCALEU)) {
        g_gizmoOperation = ImGuizmo::SCALEU;
    }
    ImGui::SameLine();
    ImGui::SetNextItemWidth(160.0f);
    if (ImGui::SliderFloat("Camera Speed", &g_cameraMoveSpeed, 1.0f, 100.0f, "%.1f")) {
        DanteApp::get().setCameraMoveSpeed(g_cameraMoveSpeed);
    }
    ImGui::End();
}

// Scene hierarchy: selection + add/remove/duplicate live here, same as Blender's Outliner -
// the actual field-by-field editing of whatever's selected lives in the Properties panel
// instead, so this list doesn't grow a wall of drag floats per row.
void drawOutlinerPanel(Engine* engine, View* view) {
    ImGui::Begin("Outliner");

    if (ImGui::CollapsingHeader("Models", ImGuiTreeNodeFlags_DefaultOpen)) {
        GltfModel* toRemove = nullptr;
        GltfModel* toReload = nullptr;
        GltfModel* toDuplicate = nullptr;
        for (auto& model : g_models) {
            ImGui::PushID(model.get());
            bool const isSelected = g_selectedModel == model.get();
            if (ImGui::Selectable(model->name.c_str(), isSelected)) {
                g_selectedModel = model.get();
                g_selectedLight = nullptr;
            }
            if (isSelected) {
                if (ImGui::SmallButton("Remove") && g_scene) {
                    toRemove = model.get();
                }
                ImGui::SameLine();
                if (ImGui::SmallButton("Reload") && g_scene) {
                    toReload = model.get();
                }
                ImGui::SameLine();
                if (ImGui::SmallButton("Duplicate") && g_scene) {
                    toDuplicate = model.get();
                }
            }
            ImGui::PopID();
        }
        if (toRemove && g_scene) {
            recordModelWillBeRemoved(toRemove); // capture state before it's gone
            unloadModel(*engine, *g_scene, toRemove);
        }
        if (toReload && g_scene) {
            reloadModel(*engine, *g_scene, toReload);
        }
        if (toDuplicate && g_scene) {
            g_selectedModel = duplicateModel(*engine, *g_scene, toDuplicate);
            g_selectedLight = nullptr;
            recordModelAdded(g_selectedModel);
        }
    }

    if (ImGui::CollapsingHeader("Lights", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::Button("Add Light Here") && g_scene) {
            float3 const camPos = float3(view->getCamera().getPosition());
            float3 const fwd = view->getCamera().getForwardVector();
            g_selectedLight = addLight(*engine, *g_scene, "New Light", camPos + fwd * 2.0f);
            g_selectedModel = nullptr;
            recordLightAdded(g_selectedLight);
        }
        PointLight* toRemoveLight = nullptr;
        for (auto& light : g_lights) {
            ImGui::PushID(light.get());
            bool const isSelected = g_selectedLight == light.get();
            if (ImGui::Selectable(light->name.c_str(), isSelected)) {
                g_selectedLight = light.get();
                g_selectedModel = nullptr;
            }
            if (isSelected) {
                if (ImGui::SmallButton("Remove##light") && g_scene) {
                    toRemoveLight = light.get();
                }
            }
            ImGui::PopID();
        }
        if (toRemoveLight && g_scene) {
            recordLightWillBeRemoved(toRemoveLight); // capture state before it's gone
            removeLight(*engine, *g_scene, toRemoveLight);
        }
    }

    ImGui::End();
}

// Detail editor for whichever single thing is selected - mirrors Blender's Properties panel
// showing the selected object's fields rather than every object's fields inline in a list.
//
// Each drag gesture across these fields becomes exactly one undo step, not one per frame the
// mouse moves while held: IsItemActivated()/IsItemDeactivatedAfterEdit() bracket the whole
// gesture, and the "before" snapshot is read *before* that frame's DragFloat call runs (in
// case the very frame a drag starts also applies its first delta).
void drawPropertiesPanel(Engine* engine) {
    ImGui::Begin("Properties");
    if (g_selectedModel) {
        static uint64_t sDragModelId = 0;
        static float3 sDragBeforePos, sDragBeforeRot;
        static float sDragBeforeScale = 1.0f;

        ImGui::TextUnformatted(g_selectedModel->name.c_str());
        ImGui::Separator();

        float3 const preCallPos = g_selectedModel->position;
        float3 const preCallRot = g_selectedModel->rotationDeg;
        float const preCallScale = g_selectedModel->scale;

        bool changed = false;
        bool anyActivated = false, anyDeactivated = false;

        changed |= ImGui::DragFloat3("Position", &g_selectedModel->position.x, 0.02f);
        anyActivated |= ImGui::IsItemActivated();
        anyDeactivated |= ImGui::IsItemDeactivatedAfterEdit();

        changed |= ImGui::DragFloat3("Rotation", &g_selectedModel->rotationDeg.x, 0.5f);
        anyActivated |= ImGui::IsItemActivated();
        anyDeactivated |= ImGui::IsItemDeactivatedAfterEdit();

        changed |= ImGui::DragFloat("Scale", &g_selectedModel->scale, 0.01f, 0.001f, 100.0f);
        anyActivated |= ImGui::IsItemActivated();
        anyDeactivated |= ImGui::IsItemDeactivatedAfterEdit();

        if (anyActivated) {
            sDragModelId = g_selectedModel->id;
            sDragBeforePos = preCallPos;
            sDragBeforeRot = preCallRot;
            sDragBeforeScale = preCallScale;
        }
        if (changed) {
            g_selectedModel->applyTransform(*engine);
        }
        if (anyDeactivated && sDragModelId == g_selectedModel->id) {
            auto cmd = std::make_unique<ModelTransformCommand>();
            cmd->modelId = sDragModelId;
            cmd->beforePos = sDragBeforePos;
            cmd->beforeRot = sDragBeforeRot;
            cmd->beforeScale = sDragBeforeScale;
            cmd->afterPos = g_selectedModel->position;
            cmd->afterRot = g_selectedModel->rotationDeg;
            cmd->afterScale = g_selectedModel->scale;
            pushCommand(std::move(cmd));
            sDragModelId = 0;
        }
    } else if (g_selectedLight) {
        static uint64_t sDragLightId = 0;
        static float3 sDragBeforePos, sDragBeforeColor;
        static float sDragBeforeIntensity = 0, sDragBeforeFalloff = 0;

        ImGui::TextUnformatted(g_selectedLight->name.c_str());
        ImGui::Separator();

        float3 const preCallPos = g_selectedLight->position;
        float3 const preCallColor = g_selectedLight->color;
        float const preCallIntensity = g_selectedLight->intensity;
        float const preCallFalloff = g_selectedLight->falloff;

        bool changed = false;
        bool anyActivated = false, anyDeactivated = false;

        changed |= ImGui::DragFloat3("Position##light", &g_selectedLight->position.x, 0.02f);
        anyActivated |= ImGui::IsItemActivated();
        anyDeactivated |= ImGui::IsItemDeactivatedAfterEdit();

        changed |= ImGui::ColorEdit3("Color##light", &g_selectedLight->color.x);
        anyActivated |= ImGui::IsItemActivated();
        anyDeactivated |= ImGui::IsItemDeactivatedAfterEdit();

        changed |= ImGui::DragFloat("Intensity (lm)##light", &g_selectedLight->intensity, 25.0f, 0.0f, 200000.0f);
        anyActivated |= ImGui::IsItemActivated();
        anyDeactivated |= ImGui::IsItemDeactivatedAfterEdit();

        changed |= ImGui::DragFloat("Falloff (m)##light", &g_selectedLight->falloff, 0.05f, 0.1f, 50.0f);
        anyActivated |= ImGui::IsItemActivated();
        anyDeactivated |= ImGui::IsItemDeactivatedAfterEdit();

        if (anyActivated) {
            sDragLightId = g_selectedLight->id;
            sDragBeforePos = preCallPos;
            sDragBeforeColor = preCallColor;
            sDragBeforeIntensity = preCallIntensity;
            sDragBeforeFalloff = preCallFalloff;
        }
        if (changed) {
            applyLight(*engine, g_selectedLight);
        }
        if (anyDeactivated && sDragLightId == g_selectedLight->id) {
            auto cmd = std::make_unique<LightParamsCommand>();
            cmd->lightId = sDragLightId;
            cmd->beforePos = sDragBeforePos;
            cmd->beforeColor = sDragBeforeColor;
            cmd->beforeIntensity = sDragBeforeIntensity;
            cmd->beforeFalloff = sDragBeforeFalloff;
            cmd->afterPos = g_selectedLight->position;
            cmd->afterColor = g_selectedLight->color;
            cmd->afterIntensity = g_selectedLight->intensity;
            cmd->afterFalloff = g_selectedLight->falloff;
            pushCommand(std::move(cmd));
            sDragLightId = 0;
        }
    } else {
        ImGui::TextDisabled("Nothing selected.");
        ImGui::TextWrapped("Select a model or light in the Outliner, or click one in the "
                "viewport while Edit Mode is on.");
    }
    ImGui::End();
}

// Asset browser: everything on disk under assets/models and assets/environments, rescanned
// on demand rather than at every frame.
void drawAssetsPanel(Engine* engine, View* view, utils::Path const& assetsDir) {
    static std::vector<utils::Path> glbFiles;
    static std::vector<utils::Path> exrFiles;
    static bool scanned = false;
    if (!scanned) {
        // Both packaged (.glb) and unpacked (.gltf + .bin + loose textures) glTF layouts are
        // common export formats (e.g. Sketchfab offers either) - AssetLoader::createAsset()
        // handles both transparently, so both need to be findable here too.
        glbFiles = findFilesByExtension(assetsDir + "models", "glb");
        std::vector<utils::Path> const gltfFiles = findFilesByExtension(assetsDir + "models", "gltf");
        glbFiles.insert(glbFiles.end(), gltfFiles.begin(), gltfFiles.end());
        exrFiles = findFilesByExtension(assetsDir + "environments", "exr");
        scanned = true;
    }

    ImGui::Begin("Assets");
    if (ImGui::Button("Rescan assets/")) {
        scanned = false;
    }
    ImGui::SameLine();
    ImGui::Text("%zu model(s), %zu environment(s)", glbFiles.size(), exrFiles.size());

    if (ImGui::CollapsingHeader("Environments", ImGuiTreeNodeFlags_DefaultOpen)) {
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
                g_selectedLight = nullptr;
                recordModelAdded(g_selectedModel);
            }
            ImGui::PopID();
        }
    }
    ImGui::End();
}

void drawPostProcessingPanel(View* view) {
    ImGui::Begin("Post Processing");
    bool ppChanged = false;

    ppChanged |= ImGui::Checkbox("Bloom##pp", &g_bloomOptions.enabled);
    ppChanged |= ImGui::SliderFloat("Bloom strength##pp", &g_bloomOptions.strength, 0.0f, 1.0f);

    ImGui::Separator();
    ppChanged |= ImGui::Checkbox("Ambient Occlusion##pp", &g_aoOptions.enabled);
    bool useGtao = g_aoOptions.aoType == AmbientOcclusionOptions::AmbientOcclusionType::GTAO;
    if (ImGui::Checkbox("Use GTAO##pp", &useGtao)) {
        g_aoOptions.aoType = useGtao ? AmbientOcclusionOptions::AmbientOcclusionType::GTAO
                                      : AmbientOcclusionOptions::AmbientOcclusionType::SAO;
        ppChanged = true;
    }
    ppChanged |= ImGui::SliderFloat("AO radius##pp", &g_aoOptions.radius, 0.05f, 5.0f);
    ppChanged |= ImGui::SliderFloat("AO strength##pp", &g_aoOptions.intensity, 0.0f, 4.0f);

    ImGui::Separator();
    ppChanged |= ImGui::Checkbox("Vignette##pp", &g_vignetteOptions.enabled);
    ppChanged |= ImGui::SliderFloat("Vignette midpoint##pp", &g_vignetteOptions.midPoint, 0.0f, 1.0f);
    ppChanged |= ImGui::SliderFloat("Vignette feather##pp", &g_vignetteOptions.feather, 0.0f, 1.0f);

    ImGui::Separator();
    ppChanged |= ImGui::Checkbox("Fog##pp", &g_fogOptions.enabled);
    ppChanged |= ImGui::SliderFloat("Fog density##pp", &g_fogOptions.density, 0.0f, 0.5f);
    ppChanged |= ImGui::SliderFloat("Fog height falloff##pp", &g_fogOptions.heightFalloff, 0.0f, 5.0f);

    ImGui::Separator();
    ppChanged |= ImGui::Checkbox("Depth of Field##pp", &g_dofOptions.enabled);
    ppChanged |= ImGui::SliderFloat("DoF blur (CoC scale)##pp", &g_dofOptions.cocScale, 0.1f, 5.0f);

    ImGui::Separator();
    ppChanged |= ImGui::Checkbox("Temporal AA##pp", &g_taaOptions.enabled);

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
    ImGui::End();
}

void drawStatsPanel() {
    ImGui::Begin("Stats");
    ImGui::Text("%.0f FPS", ImGui::GetIO().Framerate);
    ImGui::Separator();
    ImGui::Text("Edit Mode: %s", g_editMode ? "ON" : "off");
    ImGui::Text("Selected model: %s", g_selectedModel ? g_selectedModel->name.c_str() : "(none)");
    ImGui::Text("Selected light: %s", g_selectedLight ? g_selectedLight->name.c_str() : "(none)");
    ImGui::Text("Dragging: %s", g_dragging ? "yes" : "no");
    ImGui::Separator();
    ImGui::TextUnformatted("CPU (Under Construction)");
    float const cpuUsagePercent = GetCPUUsagePercent();
    ImGui::Text("CPU Usage: %.2f%%", cpuUsagePercent);
    ImGui::End();
}

// First-launch panel arrangement, Blender-ish: a thin header strip on top, an Outliner on
// the left, Properties + Post Processing stacked on the right, Assets + Stats tabbed along
// the bottom, and the 3D viewport filling whatever's left in the middle (PassthruCentralNode
// means nothing is ever docked there, so the render just shows through). Only runs the very
// first time this dockspace has no split children yet - once imgui.ini has a real saved
// layout (from a previous run, or the user dragging panels around), this leaves it alone.
void buildDefaultDockLayout(ImGuiID dockspaceId) {
    ImGuiDockNode* node = ImGui::DockBuilderGetNode(dockspaceId);
    if (node != nullptr && node->IsSplitNode()) {
        return;
    }

    ImGui::DockBuilderRemoveNode(dockspaceId);
    ImGui::DockBuilderAddNode(dockspaceId,
            (ImGuiDockNodeFlags)((int)ImGuiDockNodeFlags_PassthruCentralNode | (int)ImGuiDockNodeFlags_DockSpace));
    ImGui::DockBuilderSetNodeSize(dockspaceId, ImGui::GetMainViewport()->Size);

    ImGuiID center = dockspaceId;
    ImGuiID const top = ImGui::DockBuilderSplitNode(center, ImGuiDir_Up, 0.06f, nullptr, &center);
    ImGuiID const left = ImGui::DockBuilderSplitNode(center, ImGuiDir_Left, 0.18f, nullptr, &center);
    ImGuiID right = ImGui::DockBuilderSplitNode(center, ImGuiDir_Right, 0.25f, nullptr, &center);
    ImGuiID const bottom = ImGui::DockBuilderSplitNode(center, ImGuiDir_Down, 0.28f, nullptr, &center);
    ImGuiID const rightBottom = ImGui::DockBuilderSplitNode(right, ImGuiDir_Down, 0.5f, nullptr, &right);

    ImGui::DockBuilderDockWindow("Toolbar", top);
    ImGui::DockBuilderDockWindow("Outliner", left);
    ImGui::DockBuilderDockWindow("Properties", right);
    ImGui::DockBuilderDockWindow("Post Processing", rightBottom);
    ImGui::DockBuilderDockWindow("Assets", bottom);
    ImGui::DockBuilderDockWindow("Stats", bottom);

    ImGui::DockBuilderFinish(dockspaceId);
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
    config.cameraMoveSpeed = g_cameraMoveSpeed;
    g_currentSkyboxPath = config.iblDirectory;

    DanteApp::get().run(
        config,
        [assetsDir](Engine* engine, View* view, Scene* scene) {
            g_scene = scene;
            g_mainView = view;
            g_engine = engine;

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

            static bool styleApplied = false;
            if (!styleApplied) {
                applyEditorStyle();
                styleApplied = true;
            }

            // Must run once per frame, right after ImGui::NewFrame() (which is exactly where
            // this callback fires - see ImGuiHelper::render), before any Manipulate() call.
            ImGuizmo::BeginFrame();

            // Main menu bar has to be drawn before DockSpaceOverViewport() below - it shrinks
            // the viewport's remaining work area for the rest of the frame, which is what
            // keeps the dockspace (and everything docked into it) from sitting underneath it.
            drawMainMenuBar(engine, assetsDir);

            // Enable docking so the panels below can be dragged into any arrangement instead
            // of sitting pinned in place - re-ORing an already-set flag every frame is a
            // no-op, so no need to gate this to run once.
            ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
            ImGuiID const dockspaceId =
                    ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
            buildDefaultDockLayout(dockspaceId);

            if (ImGui::IsKeyPressed(ImGuiKey_E, false)) {
                g_editMode = !g_editMode;
            }
            // Gizmo tool shortcuts (1/2/3 rather than the Blender/Unity W/E/R convention,
            // since E is already taken above for the edit-mode toggle).
            if (ImGui::IsKeyPressed(ImGuiKey_1, false)) g_gizmoOperation = ImGuizmo::TRANSLATE;
            if (ImGui::IsKeyPressed(ImGuiKey_2, false)) g_gizmoOperation = ImGuizmo::ROTATE;
            if (ImGui::IsKeyPressed(ImGuiKey_3, false)) g_gizmoOperation = ImGuizmo::SCALEU;

            if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Z, false)) {
                if (ImGui::GetIO().KeyShift) performRedo(); else performUndo();
            }
            if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Y, false)) {
                performRedo();
            }

            drawToolbarPanel();
            drawOutlinerPanel(engine, view);
            drawPropertiesPanel(engine);
            drawAssetsPanel(engine, view, assetsDir);
            drawPostProcessingPanel(view);
            drawStatsPanel();

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

            // -------- transform gizmo: drag handles for the selected model --------
            ImGuiIO& io = ImGui::GetIO();
            bool gizmoUsed = false;
            // Tracks one gizmo drag gesture across frames so the whole drag becomes exactly
            // one undo step, pushed when IsUsing() drops back to false, rather than one per
            // frame of mouse movement.
            static uint64_t sGizmoModelId = 0;
            static bool sGizmoWasUsing = false;
            static float3 sGizmoBeforePos, sGizmoBeforeRot;
            static float sGizmoBeforeScale = 1.0f;
            if (g_selectedModel && sGizmoModelId != 0 && sGizmoModelId != g_selectedModel->id) {
                // Selection changed without a clean drag-end (only reachable if something
                // external forced a reselect mid-drag) - drop the stale in-progress snapshot
                // rather than risk pushing a command built from mismatched before/after state.
                sGizmoWasUsing = false;
            }
            if (g_editMode && g_selectedModel && g_selectedModel->asset) {
                if (!sGizmoWasUsing) {
                    // Not dragging as of last frame - (re-)snapshot now, before Manipulate()
                    // below gets a chance to mutate anything, in case a drag starts this frame.
                    sGizmoModelId = g_selectedModel->id;
                    sGizmoBeforePos = g_selectedModel->position;
                    sGizmoBeforeRot = g_selectedModel->rotationDeg;
                    sGizmoBeforeScale = g_selectedModel->scale;
                }

                ImGuizmo::SetOrthographic(false);
                // No dedicated viewport window to draw into (the 3D render is composited
                // behind ImGui, not shown via an ImGui::Image) - the foreground drawlist
                // paints on top of everything without needing one.
                ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());
                ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

                mat4f const viewMatrix(view->getCamera().getViewMatrix());
                mat4f const projMatrix(view->getCamera().getProjectionMatrix());

                constexpr float kDegToRad = 0.017453292519943295f;
                mat4f const rotation =
                        mat4f::rotation(g_selectedModel->rotationDeg.x * kDegToRad, float3{1, 0, 0})
                        * mat4f::rotation(g_selectedModel->rotationDeg.y * kDegToRad, float3{0, 1, 0})
                        * mat4f::rotation(g_selectedModel->rotationDeg.z * kDegToRad, float3{0, 0, 1});
                mat4f gizmoMatrix = mat4f::translation(g_selectedModel->position) * rotation
                        * mat4f::scaling(float3{g_selectedModel->scale});

                if (ImGuizmo::Manipulate(&viewMatrix[0][0], &projMatrix[0][0], g_gizmoOperation,
                        ImGuizmo::LOCAL, &gizmoMatrix[0][0])) {
                    float translation[3], rotationDeg[3], scale[3];
                    ImGuizmo::DecomposeMatrixToComponents(&gizmoMatrix[0][0], translation, rotationDeg, scale);
                    g_selectedModel->position = float3{translation[0], translation[1], translation[2]};
                    g_selectedModel->rotationDeg = float3{rotationDeg[0], rotationDeg[1], rotationDeg[2]};
                    // SCALEU keeps x/y/z locked together already, but this still collapses
                    // them to one float rather than picking scale[0] alone, so floating-point
                    // drift between the three doesn't compound drag after drag.
                    g_selectedModel->scale = (scale[0] + scale[1] + scale[2]) / 3.0f;
                    g_selectedModel->applyTransform(*engine);
                }
                // IsUsing() stays true for the whole drag even on frames where the mouse has
                // moved past the gizmo's own screen hitbox - IsOver() alone would let a fast
                // drag "escape" onto the fallback click-pick/drag logic below mid-gesture.
                gizmoUsed = ImGuizmo::IsUsing();

                bool const gizmoActuallyChanged = sGizmoModelId == g_selectedModel->id
                        && (g_selectedModel->position != sGizmoBeforePos
                                || g_selectedModel->rotationDeg != sGizmoBeforeRot
                                || g_selectedModel->scale != sGizmoBeforeScale);
                if (sGizmoWasUsing && !gizmoUsed && gizmoActuallyChanged) {
                    auto cmd = std::make_unique<ModelTransformCommand>();
                    cmd->modelId = sGizmoModelId;
                    cmd->beforePos = sGizmoBeforePos;
                    cmd->beforeRot = sGizmoBeforeRot;
                    cmd->beforeScale = sGizmoBeforeScale;
                    cmd->afterPos = g_selectedModel->position;
                    cmd->afterRot = g_selectedModel->rotationDeg;
                    cmd->afterScale = g_selectedModel->scale;
                    pushCommand(std::move(cmd));
                }
                sGizmoWasUsing = gizmoUsed;
            }

            // -------- viewport picking / dragging --------
            bool const mouseOverPanel = io.WantCaptureMouse || ImGuizmo::IsOver() || gizmoUsed;

            // Snapshot for whichever click-drag gesture (below) is currently moving a model -
            // captured the instant g_dragging is armed, before any delta has been applied, and
            // consumed as one undo step on mouse-release. Position-only since this drag path
            // never touches rotation/scale (rotDeg/scale below just mirror the model's current
            // values so ModelTransformCommand's undo/redo is a no-op on those two fields).
            static uint64_t sViewportDragModelId = 0;
            static float3 sViewportDragBeforePos;

            // Picking is async (View::pick's result lands 1-2 frames after the query, not
            // the same frame as the click that triggered it) - so dragging is (re-)armed
            // here, once the result actually arrives, rather than only at click time. That
            // way a single click-and-hold gesture works on the very first click, instead of
            // needing an already-selected model from a prior click.
            if (g_pendingPickValid) {
                g_pendingPickValid = false;
                auto it = g_entityToModel.find(g_pendingPickEntity.getId());
                g_selectedModel = (it != g_entityToModel.end()) ? it->second : nullptr;
                if (g_selectedModel) {
                    // Selection is single-object across both models and lights (the
                    // Properties panel only has room to show one at a time) - clicking a
                    // model in the viewport should drop any light selected via the Outliner.
                    g_selectedLight = nullptr;
                }
                if (g_selectedModel && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                    g_dragging = true;
                    sViewportDragModelId = g_selectedModel->id;
                    sViewportDragBeforePos = g_selectedModel->position;
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
                        sViewportDragModelId = g_selectedModel->id;
                        sViewportDragBeforePos = g_selectedModel->position;
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
                    // Each wheel notch is its own discrete action rather than part of a
                    // continuous drag, so it gets its own undo step immediately - there's no
                    // separate "gesture end" event to defer it to like the drags above.
                    float3 const posBeforeScroll = g_selectedModel->position;
                    float3 const fwd = view->getCamera().getForwardVector();
                    g_selectedModel->position += fwd * io.MouseWheel * g_dragDistance * 0.08f;
                    g_selectedModel->applyTransform(*engine);

                    auto cmd = std::make_unique<ModelTransformCommand>();
                    cmd->modelId = g_selectedModel->id;
                    cmd->beforePos = posBeforeScroll;
                    cmd->beforeRot = cmd->afterRot = g_selectedModel->rotationDeg;
                    cmd->beforeScale = cmd->afterScale = g_selectedModel->scale;
                    cmd->afterPos = g_selectedModel->position;
                    pushCommand(std::move(cmd));
                }
            }

            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                if (g_dragging && g_selectedModel && sViewportDragModelId == g_selectedModel->id
                        && g_selectedModel->position != sViewportDragBeforePos) {
                    auto cmd = std::make_unique<ModelTransformCommand>();
                    cmd->modelId = sViewportDragModelId;
                    cmd->beforePos = sViewportDragBeforePos;
                    cmd->beforeRot = cmd->afterRot = g_selectedModel->rotationDeg;
                    cmd->beforeScale = cmd->afterScale = g_selectedModel->scale;
                    cmd->afterPos = g_selectedModel->position;
                    pushCommand(std::move(cmd));
                }
                g_dragging = false;
                sViewportDragModelId = 0;
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
        },
        DanteApp::PreRenderCallback(), DanteApp::PostRenderCallback(),
        1920, 1080);

    return 0;
}
