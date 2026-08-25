
#ifndef DANTEGUI_IMGUIHELPER_H_
#define DANTEGUI_IMGUIHELPER_H_

#include <dante/Engine.h>
#include <dante/IndexBuffer.h>
#include <dante/Material.h>
#include <dante/MaterialInstance.h>
#include <dante/Texture.h>
#include <dante/TextureSampler.h>
#include <dante/VertexBuffer.h>
#include <dante/View.h>

#include <utils/Entity.h>
#include <utils/Path.h>

#include <functional>
#include <vector>
#include <unordered_set>

struct ImDrawData;
struct ImGuiIO;
struct ImGuiContext;

namespace dantegui {

// Translates ImGui's draw commands into Dante primitives, textures, vertex buffers, etc.
// Creates a UI-specific Scene object and populates it with a Renderable. Does not handle
// event processing; clients can simply call ImGui::GetIO() directly and set the mouse state.
class UTILS_PUBLIC ImGuiHelper {
public:
    // Using std::function instead of a vanilla C callback to make it easy for clients to pass in
    // lambdas that have captures.
    using Callback = std::function<void(dante::Engine*, dante::View*)>;

    // The constructor creates its own Scene and places it in the given View.
    ImGuiHelper(dante::Engine* engine, dante::View* view, const utils::Path& fontPath,
            ImGuiContext* imGuiContext = nullptr);
    ~ImGuiHelper();

    // Informs ImGui of the current display size, as well as a scaling factor when scissoring.
    void setDisplaySize(int width, int height, float scaleX = 1.0f,
            float scaleY = 1.0f, bool flipVertical = false);

    // High-level utility method that takes a callback for creating all ImGui windows and widgets.
    // Clients are responsible for rendering the View. This should be called on every frame,
    // regardless of whether the Renderer wants to skip or not.
    void render(float timeStepInSeconds, Callback imguiCommands);

    // Low-level alternative to render() that consumes an ImGui command list and translates it into
    // various Dante calls. This includes updating the vertex buffer, setting up material
    // instances, and rebuilding the Renderable component that encompasses the entire UI. Since this
    // makes Dante calls, it must be called from the main thread.
    void processImGuiCommands(ImDrawData* commands, const ImGuiIO& io);

    // Helper method called after resolving fontPath; public so fonts can be added by caller.
    void createAtlasTexture(dante::Engine* engine);

    // Returns the client-owned view, useful for drawing 2D overlays.
    dante::View* getView() const { return mView; }

  private:
      void createBuffers(int numRequiredBuffers);
      void populateVertexData(size_t bufferIndex, size_t vbSizeInBytes, void* vbData,
                  size_t ibSizeInBytes, void* ibData);
      void createVertexBuffer(size_t bufferIndex, size_t capacity);
      void createIndexBuffer(size_t bufferIndex, size_t capacity);
      void syncThreads();
      dante::Engine* mEngine;
      dante::View* mView; // The view is owned by the client.
      dante::Scene* mScene;
      dante::Material* mMaterial2d = nullptr;
      std::vector<dante::MaterialInstance*> mMaterial2dInstances;
#ifdef __ANDROID__
      dante::Material* mMaterialExternal = nullptr;
      std::vector<dante::MaterialInstance*> mMaterialExternalInstances;
#endif
      dante::Camera* mCamera = nullptr;
      std::vector<dante::VertexBuffer*> mVertexBuffers;
      std::vector<dante::IndexBuffer*> mIndexBuffers;
      utils::Entity mRenderable;
      utils::Entity mCameraEntity;
      dante::Texture* mTexture = nullptr;
      bool mHasSynced = false;
      ImGuiContext* mImGuiContext;
      dante::TextureSampler mSampler;
      bool mFlipVertical = false;
      utils::Path mSettingsPath;
      std::unordered_set<dante::Texture*> mImGuiTextures;
};

} // namespace dantegui

#endif /* DANTEGUI_IMGUIHELPER_H_ */
