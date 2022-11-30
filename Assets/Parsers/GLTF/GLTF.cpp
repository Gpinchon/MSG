/*
* @Author: gpinchon
* @Date:   2020-08-07 18:36:53
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-11 16:17:13
*/

#include <Assets/Asset.hpp>
#include <Assets/Parser.hpp>

#include <SG/Core/Buffer/Accessor.hpp>
#include <SG/Core/Buffer/Buffer.hpp>
#include <SG/Core/Buffer/View.hpp>
#include <SG/Core/Image/Image.hpp>
#include <SG/Core/Material/Extension/Sheen.hpp>
#include <SG/Core/Material/Extension/MetallicRoughness.hpp>
#include <SG/Core/Material/Extension/SpecularGlossiness.hpp>
#include <SG/Core/Material/TextureInfo.hpp>
#include <SG/Core/Texture/Texture2D.hpp>
#include <SG/Core/Texture/Sampler.hpp>
#include <SG/Core/Material.hpp>
#include <SG/Core/Primitive.hpp>

#include <SG/Entity/Camera.hpp>
#include <SG/Entity/Node/Node.hpp>
#include <SG/Entity/Light/Directional.hpp>

#include <SG/Component/Mesh.hpp>
#include <SG/Component/Skin.hpp>

#include <SG/Scene/Animation.hpp>
#include <SG/Scene/Animation/Channel.hpp>
#include <SG/Scene/Scene.hpp>

#include <Tools/Debug.hpp>
#include <Tools/ScopedTimer.hpp>


#define RAPIDJSON_NOEXCEPT_ASSERT(x)
#define RAPIDJSON_ASSERT(x)                                                                                         \
    {                                                                                                               \
        if (x)                                                                                                      \
            ;                                                                                                       \
        else                                                                                                        \
            throw std::runtime_error("JSON error in " + std::string(__FILE__) + " at " + std::to_string(__LINE__)); \
    };

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

#include <glm/ext.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/string_cast.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>

#define _DEBUG

