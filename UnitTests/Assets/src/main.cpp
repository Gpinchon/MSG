#include <Assets/Parsers.hpp>
#include <Assets/Parser.hpp>
#include <Assets/Asset.hpp>
#include <Assets/Uri.hpp>

#include <SG/Buffer/Buffer.hpp>

#include <Tools/Base.hpp>

#include <algorithm>
#include <vector>
#include <iterator>
#include <iostream>
#include <cassert>

using namespace TabGraph;

int main(int argc, char const *argv[])
{
    Assets::InitParsers();
    const std::string s = "Hello World !";
    std::vector<std::byte> v;
    std::transform(s.begin(), s.end(), std::back_inserter(v), [](const auto& c) { return std::byte(c); });
    Assets::Uri uri("data:application/octet-stream," + Tools::Base32::Encode(v));
    auto asset = Assets::Parser::AddParsingTask(std::make_shared<Assets::Asset>(uri)).get();
    for (const auto& object : asset->assets) {
        std::cout << object->GetName() << std::endl;
        auto &bufferData = asset->Get<SG::Buffer>().front()->GetData();
        assert(bufferData.size() == s.size());
        assert(std::memcmp(bufferData.data(), s.data(), s.size()) == 0);
    }
    return 0;
}