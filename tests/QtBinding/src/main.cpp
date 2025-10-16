// #include <MSG/ECS/Registry.hpp>

// #include <MSG/Entity/Camera.hpp>
// #include <MSG/Entity/Node.hpp>
// #include <MSG/Entity/PunctualLight.hpp>
// #include <MSG/ImageUtils.hpp>
// #include <MSG/Light/PunctualLight.hpp>
// #include <MSG/Material.hpp>
// #include <MSG/Material/Extension/SpecularGlossiness.hpp>
// #include <MSG/Sampler.hpp>
// #include <MSG/Scene.hpp>
// #include <MSG/ShapeGenerator/Cube.hpp>
// #include <MSG/ShapeGenerator/Sphere.hpp>
// #include <MSG/Texture.hpp>
// #include <MSG/TextureUtils.hpp>

// #include <MSG/Renderer.hpp>
// #include <MSG/Renderer/RenderBuffer.hpp>

// #include <MSG/Tools/FPSCounter.hpp>
// #include <MSG/Tools/ScopedTimer.hpp>

#include <QGuiApplication>
#include <QLoggingCategory>
#include <QQmlApplicationEngine>

#include <MSG/QtWindow.hpp>
#include <QtBindingTestItem.hpp>

using namespace Msg;

int main(int argc, char** argv)
{
    // Renderer::CreateRendererInfo rendererInfo {
    //     .name               = "UnitTest",
    //     .applicationVersion = 100
    // };
    // Renderer::RendererSettings rendererSettings {
    //     .mode = Renderer::RendererMode::Deferred
    // };
    // RenderBuffer::CreateRenderBufferInfo renderBufferInfo {
    //     .width  = testWindowWidth,
    //     .height = testWindowHeight
    // };
    // Window::CreateWindowInfo windowInfo {
    //     .name   = "UnitTests::Renderer",
    //     .flags  = Window::FlagsResizableBits,
    //     .width  = testWindowWidth,
    //     .height = testWindowHeight,
    //     .vSync  = true
    // };

    auto registry = ECS::DefaultRegistry::Create();
    // auto renderer     = Renderer::Create(rendererInfo, rendererSettings);
    // auto window       = Window::Create(renderer, windowInfo);
    // auto renderBuffer = RenderBuffer::Create(renderer, renderBufferInfo);

    // float cameraTheta = M_PI / 2.f - 1;
    // float cameraPhi   = M_PI;

    QGuiApplication app(argc, argv);
    QtWindow::RegisterQMLType();
    QtBindingTestItem::RegisterQMLType();
    QQmlApplicationEngine engine;
    engine.load(QUrl("qrc:/main.qml"));

    return app.exec();

    // EventBindingWrapper windowResizeBinding = Events::BindCallback(EventWindowResized::Type,
    //     [&renderer, &renderBuffer, &testScene](const Event& a_Event, const EventBindingID&, std::any) {
    //         auto& resizeEvent                                       = reinterpret_cast<const EventWindowResized&>(a_Event);
    //         renderBuffer                                            = RenderBuffer::Create(renderer, { resizeEvent.width, resizeEvent.height });
    //         testScene.GetCamera().GetComponent<Camera>().projection = GetCameraProj(resizeEvent.width, resizeEvent.height);
    //         Renderer::SetActiveRenderBuffer(renderer, renderBuffer);
    //     });
    // {
    //     Tools::ScopedTimer timer("Loading Test Scene");
    //     Renderer::Load(renderer, testScene);
    // }

    // Renderer::SetActiveScene(renderer, &testScene);
    // Renderer::SetActiveRenderBuffer(renderer, renderBuffer);
    // Renderer::Update(renderer);

    // Window::Show(window);
    // FPSCounter fpsCounter;
    // auto lastTime   = std::chrono::high_resolution_clock::now();
    // auto printTime  = lastTime;
    // auto updateTime = lastTime;
    // while (true) {
    //     Events::Update();
    //     Events::Consume();
    //     if (Window::IsClosing(window))
    //         break;
    //     const auto now = std::chrono::high_resolution_clock::now();
    //     lastTime       = now;
    //     fpsCounter.StartFrame();
    //     auto updateDelta = std::chrono::duration<double, std::milli>(now - updateTime).count();
    //     if (updateDelta > 16) {
    //         for (auto& entity : testScene.meshes) {
    //             auto entityMaterial   = entity.GetComponent<Mesh>().GetMaterials().front();
    //             auto& entityTransform = entity.GetComponent<Msg::Transform>();
    //             auto& diffuseOffset   = entityMaterial->GetExtension<MaterialExtensionSpecularGlossiness>().diffuseTexture.transform.offset;
    //             diffuseOffset.x += 0.000005f * float(updateDelta);
    //             diffuseOffset.x = diffuseOffset.x > 2 ? 0 : diffuseOffset.x;
    //             auto rot        = entity.GetComponent<Msg::Transform>().GetLocalRotation();
    //             rot             = glm::rotate(rot, 0.001f * float(updateDelta), { 0, 1, 0 });
    //             entityTransform.SetLocalRotation(rot);
    //         }
    //         cameraPhi   = cameraPhi - 0.0005f * float(updateDelta);
    //         cameraPhi   = cameraPhi > 2 * M_PI ? 0 : cameraPhi;
    //         cameraTheta = cameraTheta > M_PI ? 0 : cameraTheta;
    //         Entity::Node::UpdateWorldTransform(testScene.GetCamera(), {}, false);
    //         Entity::Node::Orbit(testScene.GetCamera(),
    //             glm::vec3(0),
    //             5, cameraTheta, cameraPhi);
    //         updateTime = now;
    //         testScene.Update();
    //         Renderer::Update(renderer);
    //         Renderer::Render(renderer);
    //     }
    //     Window::Present(window, renderBuffer);
    //     fpsCounter.EndFrame();
    //     if (std::chrono::duration<double, std::milli>(now - printTime).count() >= 48) {
    //         printTime = now;
    //         fpsCounter.Print();
    //     }
    // }
    // Renderer::Unload(renderer, testScene);
    // return 0;
}