namespace TabGraph::Assets {
namespace GLTF {
struct Dictionary {
    void Add(const std::string& a_TypeName, const std::shared_ptr<SG::Object> a_Object) {
        objects[a_TypeName].push_back(a_Object);
    }
    auto& Get(const std::string& a_TypeName) {
        return objects[a_TypeName];
    }
    template<typename T>
    auto Get(const std::string& a_TypeName, const size_t& a_Index) const {
        const auto& obj = objects.at(a_TypeName).at(a_Index);
        if (obj->IsCompatible(typeid(T))) return std::static_pointer_cast<T>(obj);
        throw std::runtime_error("Incompatible types");
    }
    std::map<std::string, std::vector<ECS::DefaultRegistry::EntityRefType>> entities;
    std::map<std::string, std::vector<std::shared_ptr<SG::Object>>> objects;
};
enum class ComponentType {
    GLTFByte = 5120,
    GLTFUByte = 5121,
    GLTFShort = 5122,
    GLTFUShort = 5123,
    GLTFUInt = 5125,
    GLTFFloat = 5126,
    MaxValue
};
enum class TextureWrap {
    ClampToEdge = 33071,
    MirroredRepeat = 33648,
    Repeat = 10497
};
enum class TextureFilter {
    Nearest = 9728,
    Linear = 9729,
    NearestMipmapNearest = 9984,
    LinearMipmapNearest = 9985,
    NearestMipmapLinear = 9986,
    LinearMipmapLinear = 9987
};
enum class BufferViewType {
    Array = 34962,
    ElementArray = 34963
};
enum class DrawingMode {
    Points = 0,
    Lines = 1,
    LineLoop = 2,
    LineStrip = 3,
    Triangles = 4,
    TriangleStrip = 5,
    TriangleFan = 6
};

static inline auto GetFilter(TextureFilter filter) {
    switch (filter) {
    case TextureFilter::Nearest:
        return SG::TextureSampler::Filter::Nearest;
    case TextureFilter::Linear:
        return SG::TextureSampler::Filter::Linear;
    case TextureFilter::NearestMipmapNearest:
        return SG::TextureSampler::Filter::NearestMipmapNearest;
    case TextureFilter::LinearMipmapNearest:
        return SG::TextureSampler::Filter::LinearMipmapNearest;
    case TextureFilter::NearestMipmapLinear:
        return SG::TextureSampler::Filter::NearestMipmapLinear;
    case TextureFilter::LinearMipmapLinear:
        return SG::TextureSampler::Filter::LinearMipmapLinear;
    default:
        throw std::runtime_error("Unknown Texture filter");
    }
}

static inline auto GetWrap(const TextureWrap& wrap) {
    switch (wrap)
    {
    case TextureWrap::ClampToEdge:
        return SG::TextureSampler::Wrap::ClampToEdge;
    case TextureWrap::MirroredRepeat:
        return SG::TextureSampler::Wrap::MirroredRepeat;
    case TextureWrap::Repeat:
        return SG::TextureSampler::Wrap::Repeat;
    default:
        throw std::runtime_error("Unknown Texture Wrap mode");
    }
}

static inline auto GetAccessorComponentNbr(const std::string& a_type)
{
    if (a_type == "SCALAR") return 1u;
    else if (a_type == "VEC2") return 2u;
    else if (a_type == "VEC3") return 3u;
    else if (a_type == "VEC4") return 4u;
    else if (a_type == "MAT2") return 4u;
    else if (a_type == "MAT3") return 9u;
    else if (a_type == "MAT4") return 16u;
    else throw std::runtime_error("Unknown Buffer Accessor type");
}

static inline auto GetAccessorComponentType(const ComponentType& a_componentType) {
    switch (a_componentType)
    {
    case (ComponentType::GLTFByte):
        return SG::BufferAccessor::ComponentType::Int8;
    case (ComponentType::GLTFUByte):
        return SG::BufferAccessor::ComponentType::Uint8;
    case (ComponentType::GLTFShort):
        return SG::BufferAccessor::ComponentType::Int16;
    case (ComponentType::GLTFUShort):
        return SG::BufferAccessor::ComponentType::Uint16;
    case (ComponentType::GLTFUInt):
        return SG::BufferAccessor::ComponentType::Int32;
    case (ComponentType::GLTFFloat):
        return SG::BufferAccessor::ComponentType::Float32;
    default:
        throw std::runtime_error("Unknown Accessor component type");
    }
}

static inline auto GetGeometryDrawingMode(DrawingMode mode)
{
    switch (mode) {
    case DrawingMode::Points:
        return SG::Primitive::DrawingMode::Points;
    case DrawingMode::Lines:
        return SG::Primitive::DrawingMode::Lines;
    case DrawingMode::LineLoop:
        return SG::Primitive::DrawingMode::LineLoop;
    case DrawingMode::LineStrip:
        return SG::Primitive::DrawingMode::LineStrip;
    case DrawingMode::Triangles:
        return SG::Primitive::DrawingMode::Triangles;
    case DrawingMode::TriangleStrip:
        return SG::Primitive::DrawingMode::TriangleStrip;
    case DrawingMode::TriangleFan:
        return SG::Primitive::DrawingMode::TriangleFan;
    default:
        return SG::Primitive::DrawingMode::Unknown;
    }
}

Uri CreateUri(const std::filesystem::path& parentPath, const std::string& dataPath)
{
    auto bufferPath{ std::filesystem::path(dataPath) };
    if (bufferPath.string().rfind("data:", 0) == 0)
        return Uri(bufferPath.string());
    else {
        if (!bufferPath.is_absolute())
            bufferPath = parentPath / bufferPath;
        return Uri(bufferPath);
    }
}

template<typename T>
T Parse(const rapidjson::Value& a_Value, const std::string& a_Name, bool a_Optional = false, const T& a_Default = {}) {
    if (a_Value.HasMember(a_Name.c_str())) return a_Value[a_Name.c_str()].Get<T>();
    else if (!a_Optional) throw std::runtime_error("Could not find value " + a_Name);
    return a_Default;
}
template<>
std::string Parse(const rapidjson::Value& a_Value, const std::string& a_Name, bool a_Optional, const std::string& a_Default)
{
    if (a_Value.HasMember(a_Name.c_str())) return a_Value[a_Name.c_str()].GetString();
    else if (!a_Optional) throw std::runtime_error("Could not find value " + a_Name);
    return a_Default;
}

template<unsigned L, typename T>
glm::vec<L, T> Parse(const rapidjson::Value& a_Value, const std::string& a_Name, bool a_Optional = false, const glm::vec<L, T>& a_Default = {})
{
    if (a_Value.HasMember(a_Name.c_str())) {
        glm::vec<L, T> ret;
        auto vector(a_Value[a_Name.c_str()].GetArray());
        for (unsigned i = 0; i < L; ++i) ret[i] = vector[i].GetFloat();
        return ret;
    }
    else if (!a_Optional) throw std::runtime_error("Could not find value " + a_Name);
    return a_Default;
}

template<>
glm::quat Parse(const rapidjson::Value& a_Value, const std::string& a_Name, bool a_Optional, const glm::quat& a_Default)
{
    if (a_Value.HasMember(a_Name.c_str())) {
        auto vector(a_Value[a_Name.c_str()].GetArray());
        return {
            vector[0].GetFloat(),
            vector[1].GetFloat(),
            vector[2].GetFloat(),
            vector[3].GetFloat()
        };
    }
    else if (!a_Optional) throw std::runtime_error("Could not find value " + a_Name);
    return a_Default;
}
}


static inline void ParseCameras(const rapidjson::Document& document, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!document.HasMember("cameras")) return;
#ifdef _DEBUG
    auto timer = Tools::ScopedTimer("Parsing cameras");
#endif
    for (const auto& camera : document["cameras"].GetArray()) {
        auto entity = SG::Camera::Create(a_AssetsContainer->GetECSRegistry());
        if (std::string(camera["type"].GetString()) == "perspective") {
            if (camera["perspective"].HasMember("zfar")) {
                SG::Component::Projection::Perspective projection;
                projection.zfar = GLTF::Parse(camera["perspective"], "zfar", false, projection.zfar);
                projection.znear = GLTF::Parse(camera["perspective"], "znear", true, projection.znear);
                projection.fov = GLTF::Parse(camera["perspective"], "fov", true, projection.fov);
                entity.GetComponent<SG::Component::Projection>() = projection;
            }
            else {
                SG::Component::Projection::PerspectiveInfinite projection;
                projection.znear = GLTF::Parse(camera["perspective"], "znear", true, projection.znear);
                projection.fov = glm::degrees(GLTF::Parse(camera["perspective"], "yfov", true, glm::radians(projection.fov)));
                entity.GetComponent<SG::Component::Projection>() = projection;
            }
        }
        else if (std::string(camera["type"].GetString()) == "orthographic") {
            SG::Component::Projection::Orthographic projection;
            entity.GetComponent<SG::Component::Projection>() = projection;
        }
        a_Dictionary.entities["cameras"].push_back(entity);
    }
}

