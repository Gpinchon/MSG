#include <Assets/Parsers.hpp>
#include <Assets/Parser.hpp>
#include <Assets/Asset.hpp>
#include <Assets/Uri.hpp>
#include <Assets/Visitor/SerializeVisitor.hpp>

#include <SG/Buffer/Buffer.hpp>
#include <SG/Node/Scene.hpp>

#include <SG/Visitor/SearchVisitor.hpp>

#include <Tools/Base.hpp>
#include <Tools/ScoppedTimer.hpp>

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
    for (const auto& object : asset->assets) {
        const std::shared_ptr<SG::Buffer> buffer = asset->Get<SG::Buffer>().front();
        assert(buffer->size() == s.size());
        assert(std::memcmp(buffer->data(), s.data(), s.size()) == 0);
        buffer->Accept(Assets::SerializeVisitor(std::cout, SG::NodeVisitor::Mode::VisitChildren));
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
            auto timer = ScoppedTimer("Asset parsing");
            asset = Assets::Parser::Parse(std::make_shared<Assets::Asset>(path));
        }
        std::ofstream file("./assets.json");
        for (auto& obj : asset->assets)
            obj->Accept(Assets::SerializeVisitor(file, SG::NodeVisitor::Mode::VisitOnce));
    }
    return 0;
}