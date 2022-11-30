#include <Assets/Parsers.hpp>
#include <Assets/Parser.hpp>
#include <Assets/Asset.hpp>
#include <Assets/Uri.hpp>

#include <ECS/Registry.hpp>

#include <SG/Core/Buffer/Buffer.hpp>
#include <SG/Scene/Scene.hpp>
#include <SG/Component/Name.hpp>

#include <Tools/Base.hpp>
#include <Tools/ScopedTimer.hpp>

#include <algorithm>
#include <vector>
#include <iterator>
#include <fstream>
#include <cassert>

using namespace TabGraph;

void TestBinaryParser() {
    const std::string s = "Hello World !";
    std::vector<std::byte> v;
    std::transform(s.begin(), s.end(), std::back_inserter(v), [](const auto& c) { return std::byte(c); });
    Assets::Uri uri("data:application/octet-stream," + Tools::Base32::Encode(v));
    auto asset = Assets::Parser::Parse(std::make_shared<Assets::Asset>(uri));
    for (const auto& object : asset->GetAssets()) {
        const std::shared_ptr<SG::Buffer> buffer = asset->Get<SG::Buffer>().front();
        assert(buffer->size() == s.size());
        assert(std::memcmp(buffer->data(), s.data(), s.size()) == 0);
    }
}

int main(int argc, char const *argv[])
{
    Assets::InitParsers();
    TestBinaryParser();
    if (argc >= 2) {
        const auto path = std::filesystem::path(argv[1]);
        std::shared_ptr<Assets::Asset> asset;
        {
            auto timer = Tools::ScopedTimer("Asset parsing");
            auto file = std::make_shared<Assets::Asset>(path);
            auto registry = ECS::DefaultRegistry::Create();
            file->SetECSRegistry(registry);
            asset = Assets::Parser::Parse(file);
            /*
            registry->GetView<SG::Name>().ForEach<SG::Name>([](auto entity, auto& name){
                std::cout << "Entity " << entity << ", Name : " << std::string(name) << "\n";
            });
            */
            registry->GetView<SG::Component::Name>().ForEach<SG::Component::Name>([](auto& name) {
                std::cout << "\"Name\" : \"" << std::string(name) << "\"\n";
            });
            std::cout << std::endl;
        }
    }
    int v;
    std::cin >> v;
    return 0;
}