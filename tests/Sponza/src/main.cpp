#include <MSG/Animation.hpp>
#include <MSG/Assets/Asset.hpp>
#include <MSG/Assets/Parser.hpp>
#include <MSG/Assets/Parsers.hpp>
#include <MSG/Assets/Uri.hpp>
#include <MSG/ECS/Registry.hpp>
#include <MSG/Entity/Camera.hpp>
#include <MSG/Entity/Node.hpp>
#include <MSG/Entity/PunctualLight.hpp>
#include <MSG/FogArea.hpp>
#include <MSG/Image.hpp>
#include <MSG/ImageUtils.hpp>
#include <MSG/Keyboard/Events.hpp>
#include <MSG/Keyboard/Keyboard.hpp>
#include <MSG/Light/PunctualLight.hpp>
#include <MSG/Mouse/Events.hpp>
#include <MSG/Mouse/Mouse.hpp>
#include <MSG/Renderer.hpp>
#include <MSG/Renderer/RenderBuffer.hpp>
#include <MSG/Sampler.hpp>
#include <MSG/Scene.hpp>
#include <MSG/ShapeGenerator/Cube.hpp>
#include <MSG/Texture.hpp>
#include <MSG/TextureUtils.hpp>
#include <MSG/Tools/FPSCounter.hpp>
#include <MSG/Tools/ScopedTimer.hpp>
#include <MSG/Window/Events.hpp>
#include <MSG/Window/Window.hpp>

#include <filesystem>

using namespace Msg;

constexpr auto testWindowWidth  = 1280;
constexpr auto testWindowHeight = 720;
struct Args {
    Args(const int& argc, char const* argv[])
    {
        // for (int i = 1; i < argc; i++) {
        //     std::string arg = argv[i];
        //     if (arg == "-dirs") {
        //         i++;
        //         modelPath = std::filesystem::current_path() / "scenes" / "Dirs.gltf";
        //     } else if (arg == "-points") {
        //         i++;
        //         modelPath = std::filesystem::current_path() / "scenes" / "Points.gltf";
        //     } else if (arg == "-points2") {
        //         i++;
        //         modelPath = std::filesystem::current_path() / "scenes" / "Points2.gltf";
        //     } else if (arg == "-spots") {
        //         i++;
        //         modelPath = std::filesystem::current_path() / "scenes" / "Spots.gltf";
        //     } else if (arg == "--help") {
        //         PrintHelp();
        //         exit(0);
        //     }
        // }
        if (modelPath.empty()) {
            std::cerr << "Missing arguments." << std::endl;
            PrintHelp();
            exit(-1);
        }
        bool error = false;
        if (!std::filesystem::exists(modelPath)) {
            std::cerr << "Model " << modelPath << " not found." << std::endl;
            error = true;
        }
        if (error)
            exit(-1);
    }
    void PrintHelp()
    {
        std::cout << "Usage : \n"
                     " -spots\n"
                     " -dirs\n"
                     " -points\n"
                     " -ibl\n"
                  << std::endl;
    }
    std::filesystem::path modelPath  = std::filesystem::current_path() / "scene" / "Sponza.gltf";
    std::filesystem::path envPath    = std::filesystem::current_path() / "scene" / "env.jpg";
    std::filesystem::path skyboxPath = std::filesystem::current_path() / "scene" / "skybox.hdr";
};

template <typename Proj>
void UpdateCameraAspectRatio(Proj& a_Proj, const float& a_AspectRatio)
{
    errorFatal("UpdateCameraAspectRatio not implemented for this camera projection");
}

template <>
void UpdateCameraAspectRatio(CameraProjectionPerspective& a_Proj, const float& a_AspectRatio)
{
    a_Proj.aspectRatio = a_AspectRatio;
}

template <>
void UpdateCameraAspectRatio(CameraProjectionPerspectiveInfinite& a_Proj, const float& a_AspectRatio)
{
    a_Proj.aspectRatio = a_AspectRatio;
}

