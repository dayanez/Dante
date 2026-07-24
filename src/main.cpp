 #include <danteapp/DanteApp.h>
#include <dante/Camera.h>
#include <dante/ColorGrading.h>
#include <dante/Engine.h>
#include <dante/LightManager.h>
#include <dante/RenderableManager.h>
#include <dante/Scene.h>
#include <dante/ToneMapper.h>
#include <dante/TransformManager.h>
#include <dante/View.h>
#include <backend/BufferDescriptor.h>
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
#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <vector>
#include <iostream>

//TODO ADD FUNCTIONING PFX SLIDER


//ALSO CHANGE LIGHTING || MAKE NEG LIGHTING -100

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
struct GltfModel {
    gltfio::AssetLoader* loader = nullptr;
    gltfio::MaterialProvider* materials = nullptr;
    gltfio::TextureProvider* textureDecoder = nullptr;
    gltfio::ResourceLoader* resourceLoader = nullptr;
    gltfio::DanteAsset* asset = nullptr;
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
    config.title = "DanteEngine";
#if defined(__APPLE__)
    config.backend = Engine::Backend::METAL;
#else
    config.backend = Engine::Backend::OPENGL;
#endif
    config.iblDirectory = (assetsDir + "environments/qwantani_night_puresky_4k.exr").getAbsolutePath();
    config.cameraMode = camutils::Mode::FREE_FLIGHT;

