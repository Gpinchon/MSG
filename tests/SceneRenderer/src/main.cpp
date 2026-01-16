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
#include <MSG/ImageUtils.hpp>
#include <MSG/Keyboard/Events.hpp>
#include <MSG/Keyboard/Keyboard.hpp>
#include <MSG/Light/PunctualLight.hpp>
#include <MSG/Mouse/Events.hpp>
#include <MSG/Mouse/Mouse.hpp>
#include <MSG/PageFile.hpp>
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
        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            if (arg == "--env") {
                i++;
                envPath = argv[i];
            } else if (arg == "--model") {
                i++;
                modelPath = argv[i];
            } else if (arg == "--maxRes") {
                i++;
                maxRes = std::stoi(argv[i]);
            } else if (arg == "--compressImages") {
                compressImages = true;
            } else if (arg == "--generateLods") {
                generateLods = true;
            } else if (arg == "--lodsNbr") {
                i++;
                lodsNbr = std::clamp(std::stoi(argv[i]), 0, 255);
            } else if (arg == "--lodsBias") {
                i++;
                lodsBias = std::clamp(std::stof(argv[i]), 0.f, 1.f);
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
        if (!envPath.empty() && !std::filesystem::exists(envPath)) {
            std::cerr << "--env : " << envPath << " not found." << std::endl;
            error = true;
        }
        if (!std::filesystem::exists(modelPath)) {
            std::cerr << "--model : " << modelPath << " not found." << std::endl;
            error = true;
        }
        if (error)
            exit(-1);
        std::cout << "--env                " << envPath << std::endl;
        std::cout << "--model              " << modelPath << std::endl;
        std::cout << "--maxRes             " << maxRes << std::endl;
        std::cout << "--compressImages     " << compressImages << std::endl;
        std::cout << "--generateLods       " << generateLods << std::endl;
        std::cout << "--lodsNbr            " << int(lodsNbr) << std::endl;
        std::cout << "--lodsBias           " << lodsBias << std::endl;
    }
    void PrintHelp()
    {
        std::cout << "Usage : \n"
                     " --env   [path : equirectangular env file]\n"
                     " --model [path : model file]\n"
                     " [OPTIONAL] --maxRes [uint : max texture size]\n"
                     " [OPTIONAL] --compressImages\n"
                     " [OPTIONAL] --generateLods\n"
                     " [OPTIONAL] --lodsNbr [uint8 : number of lods to generate]\n"
                     " [OPTIONAL] --lodsBias [float : bias for lods screen coverage]\n"
                  << std::endl;
    }
    std::filesystem::path modelPath;
    std::filesystem::path envPath;
    bool generateLods   = false;
    uint8_t lodsNbr     = 3;
    float lodsBias      = 0.f;
    bool compressImages = false;
    uint32_t maxRes     = std::numeric_limits<uint32_t>::max();
};

