#include <Assets/Asset.hpp>
#include <Assets/Parser.hpp>
#include <SG/Core/Image/Image.hpp>
#include <SG/Core/Material.hpp>
#include <SG/Core/Material/Extension/Base.hpp>
#include <SG/Core/Material/Extension/SpecularGlossiness.hpp>
#include <SG/Core/Texture/Texture.hpp>

#include <algorithm>
#include <fstream>
#include <memory>
#include <strstream>

namespace TabGraph::Assets {
using TextureCache = std::unordered_map<std::filesystem::path, std::shared_ptr<SG::Texture>>;
static std::vector<std::string> StrSplitWSpace(const std::string& input)
{
    std::istringstream buffer(input);
    return {
        (std::istream_iterator<std::string>(buffer)),
        std::istream_iterator<std::string>()
    };
}

static auto LoadTexture(TextureCache& a_TextureCache, const Uri& a_Uri, const std::shared_ptr<Assets::Asset>& a_Container)
{
    if (a_TextureCache.contains(a_Uri.DecodePath())) {
        return a_TextureCache.at(a_Uri.DecodePath());
    }
    auto asset            = std::make_shared<Assets::Asset>(a_Uri);
    asset->parsingOptions = a_Container->parsingOptions;
    asset                 = Parser::Parse(asset);
    a_Container->MergeObjects(asset);
    auto texture                       = std::make_shared<SG::Texture>(SG::TextureType::Texture2D, asset->GetCompatible<SG::Image>().front());
    a_TextureCache[a_Uri.DecodePath()] = texture;
    return texture;
}

static std::filesystem::path GetFilePath(const std::string& a_Arg0, const std::string& a_Line, const std::filesystem::path& a_ParentPath)
{
    std::string file = a_Line;
    file.erase(file.find(a_Arg0), a_Arg0.size());
    file.erase(file.begin(), std::find_if(file.begin(), file.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    return a_ParentPath / file;
}

static void StartMTLParsing(std::istream& a_Stream, const std::shared_ptr<Assets::Asset>& a_Container)
{
    std::string line;
    TextureCache textureCache;
    std::shared_ptr<SG::Material> currentMaterial;
    SG::SpecularGlossinessExtension* specGloss = nullptr;
    SG::BaseExtension* base                    = nullptr;
    std::filesystem::path parentPath           = a_Container->GetUri().DecodePath().parent_path();
    while (std::getline(a_Stream, line)) {
        if (line.empty())
            continue;
        const auto args = StrSplitWSpace(line);
        if (args.at(0) == "newmtl") {
            currentMaterial = std::make_shared<SG::Material>(args.at(1));
            currentMaterial->AddExtension(SG::BaseExtension {});
            currentMaterial->AddExtension(SG::SpecularGlossinessExtension {});
            base      = &currentMaterial->GetExtension<SG::BaseExtension>();
            specGloss = &currentMaterial->GetExtension<SG::SpecularGlossinessExtension>();
            a_Container->AddObject(currentMaterial);
        } else if (args.at(0) == "Kd") {
            specGloss->diffuseFactor = {
                std::stof(args.at(1)),
                std::stof(args.at(2)),
                std::stof(args.at(3)),
                1.f
            };
        } else if (args.at(0) == "Ks") {
            specGloss->specularFactor = {
                0.04 + 0.01 * std::stof(args.at(1)),
                0.04 + 0.01 * std::stof(args.at(2)),
                0.04 + 0.01 * std::stof(args.at(3))
            };
        } else if (args.at(0) == "Ke") {
            base->emissiveFactor = {
                std::stof(args.at(1)),
                std::stof(args.at(2)),
                std::stof(args.at(3))
            };
        } else if (args.at(0) == "Ns") {
            auto shininess              = std::clamp(std::stof(args.at(1)), 0.f, 500.f);
            auto glossiness             = shininess / 500.f;
            specGloss->glossinessFactor = glossiness;
        } else if (args.at(0) == "Tr") {
            specGloss->diffuseFactor.a *= std::max(0.f, 1 - std::stof(args.at(1)));
            if (specGloss->diffuseFactor.a < 1) {
                base->alphaMode   = SG::BaseExtension::AlphaMode::Blend;
                base->doubleSided = true;
            }
        } else if (args.at(0) == "map_Kd") {
            specGloss->diffuseTexture.textureSampler.texture = LoadTexture(textureCache, GetFilePath(args.at(0), line, parentPath), a_Container);
            if (specGloss->diffuseTexture.textureSampler.texture->GetPixelDescription().GetHasAlpha()) {
                base->alphaMode   = SG::BaseExtension::AlphaMode::Blend;
                base->doubleSided = true;
            }
        } else if (args.at(0) == "map_Ks") {
            specGloss->specularGlossinessTexture.textureSampler.texture = LoadTexture(textureCache, GetFilePath(args.at(0), line, parentPath), a_Container);
        } else if (args.at(0) == "map_Ke") {
            base->emissiveTexture.textureSampler.texture = LoadTexture(textureCache, GetFilePath(args.at(0), line, parentPath), a_Container);
        } else if (args.at(0) == "map_Bump") {
            base->normalTexture.textureSampler.texture = LoadTexture(textureCache, GetFilePath(args.at(0), line, parentPath), a_Container);
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
