#include <Assets/Asset.hpp>
#include <SG/Core/Material.hpp>
#include <SG/Core/Material/Extension/SpecularGlossiness.hpp>

#include <fstream>
#include <memory>
#include <strstream>

namespace TabGraph::Assets {
static std::vector<std::string> StrSplitWSpace(const std::string& input)
{
    std::istringstream buffer(input);
    return {
        (std::istream_iterator<std::string>(buffer)),
        std::istream_iterator<std::string>()
    };
}

static void StartMTLParsing(std::istream& a_Stream, const std::shared_ptr<Assets::Asset>& a_Container)
{
    std::string line;
    std::shared_ptr<SG::Material> currentMaterial;
    while (std::getline(a_Stream, line)) {
        if (line.empty())
            continue;
        const auto args = StrSplitWSpace(line);
        if (args.at(0) == "newmtl") {
            currentMaterial = std::make_shared<SG::Material>(args.at(1));
            currentMaterial->AddExtension(SG::SpecularGlossinessExtension {});
            a_Container->AddObject(currentMaterial);
        } else if (args.at(0) == "Kd") {
            currentMaterial->GetExtension<SG::SpecularGlossinessExtension>().diffuseFactor = {
                std::stof(args.at(1)),
                std::stof(args.at(2)),
                std::stof(args.at(3)),
                1.f
            };
        } else if (args.at(0) == "Tr") {
            currentMaterial->GetExtension<SG::SpecularGlossinessExtension>().diffuseFactor.a *= std::stof(args.at(1));
        }
    }
}

std::shared_ptr<Assets::Asset> ParseMTLLIB(const std::shared_ptr<Assets::Asset>& a_Container)
{
    if (a_Container->GetUri().GetScheme() == "file") {
        auto stream = std::ifstream(a_Container->GetUri().DecodePath(), std::ios_base::in);
        StartMTLParsing(stream, a_Container);
    } else if (a_Container->GetUri().GetScheme() == "data") {
        auto binary = DataUri(a_Container->GetUri()).Decode();
        auto stream = std::istrstream(reinterpret_cast<const char*>(binary.data()), binary.size());
        StartMTLParsing(stream, a_Container);
    }
    a_Container->SetLoaded(true);
    return a_Container;
}
}
