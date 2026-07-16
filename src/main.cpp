#include <filamentapp/FilamentApp.h>

#include <filament/Camera.h>
#include <filament/ColorGrading.h>
#include <filament/Engine.h>
#include <filament/IndexBuffer.h>
#include <filament/LightManager.h>
#include <filament/Material.h>
#include <filament/MaterialInstance.h>
#include <filament/RenderableManager.h>
#include <filament/Scene.h>
#include <filament/Skybox.h>
#include <filament/ToneMapper.h>
#include <filament/TransformManager.h>
#include <filament/VertexBuffer.h>
#include <filament/View.h>

#include <backend/BufferDescriptor.h>

#include <geometry/SurfaceOrientation.h>

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

// A flat 16x16 "stage" quad on the XZ plane, small enough that its edges stay in view
// against the sky instead of reading as an endless ground plane. Lit shading needs a
// packed tangent-frame quaternion (not just a raw normal) per vertex, which
// SurfaceOrientation computes for us.
struct GroundPlane {
    VertexBuffer* vertexBuffer = nullptr;
    IndexBuffer* indexBuffer = nullptr;
    MaterialInstance* materialInstance = nullptr;
    utils::Entity entity;

    void create(Engine& engine, Material const* material) {
        // Sits below the camera manipulator's default eye height (y=0) so it's visible
        // below the horizon rather than edge-on to it.
        static const float3 positions[] = {
            {-8, -1.0f, -8},
            { 8, -1.0f, -8},
            {-8, -1.0f,  8},
            { 8, -1.0f,  8},
        };
        static const float3 normals[] = { {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0} };
        // Winding matters here: aiDefaultMat (lit) uses Filament's default backface culling,
        // unlike the debug materials which disable it - the other winding order rendered
        // nothing at all with no error, which is what made this one hard to isolate.
        static const uint16_t indices[] = { 0, 2, 1, 2, 3, 1 };

        // Must be static: VertexBuffer::BufferDescriptor without a callback can be consumed
        // asynchronously on the render thread, so a plain stack array here would go out of
        // scope before the upload actually happens (this was the bug that made the lit
        // version of this plane invisible while the unlit debug version rendered fine).
        static short4 tangents[4];
        auto* orientation = geometry::SurfaceOrientation::Builder()
                .vertexCount(4)
                .normals(normals)
                .build();
        orientation->getQuats(tangents, 4);
        delete orientation;

        vertexBuffer = VertexBuffer::Builder()
                .vertexCount(4)
                .bufferCount(2)
                .attribute(VertexAttribute::POSITION, 0, VertexBuffer::AttributeType::FLOAT3)
                .attribute(VertexAttribute::TANGENTS, 1, VertexBuffer::AttributeType::SHORT4)
                .normalized(VertexAttribute::TANGENTS)
                .build(engine);
        vertexBuffer->setBufferAt(engine, 0,
                VertexBuffer::BufferDescriptor(positions, sizeof(positions), nullptr));
        vertexBuffer->setBufferAt(engine, 1,
                VertexBuffer::BufferDescriptor(tangents, sizeof(tangents), nullptr));

        indexBuffer = IndexBuffer::Builder()
                .indexCount(6)
                .bufferType(IndexBuffer::IndexType::USHORT)
                .build(engine);
        indexBuffer->setBuffer(engine,
                IndexBuffer::BufferDescriptor(indices, sizeof(indices), nullptr));

        materialInstance = material->createInstance();
        // A light, fully-rough gray reads as near-white under this HDRI's daylight-level
        // ambient (~30,000 lux) once tonemapped, which looked like a blown-out white slab
        // rather than a platform. Darker and slightly less rough reads as concrete/stone.
        materialInstance->setParameter("baseColor", RgbType::LINEAR, float3{0.2f, 0.19f, 0.18f});
        materialInstance->setParameter("metallic", 0.0f);
        materialInstance->setParameter("roughness", 0.8f);
        materialInstance->setParameter("reflectance", 0.04f);

        entity = utils::EntityManager::get().create();
        RenderableManager::Builder(1)
                .boundingBox({{0, -1.0f, 0}, {8, 0.01f, 8}})
                .material(0, materialInstance)
                .geometry(0, RenderableManager::PrimitiveType::TRIANGLES, vertexBuffer, indexBuffer, 0, 6)
                .culling(false)
                // FilamentApp's main view calls setVisibleLayers(0x4, 0x4) - renderables
                // default to a different layer and are silently invisible without this.
                .layerMask(0x4, 0x4)
                .build(engine, entity);
    }

