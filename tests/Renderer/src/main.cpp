#include <MSG/ECS/Registry.hpp>

#include <MSG/Cubemap.hpp>
#include <MSG/Entity/Camera.hpp>
#include <MSG/Entity/Node.hpp>
#include <MSG/Entity/PunctualLight.hpp>
#include <MSG/Light/PunctualLight.hpp>
#include <MSG/Material.hpp>
#include <MSG/Material/Extension/SpecularGlossiness.hpp>
#include <MSG/Scene.hpp>
#include <MSG/ShapeGenerator/Cube.hpp>
#include <MSG/ShapeGenerator/Sphere.hpp>
#include <MSG/Texture.hpp>

#include <MSG/Renderer.hpp>
#include <MSG/Renderer/RenderBuffer.hpp>

#include <MSG/Window/Events.hpp>
#include <MSG/Window/Window.hpp>

#include <MSG/Tools/FPSCounter.hpp>
#include <MSG/Tools/ScopedTimer.hpp>

using namespace MSG;

constexpr auto testWindowWidth  = 1280;
constexpr auto testWindowHeight = 800;
constexpr auto testCubesNbr     = 75;
constexpr auto testLightNbr     = testCubesNbr;
constexpr auto testGridSize     = testCubesNbr * 2;

auto GetCameraProj(const uint32_t& a_Width, const uint32_t& a_Height)
{
    Core::Projection::PerspectiveInfinite cameraProj;
    cameraProj.znear       = 1.f;
    cameraProj.fov         = 90.f;
    cameraProj.aspectRatio = a_Width / float(a_Height);
    return cameraProj;
}

class TestScene : public Scene {
public:
    using Scene::Scene;
    void Init()
    {
        SetBackgroundColor({ 0.529, 0.808, 0.922 });
        SetSkybox({ .texture = environment });
        for (auto& entity : meshes)
            AddEntity(entity);
        for (auto& light : lights)
            AddEntity(light);
        UpdateWorldTransforms();
    }
    std::shared_ptr<Texture> environment;
    std::vector<ECS::DefaultRegistry::EntityRefType> meshes;
    std::vector<ECS::DefaultRegistry::EntityRefType> lights;
};