struct OrbitCamera {
    explicit OrbitCamera(std::shared_ptr<ECS::DefaultRegistry> const& a_Registry)
        : entity(Entity::Camera::Create(a_Registry))
    {
        auto& settings                             = entity.GetComponent<Msg::Camera>().settings;
        settings.colorGrading.autoExposure.enabled = true;
        settings.colorGrading.autoExposure.key     = 0.18f;
        settings.colorGrading.exposure             = 0.0f;
        settings.colorGrading.saturation           = 0.5f;
        Update();
    }
    void Update() const
    {
        CameraProjectionPerspectiveInfinite cameraProj;
        cameraProj.fov                           = fov;
        cameraProj.aspectRatio                   = aspectRatio;
        entity.GetComponent<Camera>().projection = cameraProj;
        Entity::Node::Orbit(entity,
            targetPosition,
            radius, theta, phi);
        Entity::Node::UpdateWorldTransform(entity, {}, false);
    }
    float fov                = 45.f;
    float aspectRatio        = testWindowWidth / float(testWindowHeight);
    float radius             = 1;
    float theta              = M_PI / 2.f;
    float phi                = M_PI;
    glm::vec3 targetPosition = { 0, 0, 0 };
    ECS::DefaultRegistry::EntityRefType entity;
};

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
        .ssao               = { .quality = Renderer::QualitySetting::Medium }
    };
    RenderBuffer::CreateRenderBufferInfo renderBufferInfo {
        .width  = testWindowWidth,
        .height = testWindowHeight
    };
    Window::CreateWindowInfo windowInfo {
        .name   = "Msg::UnitTests::SceneRenderer",
        .flags  = Window::FlagsResizableBits,
        .width  = testWindowWidth,
        .height = testWindowHeight,
        .vSync  = true
    };

    auto renderer     = Renderer::Create(rendererInfo, rendererSettings);
    auto window       = Window::Create(renderer, windowInfo);
    auto renderBuffer = RenderBuffer::Create(renderer, renderBufferInfo);
    auto registry     = ECS::DefaultRegistry::Create();

    auto modelAsset = std::make_shared<Assets::Asset>(args.modelPath);

    modelAsset->SetECSRegistry(registry);

    modelAsset->parsingOptions.image.maxWidth    = args.maxRes;
    modelAsset->parsingOptions.image.maxHeight   = args.maxRes;
    modelAsset->parsingOptions.texture.compress  = args.compressImages;
    modelAsset->parsingOptions.mesh.generateLODs = args.generateLods;
    modelAsset->parsingOptions.mesh.lodsNbr      = args.lodsNbr;

    std::shared_ptr<Scene> scene;
    std::shared_ptr<Animation> currentAnimation;
    std::vector<std::shared_ptr<Animation>> animations;
    Assets::InitParsers();
    {
        auto model        = Assets::Parser::Parse(modelAsset);
        auto parsedScenes = model->Get<Scene>();
        animations        = model->Get<Animation>();
        if (!parsedScenes.empty())
            scene = parsedScenes.front();
        else
            scene = std::make_shared<Scene>(registry, "testScene");
        scene->SetBackgroundColor({ 0, 0, 0, 0 });
        scene->SetLevelOfDetailsBias(args.lodsBias);
        scene->GetFogSettings().globalPhaseG     = 0.75;
        scene->GetFogSettings().globalExtinction = 0;
        for (auto [entity, name, lightData] : registry->GetView<Core::Name, PunctualLight>()) {
            auto shadowSettings = lightData.GetShadowSettings();
            if (lightData.GetType() == LightType::Directional) {
                shadowSettings.castShadow = true;
                shadowSettings.resolution = 2048;
                shadowSettings.blurRadius = 2.5;
                lightData.SetPriority(1000);
            }
            lightData.SetShadowSettings(shadowSettings);
        }
    }
    if (!args.envPath.empty()) {
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
            auto skybox = std::make_shared<Texture>(TextureType::TextureCubemap, std::make_shared<Image>(cubemap));
            TextureGenerateMipmaps(*skybox);
            auto lightIBLEntity = Entity::PunctualLight::Create(registry);
            auto& lightIBLComp  = lightIBLEntity.GetComponent<PunctualLight>();
            LightIBL lightIBLData({ 64, 64 }, skybox);
            lightIBLData.intensity = 1;
            lightIBLComp           = lightIBLData;

            scene->AddEntity(lightIBLEntity);
            scene->SetSkybox(skybox);
        }
    } else if (registry->GetView<PunctualLight>().empty()) {
        auto lightDirEntity = Entity::PunctualLight::Create(registry);
        auto& lightDirComp  = lightDirEntity.GetComponent<PunctualLight>();
        auto& lightDirTrans = lightDirEntity.GetComponent<Transform>();
        lightDirTrans.SetLocalPosition(glm::vec3 { 10, 10, 10 });
        lightDirTrans.UpdateWorld({});
        Entity::Node::LookAt(lightDirEntity, glm::vec3 { 0, 0, 0 });
        lightDirTrans.UpdateWorld({});
        LightDirectional lightDirData;
        lightDirData.intensity      = 1;
        lightDirData.shadowSettings = {
            .castShadow = true,
            .blurRadius = 5,
            .resolution = 2048
        };
        lightDirComp = lightDirData;
        scene->AddEntity(lightDirEntity);
    }
    OrbitCamera camera(registry);
    if (scene->GetCamera().Empty()) {
        scene->AddEntity(camera.entity);
        scene->SetCamera(camera.entity);
    }

    int cameraMovementSpeed   = 1.f;
    int currentAnimationIndex = 0;
    Events::BindCallback(EventKeyboardKeyPressed::Type,
        [&animations = animations, &currentAnimation = currentAnimation, &currentAnimationIndex = currentAnimationIndex, &cameraMovementSpeed = cameraMovementSpeed](const Event& a_Event, const EventBindingID&, std::any) {
            auto& keyboardEvent = reinterpret_cast<const EventKeyboardKeyPressed&>(a_Event);
            if (!animations.empty() && keyboardEvent.scancode == Keyboard::ScanCode::KeyA) {
                if (currentAnimation != nullptr)
                    currentAnimation->Stop();
                currentAnimation = animations.at(currentAnimationIndex);
                currentAnimation->SetLoop(true);
                currentAnimation->SetLoopMode(Animation::LoopMode::Repeat);
                currentAnimation->Play();
                currentAnimationIndex = (currentAnimationIndex + 1) % animations.size();
            } else if (keyboardEvent.scancode == Keyboard::ScanCode::KpPlus) {
                cameraMovementSpeed++;
            } else if (keyboardEvent.scancode == Keyboard::ScanCode::KpMinus) {
                cameraMovementSpeed--;
            }
            cameraMovementSpeed = std::max(1, cameraMovementSpeed);
        });
    Events::BindCallback(EventWindowResized::Type,
        [&renderer, &renderBuffer, &camera](const Event& a_Event, const EventBindingID&, std::any) {
            auto& windowResizedEvent = reinterpret_cast<const EventWindowResized&>(a_Event);
            renderBuffer             = RenderBuffer::Create(renderer, { windowResizedEvent.width, windowResizedEvent.height });
            camera.aspectRatio       = windowResizedEvent.width / float(windowResizedEvent.height);
            Renderer::SetActiveRenderBuffer(renderer, renderBuffer);
        });
    int lastMouseX = -1;
    int lastMouseY = -1;
    Events::BindCallback(EventMouseMotion::Type,
        [&camera, &lastMouseX = lastMouseX, &lastMouseY = lastMouseY, &cameraMovementSpeed = cameraMovementSpeed](const Event& a_Event, const EventBindingID&, std::any) {
            auto& mouseMotionEvent = reinterpret_cast<const EventMouseMotion&>(a_Event);
            auto state             = Mouse::GetState();
            if (lastMouseX == -1)
                lastMouseX = mouseMotionEvent.position.x;
            if (lastMouseY == -1)
                lastMouseY = mouseMotionEvent.position.y;
            auto relMoveX = lastMouseX - mouseMotionEvent.position.x;
            auto relMoveY = lastMouseY - mouseMotionEvent.position.y;
            if (state.buttons[Mouse::LeftButton]) {
                camera.theta += relMoveY * 0.001f;
                camera.phi += relMoveX * 0.001f;
            }
            if (state.buttons[Mouse::RightButton]) {
                auto& cameraTransform = camera.entity.GetComponent<Msg::Transform>();
                auto cameraRight      = cameraTransform.GetWorldRight() * (relMoveX * 0.001f * cameraMovementSpeed);
                auto cameraUp         = cameraTransform.GetWorldUp() * -(relMoveY * 0.001f * cameraMovementSpeed);
                camera.targetPosition = camera.targetPosition + cameraRight + cameraUp;
            }
            lastMouseX = mouseMotionEvent.position.x;
            lastMouseY = mouseMotionEvent.position.y;
        });
    Events::BindCallback(EventMouseWheel::Type,
        [&camera, &cameraMovementSpeed](const Event& a_Event, const EventBindingID&, std::any) {
            auto& mouseWheelEvent = reinterpret_cast<const EventMouseWheel&>(a_Event);
            camera.radius -= mouseWheelEvent.amount.y * 0.05f * cameraMovementSpeed;
        });
    {
        Tools::ScopedTimer timer("Loading Test Scene");
        Renderer::Load(renderer, *scene);
    }
    PageFile::Global().Shrink();
    Renderer::SetActiveScene(renderer, scene.get());
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
            if (currentAnimation != nullptr)
                currentAnimation->Advance(updateDelta / 1000.f);
            scene->Update();
            camera.Update();
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