    void destroy(Engine& engine) {
        engine.destroy(entity);
        engine.destroy(materialInstance);
        engine.destroy(vertexBuffer);
        engine.destroy(indexBuffer);
        utils::EntityManager::get().destroy(entity);
    }
};

GroundPlane g_ground;
Skybox* g_skybox = nullptr;
utils::Entity g_sun;
ColorGrading* g_colorGrading = nullptr;

// An animated glTF character loaded via gltfio, playing a looping baked-in animation
// (see tools/convert_character.py - a Blender script that copies a Mixamo animation clip
// onto this character's identically-named-bone skeleton and exports both as one glb).
struct CharacterModel {
    gltfio::AssetLoader* loader = nullptr;
    gltfio::MaterialProvider* materials = nullptr;
    gltfio::TextureProvider* textureDecoder = nullptr;
    gltfio::ResourceLoader* resourceLoader = nullptr;
    gltfio::FilamentAsset* asset = nullptr;
    gltfio::Animator* animator = nullptr;

    void create(Engine& engine, Scene& scene, utils::Path const& gltfPath) {
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
                // gltfio's own per-primitive bounding boxes were degenerate for the Blender-
                // exported character (see asset->getBoundingBox() in the log line below - a
                // ~1.8cm box for a life-sized human), which made Filament's frustum culling
                // silently discard the geometry as "outside the view" even though it wasn't.
                // Disabling culling costs nothing meaningful at this scene's scale.
                rm.setCulling(instance, false);
            }
        }

        scene.addEntities(asset->getEntities(), asset->getEntityCount());

        auto& tm = engine.getTransformManager();
        auto root = tm.getInstance(asset->getRoot());
        tm.setTransform(root, mat4f::translation(float3{0, -1.0f, -2}));

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

CharacterModel g_character;

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