int main(int argc, char const* argv[])
{
    using namespace std::chrono_literals;
    auto args = Args(argc, argv);

    Renderer::CreateRendererInfo rendererInfo {
        .name               = "UnitTest",
        .applicationVersion = 100,
    };
    Renderer::RendererSettings rendererSettings {
        .internalResolution = 0.75f,
        .enableTAA          = true,
        .shadowQuality      = Renderer::QualitySetting::Medium,
        .volumetricFogRes   = Renderer::GetDefaultVolumetricFogRes(Renderer::QualitySetting::Medium),
        .mode               = Renderer::RendererMode::Deferred,
    };
    Renderer::CreateRenderBufferInfo renderBufferInfo {
        .width  = testWindowWidth,
        .height = testWindowHeight
    };
    Window::CreateWindowInfo windowInfo {
        .name   = "Msg::UnitTests::Sponza",
        .flags  = Window::FlagsResizableBits,
        .width  = testWindowWidth,
        .height = testWindowHeight,
        .vSync  = true
    };

    auto renderer     = Renderer::Create(rendererInfo, rendererSettings);
    auto window       = Window::Create(renderer, windowInfo);
    auto renderBuffer = Renderer::RenderBuffer::Create(renderer, renderBufferInfo);
    auto registry     = ECS::DefaultRegistry::Create();
    auto modelAsset   = std::make_shared<Assets::Asset>(args.modelPath);
    modelAsset->SetECSRegistry(registry);
    modelAsset->parsingOptions.mesh.generateLODs = false;
    std::shared_ptr<Scene> scene;
    std::shared_ptr<Animation> currentAnimation;
    Assets::InitParsers();
    {
        auto model        = Assets::Parser::Parse(modelAsset);
        auto parsedScenes = model->Get<Scene>();
        currentAnimation  = model->Get<Animation>().front();
        currentAnimation->Play();
        currentAnimation->SetLoop(true);
        currentAnimation->SetLoopMode(Animation::LoopMode::Repeat);
        scene = parsedScenes.front();
        scene->SetBackgroundColor({ 0, 0, 0 });
    }
    for (auto [entity, name, lightData] : registry->GetView<Core::Name, PunctualLight>()) {
        auto shadowSettings       = lightData.GetShadowSettings();
        shadowSettings.castShadow = true;
        if (std::string(name) == "Sun") {
            shadowSettings.resolution = 1024;
            shadowSettings.blurRadius = 5.f;
            lightData.SetPriority(1000);
            lightData.SetIntensity(10);
        }
        lightData.SetShadowSettings(shadowSettings);
    }

    auto [entity, camera] = *registry->GetView<Camera>().begin();
    scene->SetCamera(registry->GetEntityRef(entity));
    FogArea fogArea;
    fogArea.SetOp(FogAreaOp::Replace);
    fogArea.SetScattering({ 1.f, 1.f, 1.f });
    fogArea.SetAttenuationExp(1 / 5.f);
    fogArea.SetExtinction(0.1f);
    fogArea.emplace_back(Msg::Cube(glm::vec3(0, 0, 0), glm::vec3(100, 0.5, 100)));
    scene->GetRootEntity().AddComponent<FogArea>(fogArea);

    {
        auto envAsset = std::make_shared<Assets::Asset>(args.envPath);
        envAsset->SetECSRegistry(registry);
        envAsset->parsingOptions = {
            .image { .maxPixelValue = 1000.f }
        };
        auto env          = Assets::Parser::Parse(envAsset);
        auto parsedImages = env->GetCompatible<Image>();
        if (!parsedImages.empty()) {
            const auto& parsedImage = parsedImages.front();
            auto cubemap            = CubemapFromEqui(
                parsedImage->GetPixelDescriptor(),
                512, 512, *parsedImage);
            TextureSampler skybox;
            skybox.texture = std::make_shared<Texture>(TextureType::TextureCubemap, std::make_shared<Image>(cubemap));
            TextureGenerateMipmaps(*skybox.texture);
            auto lightIBLEntity = Entity::PunctualLight::Create(registry);
            auto& lightIBLComp  = lightIBLEntity.GetComponent<PunctualLight>();
            LightIBL lightIBLData({ 64, 64 }, skybox.texture);
            lightIBLData.intensity = 1;
            lightIBLComp           = lightIBLData;
            scene->AddEntity(lightIBLEntity);
        }
    }

    {
        auto envAsset = std::make_shared<Assets::Asset>(args.skyboxPath);
        envAsset->SetECSRegistry(registry);
        envAsset->parsingOptions = {
            .image { .maxPixelValue = 1000.f }
        };
        auto env          = Assets::Parser::Parse(envAsset);
        auto parsedImages = env->GetCompatible<Image>();
        if (!parsedImages.empty()) {
            const auto& parsedImage = parsedImages.front();
            auto cubemap            = std::make_shared<Image>(CubemapFromEqui(
                parsedImage->GetPixelDescriptor(),
                512, 512, *parsedImage));
            TextureSampler skybox;
            skybox.texture = std::make_shared<Texture>(TextureType::TextureCubemap, cubemap);
            scene->SetSkybox(skybox);
        }
    }

    Events::BindCallback(EventWindowResized::Type,
        [&renderer, &renderBuffer, &camera](const Event& a_Event, const EventBindingID&, std::any) {
            auto& windowResizedEvent = reinterpret_cast<const EventWindowResized&>(a_Event);
            auto aspectRatio         = windowResizedEvent.width / float(windowResizedEvent.height);
            renderBuffer             = Renderer::RenderBuffer::Create(renderer, { windowResizedEvent.width, windowResizedEvent.height });
            std::visit([aspectRatio](auto& a_Data) { UpdateCameraAspectRatio(a_Data, aspectRatio); }, camera.projection);
            camera.projection.UpdateMatrix();
            Renderer::SetActiveRenderBuffer(renderer, renderBuffer);
        });
    {
        Tools::ScopedTimer timer("Loading Test Scene");
        Renderer::SetActiveScene(renderer, scene.get());
        Renderer::Load(renderer, *scene);
    }
    Renderer::SetActiveRenderBuffer(renderer, renderBuffer);
    Renderer::Update(renderer);
    Window::Show(window);
    FPSCounter fpsCounter;
    auto lastTime   = std::chrono::high_resolution_clock::now();
    auto printTime  = lastTime;
    auto updateTime = lastTime;
    while (true) {
        Events::Update();
        Events::Consume();
        if (Window::IsClosing(window))
            break;
        const auto now   = std::chrono::high_resolution_clock::now();
        const auto delta = std::chrono::duration<double, std::milli>(now - lastTime).count();
        lastTime         = now;
        fpsCounter.StartFrame();
        auto updateDelta = std::chrono::duration<double, std::milli>(now - updateTime).count();
        if (updateDelta >= 15) {
            updateTime = now;
            currentAnimation->Advance(updateDelta / 1000.f);
            scene->Update();
            Renderer::Update(renderer);
            Renderer::Render(renderer);
        }
        Window::Present(window, renderBuffer);
        fpsCounter.EndFrame();
        if (std::chrono::duration<double, std::milli>(now - printTime).count() >= 48) {
            printTime = now;
            fpsCounter.Print();
        }
    }
    Renderer::Unload(renderer, *scene);
    return 0;
}
