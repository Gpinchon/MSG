#include <MSG/Renderer.hpp>
#include <MSG/Renderer/Structs.hpp>

#include <gtest/gtest.h>

using namespace Msg;

TEST(Renderer, Creation)
{
    Renderer::CreateRendererInfo rendererInfo {
        .name               = "UnitTest",
        .applicationVersion = 100
    };
    Renderer::RendererSettings rendererSettings {
        .ssao = { .strength = 0 }
    };
    auto renderer = Renderer::Create(rendererInfo, rendererSettings);
    ASSERT_FALSE(renderer == nullptr);
    Renderer::Update(renderer);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
