#include <ECS/Registry.hpp>

#include <SG/Core/Image/Cubemap.hpp>
#include <SG/Core/Material.hpp>
#include <SG/Core/Material/Extension/SpecularGlossiness.hpp>
#include <SG/Core/Texture/Texture.hpp>
#include <SG/Entity/Camera.hpp>
#include <SG/Entity/Light/PunctualLight.hpp>
#include <SG/Entity/Node.hpp>
#include <SG/Scene/Scene.hpp>
#include <SG/ShapeGenerator/Cube.hpp>
#include <SG/ShapeGenerator/Sphere.hpp>

#include <Renderer/RenderBuffer.hpp>
#include <Renderer/Renderer.hpp>
#include <Renderer/SwapChain.hpp>

#include <Window/Events.hpp>
#include <Window/Window.hpp>

#include <Tools/FPSCounter.hpp>
#include <Tools/ScopedTimer.hpp>

using namespace TabGraph;

constexpr auto testWindowWidth  = 1280;
constexpr auto testWindowHeight = 800;
constexpr auto testCubesNbr     = 75;
constexpr auto testLightNbr     = testCubesNbr;
constexpr auto testGridSize     = testCubesNbr * 2;

auto GetCameraProj(const uint32_t& a_Width, const uint32_t& a_Height)
{
    SG::Component::Projection::PerspectiveInfinite cameraProj;
    cameraProj.znear       = 1.f;
    cameraProj.fov         = 90.f;
    cameraProj.aspectRatio = a_Width / float(a_Height);
    return cameraProj;
}

static auto CreateEnv()
{
    auto env     = std::make_shared<SG::Cubemap>(SG::Pixel::SizedFormat::Uint8_NormalizedRGB, 256, 256);
    auto texture = std::make_shared<SG::Texture>(SG::TextureType::TextureCubemap, env);
    env->Allocate();
    for (uint32_t side = 0; side < 6; ++side) {
        SG::Pixel::Color color;
        switch (SG::CubemapSide(side)) {
        case SG::CubemapSide::PositiveX:
            color = { 1.0, 0.0, 0.0, 1.0 };
            // color = { 0.529, 0.808, 0.922, 1.0 };
            break;
        case SG::CubemapSide::NegativeX:
            color = { 0.5, 0.0, 0.0, 1.0 };
            // color = { 0.529, 0.808, 0.922, 1.0 };
            break;
        case SG::CubemapSide::PositiveY:
            color = { 0.0, 1.0, 0.0, 1.0 };
            // color = { 0.529, 0.808, 0.922, 1.0 };
            break;
        case SG::CubemapSide::NegativeY:
            color = { 0.0, 0.5, 0.0, 1.0 };
            // color = { 0.529, 0.808, 0.922, 1.0 };
            break;
        case SG::CubemapSide::PositiveZ:
            color = { 0.0, 0.0, 1.0, 1.0 };
            // color = { 0.529, 0.808, 0.922, 1.0 };
            break;
        case SG::CubemapSide::NegativeZ:
            color = { 0.0, 0.0, 0.5, 1.0 };
            // color = { 0.529, 0.808, 0.922, 1.0 };
            break;
        }
        env->at(side).Fill(color);
    }
    texture->GenerateMipmaps();
    return texture;
}

