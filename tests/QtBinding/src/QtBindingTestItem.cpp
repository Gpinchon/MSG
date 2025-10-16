#include <QtBindingTestItem.hpp>

#include <MSG/Entity/Camera.hpp>
#include <MSG/Entity/PunctualLight.hpp>
#include <MSG/Image.hpp>
#include <MSG/ImageUtils.hpp>
#include <MSG/Material.hpp>
#include <MSG/Material/Extension/SpecularGlossiness.hpp>
#include <MSG/Renderer.hpp>
#include <MSG/Sampler.hpp>
#include <MSG/ShapeGenerator/Cube.hpp>
#include <MSG/Texture.hpp>
#include <MSG/TextureUtils.hpp>

using namespace Msg;

constexpr auto testCubesNbr = 10;
constexpr auto testLightNbr = testCubesNbr;
constexpr auto testGridSize = testCubesNbr * 2;

CameraProjection GetCameraProj(const uint32_t& a_Width, const uint32_t& a_Height);
std::shared_ptr<Texture> CreateEnvironment();
ECS::DefaultRegistry::EntityRefType CreateCamera(
    const std::shared_ptr<ECS::DefaultRegistry>& a_Registry,
    const QSize& a_Size);
std::vector<ECS::DefaultRegistry::EntityRefType> CreateMeshes(const std::shared_ptr<ECS::DefaultRegistry>& a_Registry);
std::vector<ECS::DefaultRegistry::EntityRefType> CreateLights(
    const std::shared_ptr<ECS::DefaultRegistry>& a_Registry,
    const std::shared_ptr<Texture>& a_Env);

QtBindingTestItem::QtBindingTestItem()
    : QtItem()
    , _scene(ECS::DefaultRegistry::Create(), "QtBindingTestScene")
{
}

void Msg::QtBindingTestItem::componentComplete()
{
    QtItem::componentComplete();
    connect(
        this, &QtItem::rendererInitialized,
        this, &QtBindingTestItem::createScene);
    connect(
        this, &QtItem::rendererInvalidated,
        this, &QtBindingTestItem::clearScene);
    connect(
        this, &QtItem::renderBufferUpdated,
        this, &QtBindingTestItem::updateCameraProj);
}

void Msg::QtBindingTestItem::updatePolish()
{
    QtItem::updatePolish();
    _scene.Update();
}

void QtBindingTestItem::RegisterQMLType()
{
    qmlRegisterType<Msg::QtBindingTestItem>("MSG", 1, 0, "MSGBindingTestItem");
}

void QtBindingTestItem::createScene()
{
    _scene.SetCamera(CreateCamera(_scene.GetRegistry(), QSize(width(), height())));
    _scene.AddEntity(_scene.GetCamera());
    _scene.environment = CreateEnvironment();
    _scene.meshes      = CreateMeshes(_scene.GetRegistry());
    _scene.lights      = CreateLights(_scene.GetRegistry(), _scene.environment);
    _scene.Init();
    Renderer::Load(renderer, _scene);
    Renderer::SetActiveScene(renderer, &_scene);
}

void Msg::QtBindingTestItem::clearScene()
{
    Renderer::SetActiveScene(renderer, nullptr);
    Renderer::Unload(renderer, _scene);
    // reset the test scene to default in order to free resources
    _scene = TestScene(ECS::DefaultRegistry::Create());
}

void Msg::QtBindingTestItem::updateCameraProj(const QSize& a_NewSize)
{
    _scene.GetCamera().GetComponent<Camera>().projection = GetCameraProj(a_NewSize.width(), a_NewSize.height());
}

CameraProjection GetCameraProj(const uint32_t& a_Width, const uint32_t& a_Height)
{
    CameraProjectionPerspectiveInfinite cameraProj;
    cameraProj.znear       = 1.f;
    cameraProj.fov         = 90.f;
    cameraProj.aspectRatio = a_Width / float(a_Height);
    return cameraProj;
}

std::shared_ptr<Texture> CreateEnvironment()
{
    ImageInfo envInfo {
        .width     = 256,
        .height    = 256,
        .depth     = 6,
        .pixelDesc = PixelSizedFormat::Uint8_NormalizedRGB
    };
    auto env     = std::make_shared<Image>(envInfo);
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
        auto layer = ImageGetLayer(*env, side);
        ImageFill(layer, color);
    }
    TextureGenerateMipmaps(*texture);
    return texture;
}

ECS::DefaultRegistry::EntityRefType CreateCamera(const std::shared_ptr<ECS::DefaultRegistry>& a_Registry, const QSize& a_Size)
{
    auto testCamera                              = Entity::Camera::Create(a_Registry);
    testCamera.GetComponent<Camera>().projection = GetCameraProj(a_Size.width(), a_Size.height());
    testCamera.GetComponent<Transform>().SetLocalPosition({ 5, 5, 5 });
    Entity::Node::UpdateWorldTransform(testCamera, {}, false);
    Entity::Node::LookAt(testCamera, glm::vec3(0));
    return testCamera;
}

std::vector<ECS::DefaultRegistry::EntityRefType> CreateMeshes(const std::shared_ptr<ECS::DefaultRegistry>& a_Registry)
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
            auto testEntity = Entity::Node::Create(a_Registry);
            testEntities.push_back(testEntity);
            testEntity.AddComponent<Mesh>(testMesh);
            testEntity.GetComponent<Transform>().SetLocalPosition({ xCoord, 0, yCoord });
        }
    }
    return testEntities;
}

std::vector<ECS::DefaultRegistry::EntityRefType> CreateLights(
    const std::shared_ptr<ECS::DefaultRegistry>& a_Registry,
    const std::shared_ptr<Texture>& a_Env)
{
    std::vector<ECS::DefaultRegistry::EntityRefType> testLights;
    auto lightIBLEntity = Entity::PunctualLight::Create(a_Registry);
    auto& lightIBLComp  = lightIBLEntity.GetComponent<PunctualLight>();
    LightIBL lightIBLData({ 64, 64 }, a_Env);
    lightIBLData.intensity = 1;
    lightIBLComp           = lightIBLData;
    testLights.push_back(lightIBLEntity);
    unsigned currentLight = 0;
    for (auto x = 0u; x < testLightNbr; ++x) {
        float xCoord = (x / float(testLightNbr) - 0.5) * testGridSize;
        for (auto y = 0u; y < testLightNbr; ++y) {
            float yCoord         = (y / float(testLightNbr) - 0.5) * testGridSize;
            auto light           = Entity::PunctualLight::Create(a_Registry);
            auto& lightData      = light.GetComponent<PunctualLight>();
            auto& lightTransform = light.GetComponent<Msg::Transform>();
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