static inline void ParseTextureSamplers(const rapidjson::Value& a_JSONValue, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!a_JSONValue.HasMember("samplers")) return;
    for (const auto& gltfSampler : a_JSONValue["samplers"].GetArray()) {
        auto sampler = std::make_shared<SG::TextureSampler>();
        auto magFilter = GLTF::TextureFilter(GLTF::Parse(gltfSampler, "magFilter", true, int(GLTF::TextureFilter::Linear)));
        auto minFilter = GLTF::TextureFilter(GLTF::Parse(gltfSampler, "minFilter", true, int(GLTF::TextureFilter::Linear)));
        auto wrapS     = GLTF::TextureWrap(GLTF::Parse(gltfSampler, "wrapS", true, int(GLTF::TextureWrap::ClampToEdge)));
        auto wrapT     = GLTF::TextureWrap(GLTF::Parse(gltfSampler, "wrapT", true, int(GLTF::TextureWrap::ClampToEdge)));
        auto wrapR     = GLTF::TextureWrap(GLTF::Parse(gltfSampler, "wrapR", true, int(GLTF::TextureWrap::ClampToEdge)));
        sampler->SetMagFilter(GLTF::GetFilter(magFilter));
        sampler->SetMinFilter(GLTF::GetFilter(minFilter));
        sampler->SetWrapS(GLTF::GetWrap(wrapS));
        sampler->SetWrapT(GLTF::GetWrap(wrapT));
        sampler->SetWrapR(GLTF::GetWrap(wrapR));
        a_Dictionary.Add("samplers", sampler);
    }
}

static inline void ParseTextures(const rapidjson::Value& a_JSONValue, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!a_JSONValue.HasMember("textures")) return;
#ifdef _DEBUG
    auto timer = Tools::ScopedTimer("Parsing textures");
#endif
    for (const auto& textureValue : a_JSONValue["textures"].GetArray()) {
        auto texture = std::make_shared<SG::Texture2D>();
        const auto source = GLTF::Parse(textureValue, "source", true, -1);
        const auto sampler = GLTF::Parse(textureValue, "sampler", true, -1);
        if (source > -1) texture->SetImage(a_Dictionary.Get<SG::Image>("images", source));
        if (sampler > -1) texture->SetSampler(a_Dictionary.Get<SG::TextureSampler>("samplers", sampler));
        texture->SetCompressed(a_AssetsContainer->parsingOptions.texture.compress);
        texture->SetCompressionQuality(a_AssetsContainer->parsingOptions.texture.compressionQuality);
        a_Dictionary.Add("textures", texture);
    }
}

static inline auto ParseTextureInfo(GLTF::Dictionary& a_Dictionary, const rapidjson::Value& a_JSONValue)
{
    SG::TextureInfo texture;
    texture.texture = a_Dictionary.Get<SG::Texture>("textures", GLTF::Parse<int>(a_JSONValue, "index"));
    texture.texCoord = GLTF::Parse(a_JSONValue, "texCoord", true, texture.texCoord);
    return texture;
}

static inline auto ParseSpecularGlossiness(GLTF::Dictionary& a_Dictionary, const rapidjson::Value& extension) {
    SG::SpecularGlossinessExtension specGloss;
    specGloss.SetDiffuseFactor(GLTF::Parse(extension, "diffuseFactor", true, specGloss.GetDiffuseFactor()));
    specGloss.SetSpecularFactor(GLTF::Parse(extension, "specularFactor", true, specGloss.GetSpecularFactor()));
    specGloss.SetGlossinessFactor(GLTF::Parse(extension, "glossinessFactor", true, specGloss.GetGlossinessFactor()));
    if (extension.HasMember("diffuseTexture")) {
        specGloss.SetDiffuseTexture(ParseTextureInfo(a_Dictionary, extension["diffuseTexture"]));
    }
    if (extension.HasMember("specularGlossinessTexture")) {
        specGloss.SetSpecularGlossinessTexture(ParseTextureInfo(a_Dictionary, extension["specularGlossinessTexture"]));
    }
    return specGloss;
}

static inline auto ParseSheen(GLTF::Dictionary& a_Dictionary, const rapidjson::Value& a_Extension) {
    SG::SheenExtension sheen;
    sheen.SetColorFactor(GLTF::Parse(a_Extension, "sheenColorFactor", true, sheen.GetColorFactor()));
    sheen.SetRoughnessFactor(GLTF::Parse(a_Extension, "sheenRoughnessFactor", true, sheen.GetRoughnessFactor()));
    if (a_Extension.HasMember("sheenColorTexture")) {
        sheen.SetColorTexture(ParseTextureInfo(a_Dictionary, a_Extension["sheenColorTexture"]));
    }
    if (a_Extension.HasMember("sheenRoughnessTexture")) {
        sheen.SetRoughnessTexture(ParseTextureInfo(a_Dictionary, a_Extension["sheenRoughnessTexture"]));
    }
    return sheen;
}

static inline void ParseMaterialExtensions(GLTF::Dictionary& a_Dictionary, const rapidjson::Value& materialValue, std::shared_ptr<SG::Material> a_Material)
{
    for (const auto& extension : materialValue["extensions"].GetArray()) {
        if (std::string(extension.GetString()) == "KHR_materials_pbrSpecularGlossiness")
            a_Material->AddExtension(ParseSpecularGlossiness(a_Dictionary, extension));
        else if (std::string(extension.GetString()) == "KHR_materials_sheen")
            a_Material->AddExtension(ParseSheen(a_Dictionary, extension));
    }
}

