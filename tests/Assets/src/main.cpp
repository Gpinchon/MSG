#include <MSG/Assets/Asset.hpp>
#include <MSG/Assets/Parser.hpp>
#include <MSG/Assets/Parsers.hpp>
#include <MSG/Assets/Uri.hpp>

#include <MSG/ECS/Registry.hpp>

#include <MSG/Buffer.hpp>
#include <MSG/Core/Name.hpp>
#include <MSG/Image.hpp>
#include <MSG/Scene.hpp>

#include <MSG/Tools/Base.hpp>
#include <MSG/Tools/ScopedTimer.hpp>

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iterator>
#include <vector>

#include <gtest/gtest.h>

#include "./TestURIs.hpp"

using namespace Msg;

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
        ASSERT_EQ(image->GetSize(), glm::uvec3(64, 64, 1));
        ASSERT_EQ(image->GetPixelDescriptor().GetSizedFormat(), PixelSizedFormat::Uint8_NormalizedRGB);
    }
}

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
