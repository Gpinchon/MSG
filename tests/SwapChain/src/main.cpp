#include <MSG/Renderer.hpp>
#include <MSG/Renderer/RenderBuffer.hpp>
#include <MSG/Renderer/Structs.hpp>
#include <MSG/Window/Window.hpp>

#include <gtest/gtest.h>

#include <chrono>

using namespace Msg;

TEST(Window, Creation)
{
    Renderer::CreateRendererInfo rendererInfo {
        .name               = "UnitTest",
        .applicationVersion = 100
    };
    Renderer::RendererSettings rendererSettings {
        .ssao = { .strength = 0 }
    };
    RenderBuffer::CreateRenderBufferInfo renderBufferInfo {
        .width  = 1280,
        .height = 720
    };
    auto renderer = Renderer::Create(rendererInfo, rendererSettings);
    ASSERT_FALSE(renderer == nullptr);
    auto renderBuffer = RenderBuffer::Create(renderer, renderBufferInfo);
    ASSERT_FALSE(renderBuffer == nullptr);
    Window::CreateWindowInfo info {
        .name   = "WindowCreationTest",
        .width  = 1280,
        .height = 720
    };
    auto window = Window::Create(renderer, info);
    ASSERT_FALSE(window == nullptr);
    Renderer::Update(renderer);
    Window::Show(window);
    std::chrono::time_point startTime = std::chrono::system_clock::now();
    while ((std::chrono::system_clock::now() - startTime) < std::chrono::seconds(10)) {
        Window::WaitSwapChain(window);
        Window::Present(window, renderBuffer);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