static inline auto ParseMetallicRoughness(GLTF::Dictionary& a_Dictionary, const rapidjson::Value& a_Extension)
{
    SG::MetallicRoughnessExtension mra{};
    mra.SetColorFactor(GLTF::Parse(a_Extension, "baseColorFactor", true, mra.GetColorFactor()));
    mra.SetMetallicFactor(GLTF::Parse(a_Extension, "metallicFactor", true, mra.GetMetallicFactor()));
    mra.SetRoughnessFactor(GLTF::Parse(a_Extension, "roughnessFactor", true, mra.GetRoughnessFactor()));
    if (a_Extension.HasMember("baseColorTexture"))
        mra.SetColorTexture(ParseTextureInfo(a_Dictionary, a_Extension["baseColorTexture"]));
    if (a_Extension.HasMember("metallicRoughnessTexture"))
        mra.SetMetallicRoughnessTexture(ParseTextureInfo(a_Dictionary, a_Extension["metallicRoughnessTexture"]));
    return mra;
}

static inline void ParseMaterials(const rapidjson::Value& a_JSONValue, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!a_JSONValue.HasMember("materials")) return;
#ifdef _DEBUG
    auto timer = Tools::ScopedTimer("Parsing materials");
#endif
    for (const auto& materialValue : a_JSONValue["materials"].GetArray()) {
        auto material = std::make_shared<SG::Material>();
        material->SetName(GLTF::Parse(materialValue, "name", true, material->GetName()));
        material->SetAlphaCutoff(GLTF::Parse(materialValue, "alphaCutoff", true, material->GetAlphaCutoff()));
        material->SetDoubleSided(GLTF::Parse(materialValue, "doubleSided", true, material->GetDoubleSided()));
        material->SetEmissiveFactor(GLTF::Parse(materialValue, "emissiveFactor", true, material->GetEmissiveFactor()));
        auto alphaMode = GLTF::Parse<std::string>(materialValue, "alphaMode", true);
        if (alphaMode == "Opaque")      material->SetAlphaMode(SG::Material::AlphaMode::Opaque);
        else if (alphaMode == "MASK")   material->SetAlphaMode(SG::Material::AlphaMode::Mask);
        else if (alphaMode == "BLEND")  material->SetAlphaMode(SG::Material::AlphaMode::Blend);
        if (materialValue.HasMember("normalTexture")) {
            const auto& texInfo = materialValue["normalTexture"];
            SG::NormalTextureInfo texture = ParseTextureInfo(a_Dictionary, materialValue["normalTexture"]);
            texture.scale = GLTF::Parse(texInfo, "scale", true, texture.scale);
            material->SetNormalTexture(texture);
        }
        if (materialValue.HasMember("emissiveTexture"))
            material->SetEmissiveTexture(ParseTextureInfo(a_Dictionary, materialValue["emissiveTexture"]));
        if (materialValue.HasMember("occlusionTexture")) {
            SG::OcclusionTextureInfo texture = ParseTextureInfo(a_Dictionary, materialValue["occlusionTexture"]);
            texture.strength = GLTF::Parse(materialValue["occlusionTexture"], "strength", true, texture.strength);
            material->SetOcclusionTexture(texture);
        }
        if (materialValue.HasMember("pbrMetallicRoughness"))
            material->AddExtension(ParseMetallicRoughness(a_Dictionary, materialValue["pbrMetallicRoughness"]));
        if (materialValue.HasMember("extensions")) ParseMaterialExtensions(a_Dictionary, materialValue, material);
        a_Dictionary.Add("materials", material);
    }
}

static inline void ParseBuffers(const std::filesystem::path path, const rapidjson::Value& a_JSONValue, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!a_JSONValue.HasMember("buffers")) return;
#ifdef _DEBUG
    auto timer = Tools::ScopedTimer("Parsing buffers");
#endif
    std::vector<std::shared_ptr<Asset>> assetVector;
    for (const auto& bufferValue : a_JSONValue["buffers"].GetArray()) {
        auto asset { std::make_shared<Asset>() };
        asset->parsingOptions = a_AssetsContainer->parsingOptions;
        asset->SetUri(GLTF::CreateUri(path.parent_path(), bufferValue["uri"].GetString()));
        asset->SetName(GLTF::Parse(bufferValue, "name", true, asset->GetName()));
        assetVector.push_back(asset);
    }
    std::vector<std::future<std::shared_ptr<Asset>>> parsingFuture;
    for (const auto& asset : assetVector) parsingFuture.push_back(Parser::AddParsingTask(asset));
    for (auto& future : parsingFuture) {
        std::shared_ptr<SG::Buffer> buffer = future.get()->Get<SG::Buffer>().front();
        a_Dictionary.Add("buffers", buffer);
    }
}

static inline void ParseBufferViews(const rapidjson::Document& document, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!document.HasMember("bufferViews")) return;
#ifdef _DEBUG
    auto timer = Tools::ScopedTimer("Parsing bufferViews");