int main(int argc, char const* argv[])
{
    Renderer::CreateRendererInfo rendererInfo {
        .name               = "UnitTest",
        .applicationVersion = 100,
#ifdef __linux
        .nativeDisplayHandle = XOpenDisplay(nullptr)
#endif
    };
    auto registry     = ECS::DefaultRegistry::Create();
    auto renderer     = Renderer::Create(rendererInfo, { .mode = Renderer::RendererMode::Forward });
    auto window       = Window::Create(renderer, { .width = testWindowWidth, .height = testWindowHeight, .vSync = false, .nativeDisplayHandle = Renderer::GetNativeDisplayHandle(renderer) });
    auto renderBuffer = Renderer::RenderBuffer::Create(renderer, { Window::GetWidth(window), Window::GetHeight(window) });
    auto env          = CreateEnv();
    float cameraTheta = M_PI / 2.f - 1;
    float cameraPhi   = M_PI;

    // build a test scene
    SG::Scene testScene(registry, "testScene");
    testScene.SetBackgroundColor({ 0.529, 0.808, 0.922 });

    auto testCamera                                             = SG::Camera::Create(registry);
    testCamera.GetComponent<SG::Component::Camera>().projection = GetCameraProj(testWindowWidth, testWindowHeight);
    testCamera.GetComponent<SG::Component::Transform>().SetLocalPosition({ 5, 5, 5 });
    SG::Node::UpdateWorldTransform(testCamera, {}, false);
    SG::Node::LookAt(testCamera, glm::vec3(0));
    testScene.AddEntity(testCamera);
    testScene.SetCamera(testCamera);
    testScene.SetSkybox({ .texture = env });
    std::vector<ECS::DefaultRegistry::EntityRefType> testEntitis;
    {
        auto testMesh = SG::CreateCubeMesh("testMesh", { 1, 1, 1 });
        // auto testMesh = SG::CreateSphereMesh("testMesh", 0.75, 4);
        SG::SpecularGlossinessExtension specGloss;
        // gold
        // specGloss.diffuseFactor    = { 0.0, 0.0, 0.0, 1.0 };
        // specGloss.specularFactor   = { 1.0, 0.766, 0.336 };
        // specGloss.glossinessFactor = 0.1;
        // plastic
        specGloss.diffuseFactor    = { 1.0, 1.0, 1.0, 1.0 };
        specGloss.specularFactor   = { 0.04, 0.04, 0.04 };
        specGloss.glossinessFactor = 0;
        testMesh.GetMaterials().front()->AddExtension(specGloss);
        for (auto x = 0u; x < testCubesNbr; ++x) {
            float xCoord = (x / float(testCubesNbr) - 0.5) * testGridSize;
            for (auto y = 0u; y < testCubesNbr; ++y) {
                float yCoord    = (y / float(testCubesNbr) - 0.5) * testGridSize;
                auto testEntity = SG::Node::Create(registry);
                testEntitis.push_back(testEntity);
                testEntity.AddComponent<SG::Component::Mesh>(testMesh);
                testEntity.GetComponent<SG::Component::Transform>().SetLocalPosition({ xCoord, 0, yCoord });
                testScene.AddEntity(testEntity);
            }
        }
    }
    {
        auto lightIBLEntity = SG::PunctualLight::Create(registry);
        auto& lightIBLComp  = lightIBLEntity.GetComponent<SG::Component::PunctualLight>();
        SG::Component::LightIBL lightIBLData({ 64, 64 }, env);
        lightIBLData.intensity = 1;
        lightIBLComp           = lightIBLData;
        testScene.AddEntity(lightIBLEntity);
        unsigned currentLight = 0;
        for (auto x = 0u; x < testLightNbr; ++x) {
            float xCoord = (x / float(testLightNbr) - 0.5) * testGridSize;
            for (auto y = 0u; y < testLightNbr; ++y) {
                float yCoord         = (y / float(testLightNbr) - 0.5) * testGridSize;
                auto light           = SG::PunctualLight::Create(registry);
                auto& lightData      = light.GetComponent<SG::Component::PunctualLight>();
                auto& lightTransform = light.GetComponent<SG::Component::Transform>();
                lightTransform.SetLocalPosition({ xCoord, 1, yCoord });
                SG::Node::UpdateWorldTransform(light, {}, false);
                if (currentLight % 2 == 0) {
                    SG::Node::LookAt(light, { xCoord, 0, yCoord });
                    SG::Component::LightSpot spot;
                    spot.range          = 1;
                    spot.innerConeAngle = 0.3;
                    spot.outerConeAngle = 0.5;
                    lightData           = spot;
                } else {
                    SG::Component::LightPoint point;
                    point.range = 1;
                    lightData   = point;
                }
                std::visit([](auto& a_Data) {
                    a_Data.intensity = 10;
                    a_Data.color     = {
                        std::rand() / float(RAND_MAX),
                        std::rand() / float(RAND_MAX),
                        std::rand() / float(RAND_MAX)
                    };
                },
                    lightData);
                testScene.AddEntity(light);
                ++currentLight;
            }
        }
    }
    EventBindingWrapper windowResizeBinding = Events::BindCallback(EventWindowResized::Type,
        [&renderer, &renderBuffer, testCamera](const Event& a_Event, const EventBindingID&, std::any) {
            auto& resizeEvent                                                    = reinterpret_cast<const EventWindowResized&>(a_Event);
            renderBuffer                                                         = Renderer::RenderBuffer::Create(renderer, { resizeEvent.width, resizeEvent.height });
            testCamera.template GetComponent<SG::Component::Camera>().projection = GetCameraProj(resizeEvent.width, resizeEvent.height);
            Renderer::SetActiveRenderBuffer(renderer, renderBuffer);
        });
    testScene.UpdateWorldTransforms();
    {
        Tools::ScopedTimer timer("Loading Test Scene");
        Renderer::Load(renderer, testScene);
    }

    Renderer::SetActiveScene(renderer, &testScene);
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
        if (updateDelta > 16) {
            for (auto& entity : testEntitis) {
                auto entityMaterial   = entity.GetComponent<SG::Component::Mesh>().GetMaterials().front();
                auto& entityTransform = entity.GetComponent<SG::Component::Transform>();
                auto& diffuseOffset   = entityMaterial->GetExtension<SG::SpecularGlossinessExtension>().diffuseTexture.transform.offset;
                diffuseOffset.x += 0.000005f * float(updateDelta);
                diffuseOffset.x = diffuseOffset.x > 2 ? 0 : diffuseOffset.x;
                auto rot        = entity.GetComponent<SG::Component::Transform>().GetLocalRotation();
                rot             = glm::rotate(rot, 0.001f * float(updateDelta), { 0, 1, 0 });
                entityTransform.SetLocalRotation(rot);
            }
            cameraPhi   = cameraPhi - 0.0005f * float(updateDelta);
            cameraPhi   = cameraPhi > 2 * M_PI ? 0 : cameraPhi;
            cameraTheta = cameraTheta > M_PI ? 0 : cameraTheta;
            SG::Node::UpdateWorldTransform(testCamera, {}, false);
            SG::Node::Orbit(testCamera,
                glm::vec3(0),
                5, cameraTheta, cameraPhi);
            updateTime = now;
            testScene.Update();
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
    Renderer::Unload(renderer, testScene);
    return 0;
}
