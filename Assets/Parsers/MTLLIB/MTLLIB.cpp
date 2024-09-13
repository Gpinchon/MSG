#include <Assets/Asset.hpp>
#include <Assets/Parser.hpp>
#include <SG/Component/Name.hpp>
#include <SG/Core/Image/Image2D.hpp>
#include <SG/Core/Material.hpp>
#include <SG/Core/Material/Extension/Base.hpp>
#include <SG/Core/Material/Extension/SpecularGlossiness.hpp>
#include <SG/Core/Texture/Texture.hpp>
#include <Tools/ThreadPool.hpp>

#include <glm/common.hpp>

#include <algorithm>
#include <fstream>
#include <memory>
#include <strstream>
#include <unordered_set>

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

static std::shared_ptr<SG::Texture> LoadTexture(const Uri& a_Uri, const std::shared_ptr<Assets::Asset>& a_Container)
{
    if (a_Uri.DecodePath().empty())
        return nullptr;
    auto asset                         = std::make_shared<Assets::Asset>(a_Uri);
    asset->parsingOptions              = a_Container->parsingOptions;
    asset                              = Parser::Parse(asset);
    std::shared_ptr<SG::Image2D> image = asset->GetCompatible<SG::Image2D>().front();
    auto texture                       = std::make_shared<SG::Texture>(SG::TextureType::Texture2D, image);
    texture->GenerateMipmaps();
    if (a_Container->parsingOptions.texture.compress)
        texture->Compress(a_Container->parsingOptions.texture.compressionQuality);
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

struct Material {
    SG::Component::Name name = "";
    uint8_t illum            = 2;
    float specular           = 90.f;
    float transparency       = 0.f;
    glm::vec3 emissiveColor  = { 1.f, 1.f, 1.f };
    glm::vec3 ambientColor   = { 0.f, 0.f, 0.f };
    glm::vec3 diffuseColor   = { 1.f, 1.f, 1.f };
    glm::vec3 specularColor  = { 0.f, 0.f, 0.f };
    std::filesystem::path emissiveTexture;
    std::filesystem::path ambientTexture;
    std::filesystem::path diffuseTexture;
    std::filesystem::path specularTexture;
    std::filesystem::path bumpTexture;
};

static void StartMTLParsing(std::istream& a_Stream, const std::shared_ptr<Assets::Asset>& a_Container)
{
    std::string line;
    std::vector<Material> materials;
    std::filesystem::path parentPath = a_Container->GetUri().DecodePath().parent_path();
    while (std::getline(a_Stream, line)) {
        if (line.empty())
            continue;
        const auto args = StrSplitWSpace(line);
        if (args.at(0) == "newmtl") {
            materials.push_back({ .name = args.at(1) });
        } else if (args.at(0) == "Kd") {
            materials.back().diffuseColor = {
                std::stof(args.at(1)),
                std::stof(args.at(2)),
                std::stof(args.at(3))
            };

        } else if (args.at(0) == "Ks") {
            materials.back().specularColor = {
                std::stof(args.at(1)),
                std::stof(args.at(2)),
                std::stof(args.at(3))
            };
        } else if (args.at(0) == "Ke") {
            materials.back().emissiveColor = {
                std::stof(args.at(1)),
                std::stof(args.at(2)),
                std::stof(args.at(3))
            };
        } else if (args.at(0) == "Ns") {
            materials.back().specular = std::clamp(std::stof(args.at(1)), 0.f, 500.f);
        } else if (args.at(0) == "Tr") {
            materials.back().transparency = std::stof(args.at(1));
        } else if (args.at(0) == "map_Kd") {
            materials.back().diffuseTexture = GetFilePath(args.at(0), line, parentPath);
        } else if (args.at(0) == "map_Ks") {
            materials.back().specularTexture = GetFilePath(args.at(0), line, parentPath);
        } else if (args.at(0) == "map_Ke") {
            materials.back().emissiveTexture = GetFilePath(args.at(0), line, parentPath);
        } else if (args.at(0) == "map_Bump") {
            materials.back().bumpTexture = GetFilePath(args.at(0), line, parentPath);
        } else if (args.at(0) == "illum") {
            materials.back().illum = std::stoi(args.at(1));
        }
    }
    std::unordered_set<std::filesystem::path> texturePaths;
    for (auto& material : materials) {
        texturePaths.insert(material.ambientTexture);
        texturePaths.insert(material.bumpTexture);
        texturePaths.insert(material.diffuseTexture);
        texturePaths.insert(material.emissiveTexture);
        texturePaths.insert(material.specularTexture);
    }
    Tools::ThreadPool threadPool;
    TextureCache textures;
    for (auto& texturePath : texturePaths) {
        threadPool.PushCommand([&texture = textures[texturePath], texturePath, a_Container]() mutable {
            texture = LoadTexture(texturePath, a_Container);
        },
            false);
    }
    threadPool.Wait();
    for (auto& material : materials) {
        auto currentMaterial = std::make_shared<SG::Material>(material.name);
        currentMaterial->AddExtension(SG::BaseExtension {});
        currentMaterial->AddExtension(SG::SpecularGlossinessExtension {});
        auto base                                        = &currentMaterial->GetExtension<SG::BaseExtension>();
        auto specGloss                                   = &currentMaterial->GetExtension<SG::SpecularGlossinessExtension>();
        specGloss->diffuseFactor                         = { material.diffuseColor, 1 - material.transparency };
        specGloss->specularFactor                        = { 0.04f + 0.01f * material.specularColor };
        specGloss->glossinessFactor                      = material.specular / 500.f;
        specGloss->diffuseTexture.textureSampler.texture = textures.at(material.diffuseTexture);
        base->emissiveFactor                             = material.emissiveColor;
        base->emissiveTexture.textureSampler.texture     = textures.at(material.emissiveTexture);
        base->normalTexture.textureSampler.texture       = textures.at(material.bumpTexture);
        auto textureHasAlpha                             = specGloss->diffuseTexture.textureSampler.texture != nullptr && specGloss->diffuseTexture.textureSampler.texture->GetPixelDescription().GetHasAlpha();
        if (material.illum == 4 || material.illum == 9) {
            base->alphaMode   = SG::BaseExtension::AlphaMode::Blend;
            base->doubleSided = true;
        } else if (material.illum == 0 || material.illum == 1) {
            base->unlit = true;
        }
        a_Container->AddObject(currentMaterial);
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