#endif
    for (const auto& bufferViewValue : document["bufferViews"].GetArray()) {
        auto bufferView(std::make_shared<SG::BufferView>());
        const auto buffer = a_Dictionary.Get<SG::Buffer>("buffers", bufferViewValue["buffer"].GetInt());
        bufferView->SetBuffer(buffer);
        bufferView->SetByteLength(bufferViewValue["byteLength"].GetInt());
        bufferView->SetName(GLTF::Parse(bufferViewValue, "name", true, bufferView->GetName()));
        bufferView->SetByteOffset(GLTF::Parse(bufferViewValue, "byteOffset", true, bufferView->GetByteOffset()));
        bufferView->SetByteStride(GLTF::Parse(bufferViewValue, "byteStride", true, bufferView->GetByteStride()));
        //if (bufferViewValue.HasMember("target"))
        //    bufferView->SetType(GLTF::GetBufferViewType(GLTF::BufferViewType(bufferViewValue["target"].GetInt())));
        a_Dictionary.Add("bufferViews", bufferView);
    }
}

static inline void ParseBufferAccessors(const rapidjson::Value& a_JSONValue, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!a_JSONValue.HasMember("accessors")) return;
#ifdef _DEBUG
    auto timer = Tools::ScopedTimer("Parsing accessors");
#endif
    for (const auto& gltfbufferAccessor : a_JSONValue["accessors"].GetArray()) {
        auto bufferAccessor = std::make_shared<SG::BufferAccessor>();
        std::shared_ptr<SG::BufferView> bufferView;
        bufferAccessor->SetName(GLTF::Parse(gltfbufferAccessor, "name", true, bufferAccessor->GetName()));
        bufferAccessor->SetSize(GLTF::Parse<size_t>(gltfbufferAccessor, "count"));
        bufferAccessor->SetComponentNbr(GLTF::GetAccessorComponentNbr(GLTF::Parse<std::string>(gltfbufferAccessor, "type")));
        bufferAccessor->SetComponentType(GLTF::GetAccessorComponentType(GLTF::ComponentType(GLTF::Parse<int>(gltfbufferAccessor, "componentType"))));
        if (const auto bufferViewIndex = GLTF::Parse(gltfbufferAccessor, "bufferView", true, -1); bufferViewIndex > -1)
            bufferAccessor->SetBufferView(a_Dictionary.Get<SG::BufferView>("bufferViews", bufferViewIndex));
        bufferAccessor->SetByteOffset(GLTF::Parse(gltfbufferAccessor, "byteOffset", true, bufferAccessor->GetByteOffset()));
        bufferAccessor->SetNormalized(GLTF::Parse(gltfbufferAccessor, "normalized", true, bufferAccessor->GetNormalized()));
        a_Dictionary.Add("accessors", bufferAccessor);
    }
}

static inline void ParseMeshes(const rapidjson::Value& a_JSONValue, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!a_JSONValue.HasMember("meshes")) return;
#ifdef _DEBUG
    auto timer = Tools::ScopedTimer("Parsing meshes");
#endif
    auto defaultMaterial(std::make_shared<SG::Material>("defaultMaterial"));
    for (const auto& gltfMesh : a_JSONValue["meshes"].GetArray()) {
        auto mesh = std::make_shared<SG::Component::Mesh>();
        mesh->SetName(GLTF::Parse(gltfMesh, "name", true, mesh->GetName()));
        if (gltfMesh.HasMember("primitives")) {
            for (const auto& primitive : gltfMesh["primitives"].GetArray()) {
                auto geometry(std::make_shared<SG::Primitive>());
                auto material{ defaultMaterial };
                if (const auto materialIndex = GLTF::Parse(primitive, "material", true, -1); materialIndex > -1)
                    material = a_Dictionary.Get<SG::Material>("materials", materialIndex);
                if (primitive.HasMember("attributes")) {
                    const auto& attributes = primitive["attributes"];
                    const auto COLOR_0    = GLTF::Parse(attributes, "COLOR_0", true, -1);
                    const auto JOINTS_0   = GLTF::Parse(attributes, "JOINTS_0", true, -1);
                    const auto NORMAL     = GLTF::Parse(attributes, "NORMAL", true, -1);
                    const auto POSITION   = GLTF::Parse(attributes, "POSITION", true, -1);
                    const auto TANGENT    = GLTF::Parse(attributes, "TANGENT", true, -1);
                    const auto TEXCOORD_0 = GLTF::Parse(attributes, "TEXCOORD_0", true, -1);
                    const auto TEXCOORD_1 = GLTF::Parse(attributes, "TEXCOORD_1", true, -1);
                    const auto TEXCOORD_2 = GLTF::Parse(attributes, "TEXCOORD_2", true, -1);
                    const auto TEXCOORD_3 = GLTF::Parse(attributes, "TEXCOORD_3", true, -1);
                    const auto WEIGHTS_0  = GLTF::Parse(attributes, "WEIGHTS_0", true, -1);
                    if (COLOR_0    > -1) geometry->SetColors(*a_Dictionary.Get<SG::BufferAccessor>("accessors", COLOR_0));
                    if (JOINTS_0   > -1) geometry->SetJoints(*a_Dictionary.Get<SG::BufferAccessor>("accessors", JOINTS_0));
                    if (NORMAL     > -1) geometry->SetNormals(*a_Dictionary.Get<SG::BufferAccessor>("accessors", NORMAL));
                    if (POSITION   > -1) geometry->SetPositions(*a_Dictionary.Get<SG::BufferAccessor>("accessors", POSITION));
                    if (TANGENT    > -1) geometry->SetTangent(*a_Dictionary.Get<SG::BufferAccessor>("accessors", TANGENT));
                    if (TEXCOORD_0 > -1) geometry->SetTexCoord0(*a_Dictionary.Get<SG::BufferAccessor>("accessors", TEXCOORD_0));
                    if (TEXCOORD_1 > -1) geometry->SetTexCoord1(*a_Dictionary.Get<SG::BufferAccessor>("accessors", TEXCOORD_1));
                    if (TEXCOORD_2 > -1) geometry->SetTexCoord2(*a_Dictionary.Get<SG::BufferAccessor>("accessors", TEXCOORD_2));
                    if (TEXCOORD_3 > -1) geometry->SetTexCoord3(*a_Dictionary.Get<SG::BufferAccessor>("accessors", TEXCOORD_3));
                    if (WEIGHTS_0  > -1) geometry->SetWeights(*a_Dictionary.Get<SG::BufferAccessor>("accessors", WEIGHTS_0));
                }
                auto accessorIndex = GLTF::Parse(primitive, "indices", true, -1);
                if (accessorIndex > -1) geometry->SetIndices(*a_Dictionary.Get<SG::BufferAccessor>("accessors", accessorIndex));
                geometry->SetDrawingMode(GLTF::GetGeometryDrawingMode(GLTF::DrawingMode(GLTF::Parse(primitive, "mode", true, int(GLTF::DrawingMode::Triangles)))));
                mesh->AddPrimitive(geometry, material);
            }
        }
        a_Dictionary.Add("meshes", mesh);
    }
}

