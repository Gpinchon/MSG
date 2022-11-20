/*
* @Author: gpinchon
* @Date:   2020-08-07 18:36:53
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-11 16:17:13
*/

#include <glm/ext.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/string_cast.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>

#include <Assets/Asset.hpp>
#include <Assets/Parser.hpp>

#include <SG/Animation/Animation.hpp>
#include <SG/Animation/Channel.hpp>
#include <SG/Buffer/Accessor.hpp>
#include <SG/Buffer/Buffer.hpp>
#include <SG/Buffer/View.hpp>
#include <SG/Camera/Camera.hpp>
#include <SG/Image/Image.hpp>
#include <SG/Light/Directional.hpp>
#include <SG/Material/Material.hpp>
#include <SG/Material/Extension/Sheen.hpp>
#include <SG/Material/Extension/MetallicRoughness.hpp>
#include <SG/Material/Extension/SpecularGlossiness.hpp>
#include <SG/Shape/Geometry.hpp>
#include <SG/Shape/Mesh.hpp>
#include <SG/Shape/MeshSkin.hpp>
#include <SG/Node/Scene.hpp>
#include <SG/Texture/Texture2D.hpp>
#include <SG/Texture/Sampler.hpp>

#include <Tools/Debug.hpp>

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

namespace TabGraph::Assets {
namespace GLTF {
struct Container {
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
        return SG::Geometry::DrawingMode::Points;
    case DrawingMode::Lines:
        return SG::Geometry::DrawingMode::Lines;
    case DrawingMode::LineLoop:
        return SG::Geometry::DrawingMode::LineLoop;
    case DrawingMode::LineStrip:
        return SG::Geometry::DrawingMode::LineStrip;
    case DrawingMode::Triangles:
        return SG::Geometry::DrawingMode::Triangles;
    case DrawingMode::TriangleStrip:
        return SG::Geometry::DrawingMode::TriangleStrip;
    case DrawingMode::TriangleFan:
        return SG::Geometry::DrawingMode::TriangleFan;
    default:
        return SG::Geometry::DrawingMode::Unknown;
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


static inline void ParseCameras(const rapidjson::Document& document, GLTF::Container& a_Container)
{
    if (!document.HasMember("cameras")) return;
    auto cameraIndex(0);
    for (const auto& camera : document["cameras"].GetArray()) {
        auto newCamera(std::make_shared<SG::Camera>("Camera" + std::to_string(cameraIndex)));
        if (std::string(camera["type"].GetString()) == "perspective") {
            if (camera["perspective"].HasMember("zfar")) {
                SG::CameraProjection::Perspective projection;
                projection.zfar = GLTF::Parse(camera["perspective"], "zfar", false, projection.zfar);
                projection.znear = GLTF::Parse(camera["perspective"], "znear", true, projection.znear);
                projection.fov = GLTF::Parse(camera["perspective"], "fov", true, projection.fov);
                newCamera->SetProjection(projection);
            }
            else {
                SG::CameraProjection::PerspectiveInfinite projection;
                projection.znear = GLTF::Parse(camera["perspective"], "znear", true, projection.znear);
                projection.fov = glm::degrees(GLTF::Parse(camera["perspective"], "yfov", true, glm::radians(projection.fov)));
                newCamera->SetProjection(projection);
            }
        }
        else if (std::string(camera["type"].GetString()) == "orthographic") {
            SG::CameraProjection::Orthographic projection;
            newCamera->SetProjection(projection);
        }
        a_Container.Add("cameras", newCamera);
        cameraIndex++;
    }
}

static inline auto ParseTextureSamplers(const rapidjson::Document& document)
{
    std::vector<std::shared_ptr<SG::TextureSampler>> samplerVector;
    if (!document.HasMember("samplers")) return samplerVector;
    for (const auto& sampler : document["samplers"].GetArray()) {
        auto newSampler{ std::make_shared<SG::TextureSampler>() };
        for (rapidjson::Value::ConstMemberIterator setting = sampler.MemberBegin(); setting != sampler.MemberEnd(); setting++) {
            if ("magFilter" == setting->name.GetString())
                newSampler->SetMagFilter(GLTF::GetFilter(GLTF::TextureFilter(setting->value.GetInt())));
            else if ("minFilter" == setting->name.GetString())
                newSampler->SetMinFilter(GLTF::GetFilter(GLTF::TextureFilter(setting->value.GetInt())));
            else if ("wrapS" == setting->name.GetString())
                newSampler->SetWrapS(GLTF::GetWrap(GLTF::TextureWrap(setting->value.GetInt())));
            else if ("wrapT" == setting->name.GetString())
                newSampler->SetWrapT(GLTF::GetWrap(GLTF::TextureWrap(setting->value.GetInt())));
        }
        samplerVector.push_back(newSampler);
    }
    return samplerVector;
}

static inline void ParseTextures(const rapidjson::Document& document, GLTF::Container& a_Container, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    debugLog("Start parsing textures");
    if (!document.HasMember("textures")) return;
    auto samplers = ParseTextureSamplers(document);
    auto textureIndex(0);
    for (const auto& textureValue : document["textures"].GetArray()) {
        auto texture = std::make_shared<SG::Texture2D>();
        if (textureValue.HasMember("source")) {
            auto source(textureValue["source"].GetInt());
            texture->SetImage(a_Container.Get<SG::Image>("images", source));
        }
        if (textureValue.HasMember("sampler")) {
            auto sampler(samplers.at(textureValue["sampler"].GetInt()));
            texture->SetSampler(sampler);
        }
        texture->SetCompressed(a_AssetsContainer->parsingOptions.texture.compress);
        texture->SetCompressionQuality(a_AssetsContainer->parsingOptions.texture.compressionQuality);
        a_AssetsContainer->assets.push_back(texture);
        textureIndex++;
    }
    debugLog("Done parsing textures");
}

static inline auto ParseSpecularGlossiness(GLTF::Container& a_Container, const rapidjson::Value& extension) {
    SG::SpecularGlossinessExtension specGloss;
    specGloss.SetDiffuseFactor(GLTF::Parse(extension, "diffuseFactor", true, specGloss.GetDiffuseFactor()));
    specGloss.SetSpecularFactor(GLTF::Parse(extension, "specularFactor", true, specGloss.GetSpecularFactor()));
    specGloss.SetGlossinessFactor(GLTF::Parse(extension, "glossinessFactor", true, specGloss.GetGlossinessFactor()));
    if (extension.HasMember("diffuseTexture")) {
        const auto& texInfo = extension["diffuseTexture"];
        SG::Material::NormalTextureInfo texture;
        texture.texture = a_Container.Get<SG::Texture>("textures", GLTF::Parse<int>(texInfo, "index"));
        texture.texCoord = GLTF::Parse(texInfo, "texCoord", true, texture.texCoord);
        specGloss.SetDiffuseTexture(texture);
    }
    if (extension.HasMember("specularGlossinessTexture")) {
        const auto& texInfo = extension["specularGlossinessTexture"];
        SG::Material::NormalTextureInfo texture;
        texture.texture = a_Container.Get<SG::Texture>("textures", GLTF::Parse<int>(texInfo, "index"));
        texture.texCoord = GLTF::Parse(texInfo, "texCoord", true, texture.texCoord);
        specGloss.SetSpecularGlossinessTexture(texture);
    }
    return specGloss;
}

static inline auto ParseSheen(GLTF::Container& a_Container, const rapidjson::Value& a_Extension) {
    SG::SheenExtension sheen;
    sheen.SetColorFactor(GLTF::Parse(a_Extension, "sheenColorFactor", true, sheen.GetColorFactor()));
    sheen.SetRoughnessFactor(GLTF::Parse(a_Extension, "sheenRoughnessFactor", true, sheen.GetRoughnessFactor()));
    if (a_Extension.HasMember("sheenColorTexture")) {
        const auto& texInfo = a_Extension["sheenColorTexture"];
        SG::Material::NormalTextureInfo texture;
        texture.texture = a_Container.Get<SG::Texture>("textures", GLTF::Parse<int>(texInfo, "index"));
        texture.texCoord = GLTF::Parse(texInfo, "texCoord", true, texture.texCoord);
        sheen.SetColorTexture(texture);
    }
    if (a_Extension.HasMember("sheenRoughnessTexture")) {
        const auto& texInfo = a_Extension["sheenRoughnessTexture"];
        SG::Material::NormalTextureInfo texture;
        texture.texture = a_Container.Get<SG::Texture>("textures", GLTF::Parse<int>(texInfo, "index"));
        texture.texCoord = GLTF::Parse(texInfo, "texCoord", true, texture.texCoord);
        sheen.SetRoughnessTexture(texture);
    }
    return sheen;
}

static inline void ParseMaterialExtensions(GLTF::Container& a_Container, const rapidjson::Value& materialValue, std::shared_ptr<SG::Material> a_Material)
{
    for (const auto& extension : materialValue["extensions"].GetObject()) {
        if (std::string(extension.name.GetString()) == "KHR_materials_pbrSpecularGlossiness")
            a_Material->AddExtension(ParseSpecularGlossiness(a_Container, extension.value));
        else if (std::string(extension.name.GetString()) == "KHR_materials_sheen")
            a_Material->AddExtension(ParseSheen(a_Container, extension.value));
    }
}

static inline auto ParseMetallicRoughness(GLTF::Container& a_Container, const rapidjson::Value& a_Extension)
{
    SG::MetallicRoughnessExtension mra{};
    mra.SetColorFactor(GLTF::Parse(a_Extension, "baseColorFactor", true, mra.GetColorFactor()));
    mra.SetMetallicFactor(GLTF::Parse(a_Extension, "metallicFactor", true, mra.GetMetallicFactor()));
    mra.SetRoughnessFactor(GLTF::Parse(a_Extension, "roughnessFactor", true, mra.GetRoughnessFactor()));
    if (a_Extension.HasMember("baseColorTexture")) {
        const auto& texInfo = a_Extension["baseColorTexture"];
        SG::TextureInfo texture;
        texture.texture = a_Container.Get<SG::Texture>("textures", GLTF::Parse<int>(texInfo, "index"));
        texture.texCoord = GLTF::Parse(texInfo, "texCoord", true, texture.texCoord);
        mra.SetColorTexture(texture);
    }
    if (a_Extension.HasMember("metallicRoughnessTexture")) {
        const auto& texInfo = a_Extension["metallicRoughnessTexture"];
        SG::TextureInfo texture;
        texture.texture = a_Container.Get<SG::Texture>("textures", GLTF::Parse<int>(texInfo, "index"));
        texture.texCoord = GLTF::Parse(texInfo, "texCoord", true, texture.texCoord);
        mra.SetMetallicRoughnessTexture(texture);
    }
    return mra;
}

static inline void ParseMaterials(const rapidjson::Document& document, GLTF::Container& a_Container)
{
    debugLog("Start parsing materials");
    if (!document.HasMember("materials")) return;
    auto materialIndex(0);
    for (const auto& materialValue : document["materials"].GetArray()) {
        auto material(std::make_shared<SG::Material>("Material " + std::to_string(materialIndex)));
        material->SetName(GLTF::Parse(materialValue, "name", true, material->GetName()));
        material->SetAlphaCutoff(GLTF::Parse(materialValue, "alphaCutoff", true, material->GetAlphaCutoff()));
        material->SetDoubleSided(GLTF::Parse(materialValue, "doubleSided", true, material->GetDoubleSided()));
        material->SetEmissiveFactor(GLTF::Parse(materialValue, "emissiveFactor", true, material->GetEmissiveFactor()));
        auto alphaMode = GLTF::Parse<std::string>(materialValue, "alphaMode", true);
        if (alphaMode == "Opaque")
            material->SetAlphaMode(SG::Material::AlphaMode::Opaque);
        if (alphaMode == "MASK")
            material->SetAlphaMode(SG::Material::AlphaMode::Mask);
        if (alphaMode == "BLEND")
            material->SetAlphaMode(SG::Material::AlphaMode::Blend);

        if (materialValue.HasMember("normalTexture")) {
            const auto& texInfo = materialValue["normalTexture"];
            SG::Material::NormalTextureInfo texture;
            texture.texture  = a_Container.Get<SG::Texture>("textures", GLTF::Parse<int>(texInfo, "index"));
            texture.texCoord = GLTF::Parse(texInfo, "texCoord", true, texture.texCoord);
            texture.scale    = GLTF::Parse(texInfo, "scale", true, texture.scale);
            material->SetNormalTexture(texture);
        }
        if (materialValue.HasMember("emissiveTexture")) {
            const auto& texInfo = materialValue["emissiveTexture"];
            SG::TextureInfo texture;
            texture.texture  = a_Container.Get<SG::Texture>("textures", GLTF::Parse<int>(texInfo, "index"));
            texture.texCoord = GLTF::Parse(texInfo, "texCoord", true, texture.texCoord);
            material->SetEmissiveTexture(texture);
        }
        if (materialValue.HasMember("occlusionTexture")) {
            const auto& texInfo = materialValue["occlusionTexture"];
            SG::Material::OcclusionTextureInfo texture;
            texture.texture  = a_Container.Get<SG::Texture>("textures", GLTF::Parse<int>(texInfo, "index"));
            texture.texCoord = GLTF::Parse(texInfo, "texCoord", true, texture.texCoord);
            texture.strength = GLTF::Parse(texInfo, "strength", true, texture.strength);
            material->SetOcclusionTexture(texture);
        }
        if (materialValue.HasMember("pbrMetallicRoughness"))
            material->AddExtension(ParseMetallicRoughness(a_Container, materialValue["pbrMetallicRoughness"]));
        if (materialValue.HasMember("extensions")) ParseMaterialExtensions(a_Container, materialValue, material);
        a_Container.Add("materials", material);
        materialIndex++;
    }
    debugLog("Done parsing materials");
}

static inline void ParseBuffers(const std::filesystem::path path, const rapidjson::Document& document, GLTF::Container& a_Container, std::shared_ptr<Asset> a_AssetsContainer)
{
    debugLog("Start parsing buffers");
    if (!document.HasMember("buffers")) return;
    std::vector<std::shared_ptr<Asset>> assetVector;
    for (const auto& bufferValue : document["buffers"].GetArray()) {
        auto asset { std::make_shared<Asset>() };
        asset->parsingOptions = a_AssetsContainer->parsingOptions;
        asset->SetUri(GLTF::CreateUri(path.parent_path(), bufferValue["uri"].GetString()));
        asset->SetName(GLTF::Parse(bufferValue, "name", true, asset->GetName()));
        assetVector.push_back(asset);
    }
    std::vector<std::future<std::shared_ptr<Asset>>> parsingFuture;
    for (const auto& asset : assetVector) parsingFuture.push_back(Parser::AddParsingTask(asset));
    for (auto& future : parsingFuture)  a_Container.Add("buffers", future.get()->Get<SG::Buffer>().front());
    debugLog("Done parsing buffers");
}

static inline void ParseBufferViews(const rapidjson::Document& document, GLTF::Container& a_Container)
{
    debugLog("Start parsing bufferViews");
    if (!document.HasMember("bufferViews")) return;
    for (const auto& bufferViewValue : document["bufferViews"].GetArray()) {
        auto bufferView(std::make_shared<SG::BufferView>());
        const auto buffer = a_Container.Get<SG::Buffer>("buffers", bufferViewValue["buffer"].GetInt());
        bufferView->SetBuffer(buffer);
        bufferView->SetByteLength(bufferViewValue["byteLength"].GetInt());
        bufferView->SetName(GLTF::Parse(bufferViewValue, "name", true, bufferView->GetName()));
        bufferView->SetByteOffset(GLTF::Parse(bufferViewValue, "byteOffset", true, bufferView->GetByteOffset()));
        bufferView->SetByteStride(GLTF::Parse(bufferViewValue, "byteStride", true, bufferView->GetByteStride()));
        //if (bufferViewValue.HasMember("target"))
        //    bufferView->SetType(GLTF::GetBufferViewType(GLTF::BufferViewType(bufferViewValue["target"].GetInt())));
        a_Container.Add("bufferViews", bufferView);
    }
    debugLog("Done parsing bufferViews");
}

static inline auto ParseBufferAccessors(const rapidjson::Document& document, GLTF::Container& a_Container)
{
    debugLog("Start parsing bufferAccessors");
    std::vector<SG::BufferAccessor> bufferAccessorVector;
    if (!document.HasMember("accessors"))
        return bufferAccessorVector;
    auto bufferAccessorIndex(0);
    for (const auto& bufferAccessorValue : document["accessors"].GetArray()) {
        std::shared_ptr<SG::BufferView> bufferView;
        auto bufferViewIndex = GLTF::Parse(bufferAccessorValue, "bufferView", true, -1);
        bufferView = bufferViewIndex == -1 ? nullptr : a_Container.Get<SG::BufferView>("bufferViews", bufferViewIndex);
        const auto byteOffset = GLTF::Parse(bufferAccessorValue, "byteOffset", true, 0);
        SG::BufferAccessor bufferAccessor{
            bufferView,
            byteOffset,
            static_cast<size_t>(bufferAccessorValue["count"].GetInt()),
            GLTF::GetAccessorComponentType(GLTF::ComponentType(bufferAccessorValue["componentType"].GetInt())),
            static_cast<uint8_t>(GLTF::GetAccessorComponentNbr(bufferAccessorValue["type"].GetString())),
        };
        bufferAccessor.SetName(GLTF::Parse(bufferAccessorValue, "name", true, bufferAccessor.GetName()));
        bufferAccessor.SetNormalized(GLTF::Parse(bufferAccessorValue, "normalized", true, bufferAccessor.GetNormalized()));
        bufferAccessorVector.push_back(bufferAccessor);
        bufferAccessorIndex++;
    }
    debugLog("Done parsing bufferAccessors");
    return bufferAccessorVector;
}

static inline void ParseMeshes(const rapidjson::Document& document, GLTF::Container& a_Container, const std::vector<SG::BufferAccessor>& bufferAccessors)
{
    debugLog("Start parsing meshes");
    auto meshesItr(document.FindMember("meshes"));
    if (meshesItr == document.MemberEnd()) return;
    auto defaultMaterial(std::make_shared<SG::Material>("defaultMaterial"));
    for (const auto& mesh : meshesItr->value.GetArray()) {
        debugLog("Found new mesh");
        auto currentMesh(std::make_shared<SG::Mesh>());
        if (mesh.HasMember("name"))
            currentMesh->SetName(mesh["name"].GetString());
        if (mesh.HasMember("primitives")) {
            for (const auto& primitive : mesh["primitives"].GetArray()) {
                debugLog("Found new primitive");
                auto geometry(std::make_shared<SG::Geometry>());
                auto materialPtr{ defaultMaterial };
                if (primitive.HasMember("material")) {
                    auto& material(primitive["material"]);
                    materialPtr = a_Container.Get<SG::Material>("materials", material.GetInt());
                }
                if (primitive.HasMember("attributes")) {
                    for (const auto& attribute : primitive["attributes"].GetObject()) {
                        std::string attributeName(attribute.name.GetString());
                        /*auto attributeName(std::string(attribute.name.GetString()));
                        auto accessor(bufferAccessors.at(attribute.value.GetInt()));
                        auto accessorKey(SG::Geometry::GetAccessorKey(attributeName));
                        if (accessorKey == SG::Geometry::AccessorKey::Invalid) {
                            debugLog("Invalid Accessor Key : " + attributeName);
                        }
                        else {
                            geometry->SetAccessor(accessorKey, accessor);
                            accessor->GetBufferView()->SetType(SG::BufferView::Type::Array);
                        }*/
                    }
                }
                if (primitive.HasMember("indices")) {
                    auto &accessor(bufferAccessors.at(primitive["indices"].GetInt()));
                    geometry->SetIndices(accessor);
                }
                if (primitive.HasMember("mode"))
                    geometry->SetDrawingMode(GLTF::GetGeometryDrawingMode(GLTF::DrawingMode(primitive["mode"].GetInt())));
                currentMesh->AddGeometry(geometry, materialPtr);
            }
        }
        a_Container.Add("meshes", currentMesh);
    }
    debugLog("Done parsing meshes");
}

static inline void ParseNodes(const rapidjson::Value& a_JSONValue, GLTF::Container& a_Container)
{
    auto nodeItr(a_JSONValue.FindMember("nodes"));
    if (nodeItr == a_JSONValue.MemberEnd()) return;
    int nodeIndex = 0;
    for (const auto& node : nodeItr->value.GetArray()) {
        std::shared_ptr<SG::Node> newNode;
        if (node.HasMember("children") || node.HasMember("camera") || node.HasMember("mesh"))
            newNode = std::make_shared<SG::NodeGroup>("NodeGroup_" + std::to_string(nodeIndex));
        else
            newNode = std::make_shared<SG::Node>("Node_" + std::to_string(nodeIndex));
        auto transform(newNode);
        newNode->SetName(GLTF::Parse(node, "name", true, newNode->GetName()));
        if (node.HasMember("matrix")) {
            glm::mat4 matrix{};
            for (unsigned i(0); i < node["matrix"].GetArray().Size() && i < glm::uint(matrix.length() * 4); i++)
                matrix[i / 4][i % 4] = node["matrix"].GetArray()[i].GetFloat();
            glm::vec3 scale;
            glm::quat rotation;
            glm::vec3 translation;
            glm::vec3 skew;
            glm::vec4 perspective;
            glm::decompose(matrix, scale, rotation, translation, skew, perspective);
            transform->SetLocalPosition(translation);
            transform->SetLocalRotation(rotation);
            transform->SetLocalScale(scale);
        }
        transform->SetLocalPosition(GLTF::Parse(node, "translation", true, transform->GetLocalPosition()));
        transform->SetLocalScale(GLTF::Parse(node, "scale", true, transform->GetLocalScale()));
        transform->SetLocalRotation(GLTF::Parse(node, "rotation", true, transform->GetLocalRotation()));
        a_Container.Add("nodes", newNode);
        nodeIndex++;
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
            keyFrame.inputTangent = keyFramesValues.at<glm::vec3>(static_cast<size_t>(i) + 0);
            keyFrame.value = keyFramesValues.at<glm::vec3>(static_cast<size_t>(i) + 1);
            keyFrame.outputTangent = keyFramesValues.at<glm::vec3>(static_cast<size_t>(i) + 2);
            keyFrame.time = timings.at<float>(i / 3);
            newChannel.InsertKeyFrame(keyFrame);
        }
    }
    else {
        for (auto i = 0u; i < keyFramesValues.GetSize(); ++i) {
            SG::AnimationChannel<T>::KeyFrame keyFrame;
            keyFrame.value = keyFramesValues.at<glm::vec3>(i);
            keyFrame.time = timings.at<float>(i);
            newChannel.InsertKeyFrame(keyFrame);
        }
    }
    return newChannel;
}

static inline void ParseAnimations(const rapidjson::Document& document, GLTF::Container& a_Container, const std::vector<SG::BufferAccessor>& bufferAccessors)
{
    if (!document.HasMember("animations")) return;
    for (const auto& animation : document["animations"].GetArray()) {
        auto newAnimation(std::make_shared<SG::Animation>());
        newAnimation->SetName(GLTF::Parse(animation, "name", true, newAnimation->GetName()));
        for (const auto& channel : animation["channels"].GetArray()) {
            auto &sampler{ animation["samplers"].GetArray()[channel["sampler"].GetInt()] };
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
                auto& node = a_Container.Get<SG::Node>("nodes", target["node"].GetInt());
                if (target.HasMember("path")) {
                    std::string path(target["path"].GetString());
                    if (path == "translation") {
                        auto newChannel = GenerateAnimationChannel<glm::vec3>(channelInterpolation, bufferAccessors.at(sampler["output"].GetInt()), bufferAccessors.at(sampler["input"].GetInt()));
                        newChannel.target = node;
                        newAnimation->AddChannelPosition(newChannel);
                    }
                    else if (path == "rotation") {
                        auto newChannel = GenerateAnimationChannel<glm::quat>(channelInterpolation, bufferAccessors.at(sampler["output"].GetInt()), bufferAccessors.at(sampler["input"].GetInt()));
                        newChannel.target = node;
                        newAnimation->AddChannelRotation(newChannel);
                    }
                    else if (path == "scale") {
                        auto newChannel = GenerateAnimationChannel<glm::vec3>(channelInterpolation, bufferAccessors.at(sampler["output"].GetInt()), bufferAccessors.at(sampler["input"].GetInt()));
                        newChannel.target = node;
                        newAnimation->AddChannelScale(newChannel);
                    }
                    else if (path == "weights") {
                        //newAnimation->GetChannelPosition().target = node;
                    }
                    else throw std::runtime_error("Unknown animation path");
                }
            }
        }
        a_Container.Add("animations", newAnimation);
    }
}

static inline void ParseSkins(const rapidjson::Document& document, GLTF::Container& a_Container, const std::vector<SG::BufferAccessor>& bufferAccessors)
{
    debugLog("Start parsing Skins");
    if (!document.HasMember("skins")) return;
    for (const auto& skin : document["skins"].GetArray()) {
        auto newSkin(std::make_shared<SG::Mesh::Skin>());
        if (skin.HasMember("name"))
            newSkin->SetName(skin["name"].GetString());
        if (skin.HasMember("inverseBindMatrices")) {
            auto accessor{ bufferAccessors.at(skin["inverseBindMatrices"].GetInt()) };
            newSkin->SetInverseBindMatrices(accessor);
        }
        if (skin.HasMember("joints")) {
            for (const auto& joint : skin["joints"].GetArray())
                newSkin->AddJoint(a_Container.Get<SG::Node>("nodes", joint.GetInt()));
        }
        a_Container.Add("skins", newSkin);
    }
    debugLog("Done parsing Skins");
}

static inline auto SetParenting(const rapidjson::Document& document, GLTF::Container& a_Container)
{
    auto nodeItr(document.FindMember("nodes"));
    if (nodeItr == document.MemberEnd()) return;
    //Build parenting relationship
    auto nodeIndex = 0;
    for (const auto& gltfNode : nodeItr->value.GetArray()) {
        auto node(a_Container.Get<SG::Node>("nodes", nodeIndex));
        if (gltfNode.HasMember("mesh")) {
            auto mesh(a_Container.Get<SG::Mesh>("meshes", gltfNode["mesh"].GetInt()));
            if (gltfNode.HasMember("skin"))
                mesh->SetSkin(a_Container.Get<SG::Mesh::Skin>("skins", gltfNode["skin"].GetInt()));
            mesh->SetParent(std::static_pointer_cast<SG::NodeGroup>(node));
        }
        if (gltfNode.HasMember("camera")) {
            auto &camera = a_Container.Get<SG::Camera>("cameras", gltfNode["camera"].GetInt());
            camera->SetParent(std::static_pointer_cast<SG::NodeGroup>(node));
        }
        if (gltfNode.HasMember("children")) {
            for (const auto& child : gltfNode["children"].GetArray()) {
                const auto &childNode = a_Container.Get<SG::Node>("nodes", child.GetInt());
                childNode->SetParent(std::static_pointer_cast<SG::NodeGroup>(node));
                std::cout << "Node parenting " << node->GetName() << " -> " << childNode->GetName() << std::endl;
            }
        }
        nodeIndex++;
    }
}

static inline void ParseScenes(const rapidjson::Document& document, GLTF::Container& a_Container)
{
    auto scenes(document["scenes"].GetArray());
    int sceneIndex = 0;
    for (const auto& scene : scenes) {
        std::cout << "found scene" << std::endl;
        auto newScene(std::make_shared<SG::Scene>(std::to_string(sceneIndex)));
        for (const auto& node : scene["nodes"].GetArray()) {
            a_Container.Get<SG::Node>("nodes", node.GetInt())->SetParent(newScene);
            std::cout << a_Container.Get<SG::Node>("nodes", node.GetInt())->GetName() << " is part of Scene " << newScene->GetName() << std::endl;
        }
        for (const auto& animation : a_Container.Get("animations")) {
            newScene->AddAnimation(std::static_pointer_cast<SG::Animation>(animation));
            std::cout << (animation ? animation->GetName() : "nullptr") << std::endl;
        }
        a_Container.Add("scenes", newScene);
        sceneIndex++;
    }
}

static inline auto ParseLights(const rapidjson::Document& document)
{
    std::vector<std::shared_ptr<SG::Light>> lightsVector;
    const auto& extensions(document.FindMember("extensions"));
    if (extensions != document.MemberEnd()) {
        const auto& KHR_lights_punctual = extensions->value.FindMember("KHR_lights_punctual");
        if (KHR_lights_punctual != extensions->value.MemberEnd()) {
            const auto& lights = KHR_lights_punctual->value.FindMember("lights");
            if (lights != KHR_lights_punctual->value.MemberEnd()) {
                for (const auto& light : lights->value.GetArray()) {
                    std::shared_ptr<SG::Light> newLight;
                    if (light.HasMember("type")) {
                        auto type = light["type"].GetString();
                        if (type == "directional")
                            newLight = std::make_shared<SG::LightDirectional>();
                        lightsVector.emplace_back(newLight);
                    }
                }
            }
        }
    }
    return lightsVector;
}

static inline void ParseImages(const std::filesystem::path path, const rapidjson::Document& document, GLTF::Container& a_Container, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    auto imagesItr(document.FindMember("images"));
    if (imagesItr == document.MemberEnd()) return;
    std::vector<std::shared_ptr<Asset>> assets;
    for (const auto& gltfImagee : imagesItr->value.GetArray()) {
        auto imageUriItr(gltfImagee.FindMember("uri"));
        if (imageUriItr == gltfImagee.MemberEnd()) {
            auto imageAsset { std::make_shared<Asset>() };
            auto imageBufferViewItr(gltfImagee.FindMember("bufferView"));
            imageAsset->parsingOptions = a_AssetsContainer->parsingOptions;
            if (imageBufferViewItr == gltfImagee.MemberEnd()) {
                auto image = std::make_shared<SG::Image>();
                imageAsset->assets.push_back(image);
                imageAsset->SetLoaded(true);
            } else {
                imageAsset->SetUri(std::string("data:") + gltfImagee["mimeType"].GetString() + ",");
                auto bufferViewIndex { imageBufferViewItr->value.GetInt() };
                imageAsset->assets.push_back(a_Container.Get<SG::BufferView>("bufferViews", bufferViewIndex));
            }
            assets.push_back(imageAsset);
            continue;
        }
        auto uri = GLTF::CreateUri(path.parent_path(), imageUriItr->value.GetString());
        auto imageAsset = std::make_shared<Asset>(uri);
        imageAsset->parsingOptions = a_AssetsContainer->parsingOptions;
        assets.push_back(imageAsset);
    }
    std::vector<Parser::ParsingFuture> futures;
    for (const auto& asset : assets) futures.push_back(Parser::AddParsingTask(asset));
    for (auto& future : futures) a_Container.Add("images", future.get()->Get<SG::Image>().front());
}

std::shared_ptr<Asset> ParseGLTF(const std::shared_ptr<Asset>& a_Container)
{
    auto path = a_Container->GetUri().DecodePath();
    std::cout << path << std::endl;
    std::ifstream file(path);
    rapidjson::IStreamWrapper streamWrapper(file);
    rapidjson::Document document;
    rapidjson::ParseResult parseResult(document.ParseStream(streamWrapper));
    if (!parseResult) {
        debugLog("Invalid file !");
        return a_Container;
    }
    GLTF::Container container;
    ParseNodes(document, container);
    ParseCameras(document, container);
    ParseBuffers(path, document, container, a_Container);
    ParseBufferViews(document, container);
    auto bufferAccessors { ParseBufferAccessors(document, container) };
    ParseImages(path, document, container, a_Container);
    ParseTextures(document, container, a_Container);
    ParseMaterials(document, container);
    ParseMeshes(document, container, bufferAccessors);
    ParseSkins(document, container, bufferAccessors);
    ParseAnimations(document, container, bufferAccessors);
    ParseScenes(document, container);
    SetParenting(document, container);
    for (const auto& type : container.objects) {
        for (const auto& obj : type.second)
            a_Container->assets.push_back(obj);
    }
    a_Container->SetLoaded(true);
    return a_Container;
}
}