class RendererTestScene : public TestScene {
public:
    using TestScene::TestScene;
    RendererTestScene(const std::shared_ptr<ECS::DefaultRegistry>& a_Registry)
        : TestScene(a_Registry, "RendererTestScene")
    {
        SetCamera(CreateCamera());
        AddEntity(GetCamera());
        environment = CreateEnvironment();
        meshes      = CreateMeshes();
        lights      = CreateLights();
        Init();
    }
    std::shared_ptr<Texture> CreateEnvironment() const
    {
        auto env     = std::make_shared<Cubemap>(PixelSizedFormat::Uint8_NormalizedRGB, 256, 256);
        auto texture = std::make_shared<Texture>(TextureType::TextureCubemap, env);
        env->Allocate();
        for (uint32_t side = 0; side < 6; ++side) {
            PixelColor color;
            switch (CubemapSide(side)) {
            case CubemapSide::PositiveX:
                color = { 1.0, 0.0, 0.0, 1.0 };
                // color = { 0.529, 0.808, 0.922, 1.0 };
                break;
            case CubemapSide::NegativeX:
                color = { 0.5, 0.0, 0.0, 1.0 };
                // color = { 0.529, 0.808, 0.922, 1.0 };
                break;
            case CubemapSide::PositiveY:
                color = { 0.0, 1.0, 0.0, 1.0 };
                // color = { 0.529, 0.808, 0.922, 1.0 };
                break;
            case CubemapSide::NegativeY:
                color = { 0.0, 0.5, 0.0, 1.0 };
                // color = { 0.529, 0.808, 0.922, 1.0 };
                break;
            case CubemapSide::PositiveZ:
                color = { 0.0, 0.0, 1.0, 1.0 };
                // color = { 0.529, 0.808, 0.922, 1.0 };
                break;
            case CubemapSide::NegativeZ:
                color = { 0.0, 0.0, 0.5, 1.0 };
                // color = { 0.529, 0.808, 0.922, 1.0 };
                break;
            }
            env->at(side).Fill(color);
        }
        texture->GenerateMipmaps();
        return texture;
    }
    ECS::DefaultRegistry::EntityRefType CreateCamera() const
    {
        auto testCamera                                    = Entity::Camera::Create(GetRegistry());
        testCamera.GetComponent<Core::Camera>().projection = GetCameraProj(testWindowWidth, testWindowHeight);
        testCamera.GetComponent<MSG::Core::Transform>().SetLocalPosition({ 5, 5, 5 });
        Entity::Node::UpdateWorldTransform(testCamera, {}, false);
        Entity::Node::LookAt(testCamera, glm::vec3(0));
        return testCamera;
    }
    std::vector<ECS::DefaultRegistry::EntityRefType> CreateMeshes() const
    {
        std::vector<ECS::DefaultRegistry::EntityRefType> testEntities;
        auto testMesh = ShapeGenerator::CreateCubeMesh("testMesh", { 1, 1, 1 });
        MaterialExtensionSpecularGlossiness specGloss;
        // plastic
        specGloss.diffuseFactor    = { 1.0, 1.0, 1.0, 1.0 };
        specGloss.specularFactor   = { 0.04, 0.04, 0.04 };
        specGloss.glossinessFactor = 0;
        testMesh.GetMaterials().front()->AddExtension(specGloss);
        for (auto x = 0u; x < testCubesNbr; ++x) {
            float xCoord = (x / float(testCubesNbr) - 0.5) * testGridSize;
            for (auto y = 0u; y < testCubesNbr; ++y) {
                float yCoord    = (y / float(testCubesNbr) - 0.5) * testGridSize;
                auto testEntity = Entity::Node::Create(GetRegistry());
                testEntities.push_back(testEntity);
                testEntity.AddComponent<Mesh>(testMesh);
                testEntity.GetComponent<MSG::Core::Transform>().SetLocalPosition({ xCoord, 0, yCoord });
            }
        }
        return testEntities;
    }
    std::vector<ECS::DefaultRegistry::EntityRefType> CreateLights() const
    {
        std::vector<ECS::DefaultRegistry::EntityRefType> testLights;
        auto lightIBLEntity = Entity::PunctualLight::Create(GetRegistry());
        auto& lightIBLComp  = lightIBLEntity.GetComponent<PunctualLight>();
        LightIBL lightIBLData({ 64, 64 }, environment);
        lightIBLData.intensity = 1;
        lightIBLComp           = lightIBLData;
        testLights.push_back(lightIBLEntity);
        unsigned currentLight = 0;
        for (auto x = 0u; x < testLightNbr; ++x) {
            float xCoord = (x / float(testLightNbr) - 0.5) * testGridSize;
            for (auto y = 0u; y < testLightNbr; ++y) {
                float yCoord         = (y / float(testLightNbr) - 0.5) * testGridSize;
                auto light           = Entity::PunctualLight::Create(GetRegistry());
                auto& lightData      = light.GetComponent<PunctualLight>();
                auto& lightTransform = light.GetComponent<MSG::Core::Transform>();
                lightTransform.SetLocalPosition({ xCoord, 1, yCoord });
                Entity::Node::UpdateWorldTransform(light, {}, false);
                if (currentLight % 2 == 0) {
                    Entity::Node::LookAt(light, { xCoord, 0, yCoord });
                    LightSpot spot;
                    spot.range          = 1;
                    spot.innerConeAngle = 0.3;
                    spot.outerConeAngle = 0.5;
                    lightData           = spot;
                } else {
                    LightPoint point;
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
                testLights.push_back(light);
                ++currentLight;
            }
        }
        return testLights;
    }
};

int main(int argc, char const* argv[])
{
    Renderer::CreateRendererInfo rendererInfo {
        .name               = "UnitTest",
        .applicationVersion = 100
    };
    Renderer::RendererSettings rendererSettings {
        .mode = Renderer::RendererMode::Forward
    };
    Renderer::CreateRenderBufferInfo renderBufferInfo {
        .width  = testWindowWidth,
        .height = testWindowHeight
    };
    Window::CreateWindowInfo windowInfo {
        .name   = "UnitTests::Renderer",
        .flags  = Window::FlagsResizableBits,
        .width  = testWindowWidth,
        .height = testWindowHeight,
        .vSync  = true
    };

    auto registry     = ECS::DefaultRegistry::Create();
    auto renderer     = Renderer::Create(rendererInfo, rendererSettings);
    auto window       = Window::Create(renderer, windowInfo);
    auto renderBuffer = Renderer::RenderBuffer::Create(renderer, renderBufferInfo);

    float cameraTheta = M_PI / 2.f - 1;
    float cameraPhi   = M_PI;

    // build a test scene
    RendererTestScene testScene(registry);
    EventBindingWrapper windowResizeBinding = Events::BindCallback(EventWindowResized::Type,
        [&renderer, &renderBuffer, &testScene](const Event& a_Event, const EventBindingID&, std::any) {
            auto& resizeEvent                                             = reinterpret_cast<const EventWindowResized&>(a_Event);
            renderBuffer                                                  = Renderer::RenderBuffer::Create(renderer, { resizeEvent.width, resizeEvent.height });
            testScene.GetCamera().GetComponent<Core::Camera>().projection = GetCameraProj(resizeEvent.width, resizeEvent.height);
            Renderer::SetActiveRenderBuffer(renderer, renderBuffer);
        });
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
        const auto now = std::chrono::high_resolution_clock::now();
        lastTime       = now;
        fpsCounter.StartFrame();
        auto updateDelta = std::chrono::duration<double, std::milli>(now - updateTime).count();
        if (updateDelta > 16) {
            for (auto& entity : testScene.meshes) {
                auto entityMaterial   = entity.GetComponent<Mesh>().GetMaterials().front();
                auto& entityTransform = entity.GetComponent<MSG::Core::Transform>();
                auto& diffuseOffset   = entityMaterial->GetExtension<MaterialExtensionSpecularGlossiness>().diffuseTexture.transform.offset;
                diffuseOffset.x += 0.000005f * float(updateDelta);
                diffuseOffset.x = diffuseOffset.x > 2 ? 0 : diffuseOffset.x;
                auto rot        = entity.GetComponent<MSG::Core::Transform>().GetLocalRotation();
                rot             = glm::rotate(rot, 0.001f * float(updateDelta), { 0, 1, 0 });
                entityTransform.SetLocalRotation(rot);
            }
            cameraPhi   = cameraPhi - 0.0005f * float(updateDelta);
            cameraPhi   = cameraPhi > 2 * M_PI ? 0 : cameraPhi;
            cameraTheta = cameraTheta > M_PI ? 0 : cameraTheta;
            Entity::Node::UpdateWorldTransform(testScene.GetCamera(), {}, false);
            Entity::Node::Orbit(testScene.GetCamera(),
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
