#include <Assets/Asset.hpp>
#include <Assets/Parser.hpp>
#include <Assets/Parsers.hpp>
#include <Assets/Uri.hpp>

#include <ECS/Registry.hpp>

#include <Buffer.hpp>
#include <Core/Name.hpp>
#include <Image.hpp>
#include <Scene.hpp>

#include <Tools/Base.hpp>
#include <Tools/ScopedTimer.hpp>

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iterator>
#include <vector>

#include <gtest/gtest.h>

#include "./TestURIs.hpp"

using namespace MSG;

TEST(Assets, Binary)
{
    Assets::InitParsers();
    const std::string s = "Hello World !";
    std::vector<std::byte> v;
    std::transform(s.begin(), s.end(), std::back_inserter(v), [](const auto& c) { return std::byte(c); });
    Assets::Uri uri("data:application/octet-stream," + Tools::Base32::Encode(v));
    auto asset = Assets::Parser::Parse(std::make_shared<Assets::Asset>(uri));
    for (const auto& object : asset->GetObjects()) {
        const std::shared_ptr<Buffer> buffer = asset->Get<Buffer>().front();
        ASSERT_EQ(buffer->size(), s.size());
        ASSERT_EQ(std::memcmp(buffer->data(), s.data(), s.size()), 0);
    }
}

TEST(Assets, JPEG)
{
    Assets::InitParsers();
    Assets::Uri uri(RubberDucky);
    auto asset = Assets::Parser::Parse(std::make_shared<Assets::Asset>(uri));
    for (const auto& object : asset->GetObjects()) {
        const auto image = asset->GetCompatible<Image>().front();
        ASSERT_EQ(asset->GetAssetType(), "image/jpeg");
        ASSERT_EQ(image->GetType(), ImageType::Image2D);
        ASSERT_EQ(image->GetSize(), glm::uvec3(64, 64, 1));
        ASSERT_EQ(image->GetPixelDescriptor().GetSizedFormat(), PixelSizedFormat::Uint8_NormalizedRGB);
    }
}

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
