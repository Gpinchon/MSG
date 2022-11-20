#include <Assets/Parsers.hpp>
#include <Assets/Parser.hpp>
#include <Assets/Asset.hpp>
#include <Assets/Uri.hpp>

#include <SG/Buffer/Buffer.hpp>
#include <SG/Visitor/SerializeVisitor.hpp>
#include <SG/Node/Scene.hpp>

#include <Tools/Base.hpp>

#include <algorithm>
#include <vector>
#include <iterator>
#include <iostream>
#include <cassert>

using namespace TabGraph;

void TestBinaryParser() {
    const std::string s = "Hello World !";
    std::vector<std::byte> v;
    std::transform(s.begin(), s.end(), std::back_inserter(v), [](const auto& c) { return std::byte(c); });
    Assets::Uri uri("data:application/octet-stream," + Tools::Base32::Encode(v));
    auto asset = Assets::Parser::AddParsingTask(std::make_shared<Assets::Asset>(uri)).get();
    for (const auto& object : asset->assets) {
        std::cout << object->GetName() << std::endl;
        const auto buffer = asset->Get<SG::Buffer>().front();
        assert(buffer->size() == s.size());
        assert(std::memcmp(buffer->data(), s.data(), s.size()) == 0);
        buffer->Accept(SG::SerializeVisitor(std::cout, SG::NodeVisitor::Mode::VisitChildren));
        
    }
}

int main(int argc, char const *argv[])
{
    Assets::InitParsers();
    TestBinaryParser();
    if (argc >= 2) {
        const auto path = std::filesystem::path(argv[1]);
        Assets::Uri uri(path);
        auto asset = Assets::Parser::AddParsingTask(std::make_shared<Assets::Asset>(uri)).get();
        SG::SerializeVisitor serializeVisitor(std::cout, SG::NodeVisitor::Mode::VisitChildren);
        asset->Get<SG::Scene>().front()->Accept(serializeVisitor);
    }
    return 0;
}