static inline void ParseNodes(const rapidjson::Value& a_JSONValue, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!a_JSONValue.HasMember("nodes")) return;
#ifdef _DEBUG
    auto timer = Tools::ScopedTimer("Parsing nodes");
#endif
    for (const auto& gltfNode : a_JSONValue["nodes"].GetArray()) {
        auto entity = SG::Node::Create(a_AssetsContainer->GetECSRegistry());
        auto& transform = entity.GetComponent<SG::Component::Transform>();
        auto& name = entity.GetComponent<SG::Component::Name>();
        name = GLTF::Parse(gltfNode, "name", true, std::string(name));
        if (gltfNode.HasMember("matrix")) {
            glm::mat4 matrix{};
            for (unsigned i(0); i < gltfNode["matrix"].GetArray().Size() && i < glm::uint(matrix.length() * 4); i++)
                matrix[i / 4][i % 4] = gltfNode["matrix"].GetArray()[i].GetFloat();
            glm::vec3 scale;
            glm::quat rotation;
            glm::vec3 translation;
            glm::vec3 skew;
            glm::vec4 perspective;
            glm::decompose(matrix, scale, rotation, translation, skew, perspective);
            transform.position = translation;
            transform.rotation = rotation;
            transform.scale = scale;
        }
        transform.position = GLTF::Parse(gltfNode, "translation", true, transform.position);
        transform.rotation = GLTF::Parse(gltfNode, "scale", true, transform.rotation);
        transform.scale = GLTF::Parse(gltfNode, "rotation", true, transform.scale);
        a_Dictionary.entities["nodes"].push_back(entity);
    }
}

template<typename T, int I>
auto ConvertTo(const SG::BufferAccessor& accessor) {
    struct DataStruct {
        T data[I];
    };
    std::vector<DataStruct> data;
    for (auto &d : static_cast<Buffer::TypedAccessor<DataStruct>>(accessor)) {
        data.push_back(d);
    }
    return data;
}

template <typename T>
static inline auto GenerateAnimationChannel(SG::AnimationInterpolation interpolation, const SG::BufferAccessor& keyFramesValues, const SG::BufferAccessor& timings) {
    SG::AnimationChannel<T> newChannel;
    if (interpolation == SG::AnimationInterpolation::CubicSpline) {
        for (auto i = 0u; i < keyFramesValues.GetSize(); i += 3) {
            SG::AnimationChannel<T>::KeyFrame keyFrame;
            keyFrame.inputTangent = keyFramesValues.at<T>(static_cast<size_t>(i) + 0);
            keyFrame.value = keyFramesValues.at<T>(static_cast<size_t>(i) + 1);
            keyFrame.outputTangent = keyFramesValues.at<T>(static_cast<size_t>(i) + 2);
            keyFrame.time = timings.at<float>(i / 3);
            newChannel.InsertKeyFrame(keyFrame);
        }
    }
    else {
        for (auto i = 0u; i < keyFramesValues.GetSize(); ++i) {
            SG::AnimationChannel<T>::KeyFrame keyFrame;
            keyFrame.value = keyFramesValues.at<T>(i);
            keyFrame.time = timings.at<float>(i);
            newChannel.InsertKeyFrame(keyFrame);
        }
    }
    return newChannel;
}

static inline void ParseAnimations(const rapidjson::Document& document, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!document.HasMember("animations")) return;
#ifdef _DEBUG
    auto timer = Tools::ScopedTimer("Parsing animations");
