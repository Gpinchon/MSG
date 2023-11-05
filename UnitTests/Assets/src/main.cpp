#include <Assets/Asset.hpp>
#include <Assets/Parser.hpp>
#include <Assets/Parsers.hpp>
#include <Assets/Uri.hpp>

#include <ECS/Registry.hpp>

#include <SG/Component/Name.hpp>
#include <SG/Core/Buffer/Buffer.hpp>
#include <SG/Core/Image/Image.hpp>
#include <SG/Scene/Scene.hpp>

#include <Tools/Base.hpp>
#include <Tools/ScopedTimer.hpp>

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iterator>
#include <vector>

#include <gtest/gtest.h>

#include "./TestURIs.hpp"

using namespace TabGraph;

TEST(Assets, Binary)
{
    Assets::InitParsers();
    const std::string s = "Hello World !";
    std::vector<std::byte> v;
    std::transform(s.begin(), s.end(), std::back_inserter(v), [](const auto& c) { return std::byte(c); });
    Assets::Uri uri("data:application/octet-stream," + Tools::Base32::Encode(v));
    auto asset = Assets::Parser::Parse(std::make_shared<Assets::Asset>(uri));
    for (const auto& object : asset->GetObjects()) {
        const std::shared_ptr<SG::Buffer> buffer = asset->Get<SG::Buffer>().front();
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
        const auto image = asset->Get<SG::Image>().front();
        ASSERT_EQ(asset->GetAssetType(), "image/jpeg");
        ASSERT_EQ(image->GetType(), SG::Image::Type::Image2D);
        ASSERT_EQ(image->GetSize(), glm::ivec3(64, 64, 1));
        ASSERT_EQ(image->GetPixelDescription().GetSizedFormat(), SG::Pixel::SizedFormat::Uint8_NormalizedRGB);
    }
}

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    //if (argc >= 2) {
    //    Assets::InitParsers();
    //    const auto path = std::filesystem::path(argv[1]);
    //    std::shared_ptr<Assets::Asset> asset;
    //    {
    //        auto timer    = Tools::ScopedTimer("Asset parsing");
    //        auto file     = std::make_shared<Assets::Asset>(path);
    //        auto registry = ECS::DefaultRegistry::Create();
    //        file->SetECSRegistry(registry);
    //        asset = Assets::Parser::Parse(file);
    //        registry->GetView<SG::Component::Name>().ForEach<SG::Component::Name>([](auto entity, auto& name) {
    //            std::cout << "Entity " << entity << ", Name : " << std::string(name) << "\n";
    //        });
    //        // registry->GetView<SG::Component::Name>().ForEach<SG::Component::Name>([](auto& name) {
    //        //     std::cout << "\"Name\" : \"" << std::string(name) << "\"\n";
    //        // });
    //        std::cout << std::endl;
    //    }
    //}
    return RUN_ALL_TESTS();
}
