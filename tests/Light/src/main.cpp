#include <MSG/Animation.hpp>
#include <MSG/Assets/Asset.hpp>
#include <MSG/Assets/Parser.hpp>
#include <MSG/Assets/Parsers.hpp>
#include <MSG/Assets/Uri.hpp>
#include <MSG/ECS/Registry.hpp>
#include <MSG/Entity/Camera.hpp>
#include <MSG/Entity/Node.hpp>
#include <MSG/Entity/PunctualLight.hpp>
#include <MSG/Image.hpp>
#include <MSG/Image/Cubemap.hpp>
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
#include <MSG/Tools/FPSCounter.hpp>
#include <MSG/Tools/ScopedTimer.hpp>
#include <MSG/Window/Events.hpp>
#include <MSG/Window/Window.hpp>

#include <filesystem>

using namespace MSG;

constexpr auto testWindowWidth  = 1280;
constexpr auto testWindowHeight = 720;
struct Args {
    Args(const int& argc, char const* argv[])
    {
        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            if (arg == "-dirs") {
                i++;
                modelPath = std::filesystem::current_path() / "scenes" / "Dirs.gltf";
            } else if (arg == "-points") {
                i++;
                modelPath = std::filesystem::current_path() / "scenes" / "Points.gltf";
            } else if (arg == "-points2") {
                i++;
                modelPath = std::filesystem::current_path() / "scenes" / "Points2.gltf";
            } else if (arg == "-spots") {
                i++;
                modelPath = std::filesystem::current_path() / "scenes" / "Spots.gltf";
            } else if (arg == "--help") {
                PrintHelp();
                exit(0);
            }
        }
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
    std::filesystem::path modelPath;
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
        .enableTAA = true,
        .mode      = Renderer::RendererMode::Deferred
    };
    Renderer::CreateRenderBufferInfo renderBufferInfo {
        .width  = testWindowWidth,
        .height = testWindowHeight
    };
    Window::CreateWindowInfo windowInfo {
        .name   = "MSG::UnitTests::Light",
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
    for (auto [entity, lightData] : registry->GetView<PunctualLight>()) {
        auto shadowSettings       = lightData.GetShadowSettings();
        shadowSettings.castShadow = true;
        lightData.SetShadowSettings(shadowSettings);
    }
    auto [entity, camera] = *registry->GetView<Camera>().begin();
    scene->SetCamera(registry->GetEntityRef(entity));
    scene->GetFogSettings().globalExtinction = 0.1f,

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