#endif
    for (const auto& gltfAnimation : document["animations"].GetArray()) {
        auto newAnimation(std::make_shared<SG::Animation>());
        newAnimation->SetName(GLTF::Parse(gltfAnimation, "name", true, newAnimation->GetName()));
        for (const auto& channel : gltfAnimation["channels"].GetArray()) {
            auto &sampler{ gltfAnimation["samplers"].GetArray()[channel["sampler"].GetInt()] };
            SG::AnimationInterpolation channelInterpolation{ SG::AnimationInterpolation::Linear };
            if (sampler.HasMember("interpolation")) {
                std::string interpolation(sampler["interpolation"].GetString());
                if (interpolation == "LINEAR")
                    channelInterpolation = SG::AnimationInterpolation::Linear;
                else if (interpolation == "STEP")
                    channelInterpolation = SG::AnimationInterpolation::Step;
                else if (interpolation == "CUBICSPLINE")
                    channelInterpolation = SG::AnimationInterpolation::CubicSpline;
            }
            if (channel.HasMember("target")) {
                auto& target(channel["target"]);
                auto& entity = a_Dictionary.entities["nodes"].at(GLTF::Parse<int>(target, "node"));
                const auto path = GLTF::Parse<std::string>(target, "path", true, "");
                const auto input  = a_Dictionary.Get<SG::BufferAccessor>("accessors", GLTF::Parse<int>(sampler, "input"));
                const auto output = a_Dictionary.Get<SG::BufferAccessor>("accessors", GLTF::Parse<int>(sampler, "output"));
                if (path == "translation") {
                    auto newChannel = GenerateAnimationChannel<glm::vec3>(channelInterpolation, *output, *input);
                    newChannel.target = entity;
                    newAnimation->AddChannelPosition(newChannel);
                }
                else if (path == "rotation") {
                    auto newChannel = GenerateAnimationChannel<glm::quat>(channelInterpolation, *output, *input);
                    newChannel.target = entity;
                    newAnimation->AddChannelRotation(newChannel);
                }
                else if (path == "scale") {
                    auto newChannel = GenerateAnimationChannel<glm::vec3>(channelInterpolation, *output, *input);
                    newChannel.target = entity;
                    newAnimation->AddChannelScale(newChannel);
                }
                else if (path == "weights") {
                    //newAnimation->GetChannelPosition().target = node;
                }
                else throw std::runtime_error("Unknown animation path");
            }
        }
        a_Dictionary.Add("animations", newAnimation);
        a_AssetsContainer->Add(newAnimation);
    }
}

static inline void ParseSkins(const rapidjson::Document& a_Document, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!a_Document.HasMember("skins")) return;
#ifdef _DEBUG
    auto timer = Tools::ScopedTimer("Parsing skins");
#endif
    for (const auto& gltfSkin : a_Document["skins"].GetArray()) {
        auto skin(std::make_shared<SG::Component::Skin>());
        if (gltfSkin.HasMember("name"))
            skin->SetName(gltfSkin["name"].GetString());
        if (auto inverseBindMatrices = GLTF::Parse(gltfSkin, "inverseBindMatrices", true, -1); inverseBindMatrices > -1)
            skin->SetInverseBindMatrices(*a_Dictionary.Get<SG::BufferAccessor>("accessors", inverseBindMatrices));
        if (gltfSkin.HasMember("joints")) {
            for (const auto& joint : gltfSkin["joints"].GetArray())
                skin->AddJoint(a_Dictionary.entities["nodes"].at(joint.GetInt()));
        }
        a_Dictionary.Add("skins", skin);
    }
}

static inline void ParseScenes(const rapidjson::Value& a_JSONValue, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!a_JSONValue.HasMember("scenes")) return;
#ifdef _DEBUG
    auto timer = Tools::ScopedTimer("Parsing scenes");
#endif
    for (const auto& gltfScene : a_JSONValue["scenes"].GetArray()) {
        auto scene = std::make_shared<SG::Scene>(a_AssetsContainer->GetECSRegistry());
        for (const auto& node : gltfScene["nodes"].GetArray()) {
            scene->AddEntity(a_Dictionary.entities["nodes"].at(node.GetInt()));
        }
        a_Dictionary.Add("scenes", scene);
        a_AssetsContainer->Add(scene);
    }
}

static inline void Parse_KHR_lights_punctual(const rapidjson::Value& a_JSONValue, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!a_JSONValue.HasMember("lights")) return;
#ifdef _DEBUG
    auto timer = Tools::ScopedTimer("Parsing lights");
#endif
    for (const auto& gltfLight : a_JSONValue["lights"].GetArray()) {
        auto entity = SG::Light::Create(a_AssetsContainer->GetECSRegistry());
        if (gltfLight.HasMember("type")) {
            auto type = gltfLight["type"].GetString();
            if (type == "directional") entity.AddComponent<SG::Component::LightDirectional>();
            entity.GetComponent<SG::Component::Name>() = GLTF::Parse(gltfLight, "name", true, std::string(entity.GetComponent<SG::Component::Name>()));
            auto& settings = entity.GetComponent<SG::Component::Light>();
            settings.color     = GLTF::Parse(gltfLight, "color", true, settings.color);
            settings.intensity = GLTF::Parse(gltfLight, "intensity", true, settings.intensity);
            settings.range     = GLTF::Parse(gltfLight, "range", true, settings.range);
        }
        a_Dictionary.entities["lights"].push_back(entity);
    }
}

static inline void ParseGLTFExtensions(const rapidjson::Value& a_JSONValue, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!a_JSONValue.HasMember("extension")) return;
#ifdef _DEBUG
    auto timer = Tools::ScopedTimer("Parsing extensions");
#endif
    const auto& extensions = a_JSONValue["extensions"];
    if (extensions.HasMember("KHR_lights_punctual")) Parse_KHR_lights_punctual(extensions["KHR_lights_punctual"], a_Dictionary, a_AssetsContainer);
}