// M4: an animated glTF character (see CharacterModel), on top of the M2 skybox/ground
// plane/camera/FPS overlay.
int main() {
    utils::Path const assetsDir = resolveAssetsDir();

    Config config;
    config.title = "Dante";
#if defined(__APPLE__)
    config.backend = Engine::Backend::METAL;
#else
    config.backend = Engine::Backend::OPENGL;
#endif
    config.iblDirectory = (assetsDir + "environments/flower_road_2k.hdr").getAbsolutePath();
    config.cameraMode = camutils::Mode::FREE_FLIGHT;

    FilamentApp::get().run(
        config,
        [assetsDir](Engine* engine, View* view, Scene* scene) {
            // FXAA (View's default) softens the whole frame, not just jagged edges - that
            // was noticeable when the skybox still showed the HDRI photo directly. Now that
            // the skybox is a flat color (see below) there's no photo detail left to soften,
            // and there IS real geometry (the character) with edges worth smoothing, so
            // temporal AA replaces the flat NONE from before.
            view->setAntiAliasing(AntiAliasing::NONE);
            TemporalAntiAliasingOptions taaOptions;
            taaOptions.enabled = true;
            view->setTemporalAntiAliasingOptions(taaOptions);

            // Subtle bloom on overbright highlights (the sky/IBL reflections) - strength is
            // deliberately low so it reads as a glow, not a haze over the whole frame.
            BloomOptions bloomOptions;
            bloomOptions.enabled = true;
            bloomOptions.strength = 0.08f;
            view->setBloomOptions(bloomOptions);

            // Screen-space ambient occlusion: mainly grounds the character against the
            // plane with contact shadowing that the single IBL alone doesn't provide.
            AmbientOcclusionOptions aoOptions;
            aoOptions.enabled = true;
            view->setAmbientOcclusionOptions(aoOptions);

            // f/8, 1/125s, ISO 100: two stops brighter than a literal sunny-16 exposure.
            // Sunny-16 (f/16) is the textbook-correct exposure for direct real sunlight, but
            // it read as near-silhouetted here - this character's materials (dark tactical
            // gear, low base reflectance) don't have the wide dynamic range a real outdoor
            // photo's mix of surfaces would, so the mathematically "correct" exposure just
            // wasn't a good exposure for this particular scene. This aperture number is also
            // what the depth-of-field cocScale below is keyed to - change one, change both.
            view->getCamera().setExposure(8.0f, 1.0f / 125.0f, 100.0f);

            // A shadow-casting sun. This is the only light in the scene besides the IBL -
            // gltfio already sets castShadows/receiveShadows(true) on the character by
            // default (see AssetLoader.cpp), and RenderableManager defaults receiveShadows
            // to true, so the ground plane picks up the shadow with no extra code. Filament
            // defaults to PCF shadow maps (View::setShadowType); soft/VSM shadows are a
            // later tuning knob, not needed to get shadows working at all.
            g_sun = utils::EntityManager::get().create();
            LightManager::Builder(LightManager::Type::SUN)
                    .color({0.98f, 0.95f, 0.92f})
                    .intensity(100000.0f) // lux - real midday sun, matches the sunny-16 exposure above
                    .direction(normalize(float3{-0.6f, -1.0f, -0.75f}))
                    .castShadows(true)
                    .sunAngularRadius(0.545f) // real sun's angular size in degrees
                    .build(*engine, g_sun);
            scene->addEntity(g_sun);

            // ACES tone mapping instead of Filament's default (ACESLegacyToneMapper) - a
            // deliberate choice rather than the default, not a correctness fix. ColorGrading
            // only needs the ToneMapper object during this synchronous build() call, so a
            // stack-local instance is enough; the resulting ColorGrading object itself is
            // what has to outlive the frame and gets torn down in the cleanup callback below.
            ACESToneMapper toneMapper;
            g_colorGrading = ColorGrading::Builder()
                    .toneMapper(&toneMapper)
                    .build(*engine);
            view->setColorGrading(g_colorGrading);

            // Depth of field, focused roughly on the character. cocScale lets DoF blur be
            // tuned independently of the camera's actual aperture (cocScale = cameraAperture
            // / desiredDoFAperture, per Options.h). Kept deliberately subtle (~f/5.6-equivalent
            // instead of ~f/2.8) - the free-fly camera means there's no fixed, known
            // camera-to-character distance to focus on precisely, so a strong blur risks
            // softening the subject itself whenever that guess is off, not just the
            // background. This is the one setting here that genuinely wants interactive
            // tuning once you're driving the camera yourself.
            view->getCamera().setFocusDistance(3.0f);
            DepthOfFieldOptions dofOptions;
            dofOptions.enabled = true;
            dofOptions.cocScale = 2.0f / 1.6f;
            view->setDepthOfFieldOptions(dofOptions);

            // Faint atmospheric fog for depth cueing. Starts past the character/ground
            // (distance) so it doesn't wash out the nearby ground - and with it, any shadow
            // landing there - which a closer start distance did. Sampling color from the IBL
            // (fogColorFromIbl) keeps it consistent with whatever environment is loaded.
            FogOptions fogOptions;
            fogOptions.enabled = false;
            fogOptions.distance = 10.0f;
            fogOptions.density = 0.03f;
            fogOptions.fogColorFromIbl = true;
            view->setFogOptions(fogOptions);

            // Subtle vignette - defaults are a mild, standard photographic falloff.
            VignetteOptions vignetteOptions;
            vignetteOptions.enabled = false; //Disabling because right now, it looks ugly no point in having it. 
            view->setVignetteOptions(vignetteOptions);

            // Replace the HDRI's photographic background with a plain clear-sky color.
            // Config::iblDirectory still loads that HDRI for indirect lighting (ambient/
            // reflections), which we keep - only the visible skybox is swapped out here,
            // since Scene's skybox and indirect light are independent of each other.
            g_skybox = Skybox::Builder().color({0.15f, 0.4f, 0.85f, 1.0f}).build(*engine);
            scene->setSkybox(g_skybox);

            // Note: FilamentApp::doFrame() re-derives Camera::lookAt() from the camera
            // manipulator every frame, so a one-time Camera::lookAt() call here would be
            // overwritten on the next frame - the manipulator's own state is what actually
            // needs to change, which Config doesn't expose. The ground plane is positioned
            // below the manipulator's default eye height instead (see GroundPlane::create).
            g_ground.create(*engine, FilamentApp::get().getDefaultMaterial());
            scene->addEntity(g_ground.entity);

            g_character.create(*engine, *scene, assetsDir + "models/character/ch15_firing.glb");

            // Advance the character's animation each frame.
            FilamentApp::get().animate([](Engine*, View*, double now) {
                if (!g_character.animator || g_character.animator->getAnimationCount() == 0) {
                    return;
                }
                static double startTime = now;
                float duration = g_character.animator->getAnimationDuration(0);
                float elapsed = duration > 0.0f ? fmodf((float)(now - startTime), duration) : 0.0f;
                g_character.animator->applyAnimation(0, elapsed);
                g_character.animator->updateBoneMatrices();
            });
        },
        [](Engine* engine, View*, Scene* scene) {
            g_character.destroy(*engine, *scene);
            scene->remove(g_ground.entity);
            g_ground.destroy(*engine);
            engine->destroy(g_skybox);
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
