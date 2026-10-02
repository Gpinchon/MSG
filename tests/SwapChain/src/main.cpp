#include <MSG/Renderer.hpp>
#include <MSG/Renderer/RenderBuffer.hpp>
#include <MSG/Renderer/Structs.hpp>
#include <MSG/Window/Window.hpp>
#include <MSG/Assets/Asset.hpp>
#include <MSG/Assets/Parsers.hpp>
#include <MSG/Assets/Parser.hpp>
#include <MSG/Image.hpp>
#include <MSG/ImageUtils.hpp>
#include <MSG/Sampler.hpp>
#include <gtest/gtest.h>
#include <chrono>

#include "./TestURIs.hpp"

using namespace Msg;

constexpr uint32_t s_WindowWidth  = 1280;
constexpr uint32_t s_WindowHeight = 720;

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
        .width  = s_WindowWidth,
        .height = s_WindowHeight
    };
    auto renderer = Renderer::Create(rendererInfo, rendererSettings);
    ASSERT_FALSE(renderer == nullptr);
    auto renderBuffer = RenderBuffer::Create(renderer, renderBufferInfo);
    ASSERT_FALSE(renderBuffer == nullptr);
    // Upload our rubberducky to the render buffer
    {
        Assets::InitParsers();
        Assets::Uri uri(RubberDucky);
        auto asset = Assets::Parser::Parse(std::make_shared<Assets::Asset>(uri));
        auto image = *asset->GetCompatible<Image>().front();
        Sampler3D resizeSampler;
        resizeSampler.SetMinFilter(SamplerFilter::Linear);
        image = ImageResize(image, resizeSampler, { s_WindowWidth, s_WindowHeight, 1 });
        RenderBuffer::ImageCopyInfo copyInfo;
        copyInfo.dstOffset.x = (s_WindowWidth / 2) - (image.GetSize().x / 2);
        copyInfo.dstOffset.y = (s_WindowHeight / 2) - (image.GetSize().y / 2);
        RenderBuffer::UploadImage(renderer, renderBuffer, image, copyInfo);
    }
    Window::CreateWindowInfo info {
        .name   = "WindowCreationTest",
        .width  = s_WindowWidth,
        .height = s_WindowHeight
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
