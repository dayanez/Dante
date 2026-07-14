#include <filamentapp/FilamentApp.h>

#include <filament/Engine.h>
#include <filament/IndexBuffer.h>
#include <filament/Material.h>
#include <filament/MaterialInstance.h>
#include <filament/RenderableManager.h>
#include <filament/Scene.h>
#include <filament/VertexBuffer.h>
#include <filament/View.h>

#include <geometry/SurfaceOrientation.h>

#include <utils/EntityManager.h>

#include <imgui.h>

using namespace filament;
using namespace filament::math;

namespace {

// A flat 40x40 quad on the XZ plane. Lit shading needs a packed tangent-frame quaternion
// (not just a raw normal) per vertex, which SurfaceOrientation computes for us.
struct GroundPlane {
    VertexBuffer* vertexBuffer = nullptr;
    IndexBuffer* indexBuffer = nullptr;
    MaterialInstance* materialInstance = nullptr;
    utils::Entity entity;

    void create(Engine& engine, Material const* material) {
        // Sits below the camera manipulator's default eye height (y=0) so it's visible
        // below the horizon rather than edge-on to it.
        static const float3 positions[] = {
            {-20, -1.6f, -20},
            { 20, -1.6f, -20},
            {-20, -1.6f,  20},
            { 20, -1.6f,  20},
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
                .boundingBox({{0, -1.6f, 0}, {20, 0.01f, 20}})
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

} // namespace

// M2: skybox (IBL from a CC0 HDRI, loaded automatically by FilamentApp via
// Config::iblDirectory), a lit ground plane, a free-flying camera, and an FPS counter
// drawn through filagui. Animated models come in M4.
int main() {
    Config config;
    config.title = "Dante";
    config.backend = Engine::Backend::VULKAN;
    config.iblDirectory = DANTE_ASSETS_DIR "/environments/flower_road_2k.hdr";
    config.cameraMode = camutils::Mode::FREE_FLIGHT;

    FilamentApp::get().run(
        config,
        [](Engine* engine, View*, Scene* scene) {
            // Note: FilamentApp::doFrame() re-derives Camera::lookAt() from the camera
            // manipulator every frame, so a one-time Camera::lookAt() call here would be
            // overwritten on the next frame - the manipulator's own state is what actually
            // needs to change, which Config doesn't expose. The ground plane is positioned
            // below the manipulator's default eye height instead (see GroundPlane::create).
            g_ground.create(*engine, FilamentApp::get().getDefaultMaterial());
            scene->addEntity(g_ground.entity);
        },
        [](Engine* engine, View*, Scene* scene) {
            scene->remove(g_ground.entity);
            g_ground.destroy(*engine);
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