static inline void ParseImages(const std::filesystem::path path, const rapidjson::Document& a_JSONValue, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!a_JSONValue.HasMember("images")) return;
#ifdef _DEBUG
    auto timer = Tools::ScopedTimer("Parsing images");
#endif
    std::vector<std::shared_ptr<Asset>> assets;
    for (const auto& gltfImage : a_JSONValue["images"].GetArray()) {
        auto imageAsset = std::make_shared<Asset>();
        imageAsset->parsingOptions = a_AssetsContainer->parsingOptions;
        auto uri = GLTF::Parse<std::string>(gltfImage, "uri", true, "");
        if (!uri.empty()) {
            imageAsset->SetUri(GLTF::CreateUri(path.parent_path(), uri));
        }
        else {
            const auto bufferViewIndex = GLTF::Parse(gltfImage, "bufferView", true, -1);
            if (bufferViewIndex == -1) {
                imageAsset->Add(std::make_shared<SG::Image>());
                imageAsset->SetLoaded(true);
            } else {
                const auto mimeType = GLTF::Parse<std::string>(gltfImage, "mimeType");
                imageAsset->SetUri(std::string("data:") + mimeType + ",");
                imageAsset->parsingOptions.data.useBufferView = true;
                imageAsset->SetBufferView(a_Dictionary.Get<SG::BufferView>("bufferViews", bufferViewIndex));
            }
        }
        assets.push_back(imageAsset);
    }
    std::vector<Parser::ParsingFuture> futures;
    for (const auto& asset : assets) futures.push_back(Parser::AddParsingTask(asset));
    for (auto& future : futures) {
        auto asset = future.get();
        if (asset->GetLoaded()) a_Dictionary.Add("images", asset->Get<SG::Image>().front());
        else debugLog("Error while parsing" + std::string(asset->GetUri()));
    }
}

static inline void SetParenting(const rapidjson::Document& a_Document, GLTF::Dictionary& a_Dictionary)
{
    if (!a_Document.HasMember("nodes")) return;
#ifdef _DEBUG
    auto timer = Tools::ScopedTimer("Setting parenting");
#endif
    //Build parenting relationship
    auto nodeIndex = 0;
    for (const auto& gltfNode : a_Document["nodes"].GetArray()) {
        auto& entity = a_Dictionary.entities["nodes"].at(nodeIndex);
        auto meshIndex = GLTF::Parse(gltfNode, "mesh", true, -1);
        auto skinIndex = GLTF::Parse(gltfNode, "skin", true, -1);
        auto cameraIndex = GLTF::Parse(gltfNode, "camera", true, -1);
        if (cameraIndex > -1) {
            auto& camera = a_Dictionary.entities["cameras"].at(cameraIndex);
            SG::Node::SetParent(camera, entity);
        }
        if (meshIndex > -1) {
            auto mesh = a_Dictionary.Get<SG::Component::Mesh>("meshes", meshIndex);
            entity.AddComponent<SG::Component::Mesh>(*mesh);
        }
        if (skinIndex > -1) {
            auto skin = a_Dictionary.Get<SG::Component::Skin>("skins", skinIndex);
            entity.AddComponent<SG::Component::Skin>(*skin);
        }
        if (gltfNode.HasMember("children")) {
            entity.AddComponent<SG::Component::Children>();
            for (const auto& child : gltfNode["children"].GetArray()) {
                const auto& childEntity = a_Dictionary.entities["nodes"].at(child.GetInt());
                SG::Node::SetParent(childEntity, entity);
            }
        }
        nodeIndex++;
    }
}

std::shared_ptr<Asset> ParseGLTF(const std::shared_ptr<Asset>& a_AssetsContainer)
{
#ifdef _DEBUG
    auto timer = Tools::ScopedTimer("Parsing GLTF");
#endif
    auto path = a_AssetsContainer->GetUri().DecodePath();
    std::ifstream file(path);
    rapidjson::IStreamWrapper streamWrapper(file);
    rapidjson::Document document;
    rapidjson::ParseResult parseResult(document.ParseStream(streamWrapper));
    if (!parseResult) {
        debugLog("Invalid file : " + path.string());
        return a_AssetsContainer;
    }
    auto& mutex = a_AssetsContainer->GetECSRegistry()->GetLock();
    std::scoped_lock lock(mutex);
    GLTF::Dictionary dictionary;
    ParseGLTFExtensions(document, dictionary, a_AssetsContainer);
    ParseCameras(document, dictionary, a_AssetsContainer);
    ParseBuffers(path, document, dictionary, a_AssetsContainer);
    ParseBufferViews(document, dictionary, a_AssetsContainer);
    ParseImages(path, document, dictionary, a_AssetsContainer);
    ParseTextureSamplers(document, dictionary, a_AssetsContainer);
    ParseTextures(document, dictionary, a_AssetsContainer);
    ParseMaterials(document, dictionary, a_AssetsContainer);
    ParseBufferAccessors(document, dictionary, a_AssetsContainer);
    ParseMeshes(document, dictionary, a_AssetsContainer);
    ParseNodes(document, dictionary, a_AssetsContainer);
    ParseSkins(document, dictionary, a_AssetsContainer);
    ParseAnimations(document, dictionary, a_AssetsContainer);
    ParseScenes(document, dictionary, a_AssetsContainer);
    SetParenting(document, dictionary);
    a_AssetsContainer->SetAssetType("model/gltf+json");
    a_AssetsContainer->SetLoaded(true);
    return a_AssetsContainer;
}
}