    DanteApp::get().run(
        config,
        [assetsDir](Engine* engine, View* view, Scene* scene) {

            view->setAntiAliasing(AntiAliasing::NONE);
            TemporalAntiAliasingOptions taaOptions;
            taaOptions.enabled = false;
            view->setTemporalAntiAliasingOptions(taaOptions);

            BloomOptions bloomOptions;
            bloomOptions.enabled = true;
            bloomOptions.strength = 0.04f;
            view->setBloomOptions(bloomOptions);


            AmbientOcclusionOptions aoOptions;
            aoOptions.enabled = false;
            view->setAmbientOcclusionOptions(aoOptions);

            //sunlight
            //view->getCamera().setExposure(16.0f, 1.0f / 50.0f, 70.0f);                                             //starts here
            view->getCamera().setExposure(16.0f, 1.0f / 125.0f, 100.0f);

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
            fogOptions.enabled = true;
            fogOptions.distance = 5.0f;
            fogOptions.density = 0.02f;
            fogOptions.fogColorFromIbl = true;
            view->setFogOptions(fogOptions);

            //Vigshit
            VignetteOptions vignetteOptions;
            vignetteOptions.enabled = false; //Disabling because right now, it looks ugly no point in having it.
            view->setVignetteOptions(vignetteOptions);

            // Skybox and indirect light both come from config.iblDirectory - DanteApp
            // loads and sets them on the scene before this callback runs (see
            // DanteApp::loadIBL), so the night-sky EXR is already visible here.

            // Note: DanteApp::doFrame() re-derives Camera::lookAt() from the camera
            // manipulator every frame, so a one-time Camera::lookAt() call here would be
            // overwritten on the next frame - the manipulator's own state is what actually
            // needs to change, which Config doesn't expose.
            g_character.create(*engine, *scene, assetsDir + "models/character/ch15_firing.glb",
                    float3{0.2, -1.0f, 1.7});                                                              //im assuming that xy might be in "g_character" ill try adjusting float3
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
                    
            //Im thinking that now what i do is print Waiting Press: then type key gives option to change hardcoded value, and enable true or false. then repeat 
            
            
            
            // Making Control for postfx
            
            /*
            So step 1: 
                Make commands a struct we will typedef it too so its easier to call
                
            So step 2: 
                make Lambda which is essentially a func inside a func and we ill use that for the logic below 
                
                since we are using |using namespace std;| no std::string message 
                
                LoL review this tomorrow too didnt know you can make strings like string str = "Hello"; my dumbass thought it was like fucking C where its char str[256] = "Hello";
                
                Example of Lambda Syntax: 
                
                auto print_message = [](std::string message)
                {
                
                    std::cout <<message<< "\n";
                };
                for (int i = 0; i<10; i++){
                print_message("Hello!");
                }
            
            */
            
            
            //Thinking we turn this into a lamb
            //COMMANDS
            string v; 
            string s; 
            //================================
            //Inital Waiting 
            cout <<"Waiting KeyPress: "<<endl;
            getline(cin, v, s);
            
            if (v == "v" || "V"){
                //Logic for pfx would go here
                cout<<"v test worked"<< endl;
            
            };
            
            if (s == "s"||"S"){
                cout << "s test worked"<< endl;
                
             
            };
             
                
           
            
           
            

            // Advance every model's animation each frame (a model with no animations is a
            // no-op via the animationCount() == 0 check).
            DanteApp::get().animate([](Engine*, View*, double now) {
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
        
        /*
        Deciding for now at least before button is added. We will use terminal to input keybinds and then will prompt user to insert custom value
        Then this will change whatever pfx is called. This makes dev a whole lot easier doesnt need to be QOL right now because its just me and ik the
        keybinds. 
        */
      
        //Engine and View need to be parameters we are pulling logic from them
        
     
        [](Engine* engine, View*, Scene* scene) {
            g_character.destroy(*engine, *scene);
            g_bathroom.destroy(*engine, *scene);
            g_smileyMonster.destroy(*engine, *scene);
            scene->remove(g_sun);
            engine->destroy(g_sun);
            utils::EntityManager::get().destroy(g_sun);
            engine->destroy(g_colorGrading);
        },
        //Need to make another gui window
        [](Engine*, View*) {
            ImGui::SetNextWindowPos(ImVec2(10,40), ImGuiCond_Always);
            ImGui::SetNextWindowBgAlpha(0.35f);
            ImGui::Begin("PFX Control Check Term", nullptr,
                    ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                    ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
                    ImGuiWindowFlags_NoNav | ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::Text("PFX Control Check Term");
            ImGui::End();
            
            //--------------------------
            ImGui::SetNextWindowPos(ImVec2(10,10), ImGuiCond_Always);
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


//Start in main.cpp — no engine internals required, fastest feedback loop. Your Options structs (BloomOptions,
  //AmbientOcclusionOptions, etc.) are already there but hardcoded. First real task: add an ImGui panel (you already have
  //one for FPS, imgui.h is already included) with sliders/checkboxes wired to those structs, so you can drag bloom
  //strength or toggle SSAO at runtime instead of recompiling. Small scope, touches real C++ (structs, pointers to View),
  //and gives you an actual dev tool you'll use for everything after.

  //Then add something to the scene using patterns you've already read. You've got GltfModel::create() as a template — use
  //it to load a 4th model yourself. Or add a second light: build a flickering point light near the monster with
  //LightManager::Builder, following the exact lifecycle g_sun already uses (create entity → build → add to scene →
  //destroy on teardown). This is where you start writing rather than just calling — same APIs, your own logic (e.g.
  //animate its intensity per-frame for a flicker, which means touching the animate() callback and doing real per-frame
  //math).

  //Then write an actual shader — a new .mat file. This is the part you actually asked about. Filament's material files
  //are a good on-ramp because the shading language is a constrained subset with a clear structure (you don't need to know
 // a full GLSL pipeline to start). Concrete first shader: a simple emissive rim-light/fresnel glow on the smiley monster
 // — visually fun, fits the horror-game aesthetic, and is genuinely "I wrote a shader" rather than "I configured one."
 // Look at one of the simpler existing .mat files (clearDepth.mat or blitLow.mat) for the file structure before
 // attempting your own — start from the simplest real example in the repo, not a blank file.

 // Capstone, once the above feels comfortable: add a new pass to the frame graph. Something like a custom vignette or
 // chromatic-aberration post effect as a new FrameGraphPass in fg/, with its own .mat. This is real engine-level graphics
 // programming — declaring a pass, its inputs/outputs, and the shader that runs in it — but it's only approachable after
 // you've seen how FrameGraph.cpp/PassNode.cpp and a couple of existing post-process materials fit together.
