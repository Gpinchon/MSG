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


static inline void ParseCameras(const rapidjson::Document& document, GLTF::Container& a_Container, const std::shared_ptr<Asset>& a_AssetsContainer)
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

static inline void ParseTextureSamplers(const rapidjson::Value& a_JSONValue, GLTF::Container& a_Container, const std::shared_ptr<Asset>& a_AssetsContainer)
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
        a_Container.Add("samplers", sampler);
        a_AssetsContainer->assets.push_back(sampler);
    }
}

static inline void ParseTextures(const rapidjson::Value& a_JSONValue, GLTF::Container& a_Container, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!a_JSONValue.HasMember("textures")) return;
    debugLog("Start parsing textures");
    for (const auto& textureValue : a_JSONValue["textures"].GetArray()) {
        auto texture = std::make_shared<SG::Texture2D>();
        const auto source = GLTF::Parse(textureValue, "source", true, -1);
        const auto sampler = GLTF::Parse(textureValue, "sampler", true, -1);
        if (source > -1) texture->SetImage(a_Container.Get<SG::Image>("images", source));
        if (sampler > -1) texture->SetSampler(a_Container.Get<SG::TextureSampler>("samplers", sampler));
        texture->SetCompressed(a_AssetsContainer->parsingOptions.texture.compress);
        texture->SetCompressionQuality(a_AssetsContainer->parsingOptions.texture.compressionQuality);
        a_Container.Add("textures", texture);
        a_AssetsContainer->assets.push_back(texture);
    }
    debugLog("Done parsing textures");
}

static inline auto ParseTextureInfo(GLTF::Container& a_Container, const rapidjson::Value& a_JSONValue)
{
    SG::TextureInfo texture;
    texture.texture = a_Container.Get<SG::Texture>("textures", GLTF::Parse<int>(a_JSONValue, "index"));
    texture.texCoord = GLTF::Parse(a_JSONValue, "texCoord", true, texture.texCoord);
    return texture;
}

static inline auto ParseSpecularGlossiness(GLTF::Container& a_Container, const rapidjson::Value& extension) {
    SG::SpecularGlossinessExtension specGloss;
    specGloss.SetDiffuseFactor(GLTF::Parse(extension, "diffuseFactor", true, specGloss.GetDiffuseFactor()));
    specGloss.SetSpecularFactor(GLTF::Parse(extension, "specularFactor", true, specGloss.GetSpecularFactor()));
    specGloss.SetGlossinessFactor(GLTF::Parse(extension, "glossinessFactor", true, specGloss.GetGlossinessFactor()));
    if (extension.HasMember("diffuseTexture")) {
        specGloss.SetDiffuseTexture(ParseTextureInfo(a_Container, extension["diffuseTexture"]));
    }
    if (extension.HasMember("specularGlossinessTexture")) {
        specGloss.SetSpecularGlossinessTexture(ParseTextureInfo(a_Container, extension["specularGlossinessTexture"]));
    }
    return specGloss;
}

static inline auto ParseSheen(GLTF::Container& a_Container, const rapidjson::Value& a_Extension) {
    SG::SheenExtension sheen;
    sheen.SetColorFactor(GLTF::Parse(a_Extension, "sheenColorFactor", true, sheen.GetColorFactor()));
    sheen.SetRoughnessFactor(GLTF::Parse(a_Extension, "sheenRoughnessFactor", true, sheen.GetRoughnessFactor()));
    if (a_Extension.HasMember("sheenColorTexture")) {
        sheen.SetColorTexture(ParseTextureInfo(a_Container, a_Extension["sheenColorTexture"]));
    }
    if (a_Extension.HasMember("sheenRoughnessTexture")) {
        sheen.SetRoughnessTexture(ParseTextureInfo(a_Container, a_Extension["sheenRoughnessTexture"]));
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
    if (a_Extension.HasMember("baseColorTexture"))
        mra.SetColorTexture(ParseTextureInfo(a_Container, a_Extension["baseColorTexture"]));
    if (a_Extension.HasMember("metallicRoughnessTexture"))
        mra.SetMetallicRoughnessTexture(ParseTextureInfo(a_Container, a_Extension["metallicRoughnessTexture"]));
    return mra;
}

static inline void ParseMaterials(const rapidjson::Value& a_JSONValue, GLTF::Container& a_Container, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!a_JSONValue.HasMember("materials")) return;
    debugLog("Start parsing materials");
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
            SG::Material::NormalTextureInfo texture = ParseTextureInfo(a_Container, materialValue["normalTexture"]);
            texture.scale = GLTF::Parse(texInfo, "scale", true, texture.scale);
            material->SetNormalTexture(texture);
        }
        if (materialValue.HasMember("emissiveTexture"))
            material->SetEmissiveTexture(ParseTextureInfo(a_Container, materialValue["emissiveTexture"]));
        if (materialValue.HasMember("occlusionTexture")) {
            SG::Material::OcclusionTextureInfo texture = ParseTextureInfo(a_Container, materialValue["occlusionTexture"]);
            texture.strength = GLTF::Parse(materialValue["occlusionTexture"], "strength", true, texture.strength);
            material->SetOcclusionTexture(texture);
        }
        if (materialValue.HasMember("pbrMetallicRoughness"))
            material->AddExtension(ParseMetallicRoughness(a_Container, materialValue["pbrMetallicRoughness"]));
        if (materialValue.HasMember("extensions")) ParseMaterialExtensions(a_Container, materialValue, material);
        a_Container.Add("materials", material);
        a_AssetsContainer->assets.push_back(material);
    }
    debugLog("Done parsing materials");
}

static inline void ParseBuffers(const std::filesystem::path path, const rapidjson::Value& a_JSONValue, GLTF::Container& a_Container, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!a_JSONValue.HasMember("buffers")) return;
    debugLog("Start parsing buffers");
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
        auto buffer = future.get()->Get<SG::Buffer>().front();
        a_Container.Add("buffers", buffer);
        a_AssetsContainer->assets.push_back(buffer);
    }
    debugLog("Done parsing buffers");
}

static inline void ParseBufferViews(const rapidjson::Document& document, GLTF::Container& a_Container, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!document.HasMember("bufferViews")) return;
    debugLog("Start parsing bufferViews");
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
        a_AssetsContainer->assets.push_back(bufferView);
    }
    debugLog("Done parsing bufferViews");
}

static inline void ParseBufferAccessors(const rapidjson::Value& a_JSONValue, GLTF::Container& a_Container, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!a_JSONValue.HasMember("accessors")) return;
    debugLog("Start parsing bufferAccessors");
    for (const auto& gltfbufferAccessor : a_JSONValue["accessors"].GetArray()) {
        auto bufferAccessor = std::make_shared<SG::BufferAccessor>();
        std::shared_ptr<SG::BufferView> bufferView;
        bufferAccessor->SetName(GLTF::Parse(gltfbufferAccessor, "name", true, bufferAccessor->GetName()));
        bufferAccessor->SetSize(GLTF::Parse<size_t>(gltfbufferAccessor, "count"));
        bufferAccessor->SetComponentNbr(GLTF::GetAccessorComponentNbr(GLTF::Parse<std::string>(gltfbufferAccessor, "type")));
        bufferAccessor->SetComponentType(GLTF::GetAccessorComponentType(GLTF::ComponentType(GLTF::Parse<int>(gltfbufferAccessor, "componentType"))));
        if (const auto bufferViewIndex = GLTF::Parse(gltfbufferAccessor, "bufferView", true, -1); bufferViewIndex > -1)
            bufferAccessor->SetBufferView(a_Container.Get<SG::BufferView>("bufferViews", bufferViewIndex));
        bufferAccessor->SetByteOffset(GLTF::Parse(gltfbufferAccessor, "byteOffset", true, bufferAccessor->GetByteOffset()));
        bufferAccessor->SetNormalized(GLTF::Parse(gltfbufferAccessor, "normalized", true, bufferAccessor->GetNormalized()));
        a_Container.Add("accessors", bufferAccessor);
        a_AssetsContainer->assets.push_back(bufferAccessor);
    }
    debugLog("Done parsing bufferAccessors");
}

static inline void ParseMeshes(const rapidjson::Value& a_JSONValue, GLTF::Container& a_Container, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    debugLog("Start parsing meshes");
    if (!a_JSONValue.HasMember("meshes")) return;
    auto defaultMaterial(std::make_shared<SG::Material>("defaultMaterial"));
    for (const auto& gltfMesh : a_JSONValue["meshes"].GetArray()) {
        auto mesh = std::make_shared<SG::Mesh>();
        mesh->SetName(GLTF::Parse(gltfMesh, "name", true, mesh->GetName()));
        if (gltfMesh.HasMember("primitives")) {
            for (const auto& primitive : gltfMesh["primitives"].GetArray()) {
                auto geometry(std::make_shared<SG::Geometry>());
                auto material{ defaultMaterial };
                if (const auto materialIndex = GLTF::Parse(primitive, "material", true, -1); materialIndex > -1)
                    material = a_Container.Get<SG::Material>("materials", materialIndex);
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
                auto accessorIndex = GLTF::Parse(primitive, "indices", true, -1);
                if (accessorIndex > -1) geometry->SetIndices(*a_Container.Get<SG::BufferAccessor>("accessors", accessorIndex));
                geometry->SetDrawingMode(GLTF::GetGeometryDrawingMode(GLTF::DrawingMode(GLTF::Parse(primitive, "mode", true, int(GLTF::DrawingMode::Triangles)))));
                mesh->AddGeometry(geometry, material);
            }
        }
        a_Container.Add("meshes", mesh);
        a_AssetsContainer->assets.push_back(mesh);
    }
    debugLog("Done parsing meshes");
}

static inline void ParseNodes(const rapidjson::Value& a_JSONValue, GLTF::Container& a_Container, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!a_JSONValue.HasMember("nodes")) return;
    for (const auto& gltfNode : a_JSONValue["nodes"].GetArray()) {
        std::shared_ptr<SG::Node> node;
        if (gltfNode.HasMember("children") || gltfNode.HasMember("camera") || gltfNode.HasMember("mesh"))
            node = std::make_shared<SG::NodeGroup>();
        else node = std::make_shared<SG::Node>();
        node->SetName(GLTF::Parse(gltfNode, "name", true, node->GetName()));
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
            node->SetLocalPosition(translation);
            node->SetLocalRotation(rotation);
            node->SetLocalScale(scale);
        }
        node->SetLocalPosition(GLTF::Parse(gltfNode, "translation", true, node->GetLocalPosition()));
        node->SetLocalScale(GLTF::Parse(gltfNode, "scale", true, node->GetLocalScale()));
        node->SetLocalRotation(GLTF::Parse(gltfNode, "rotation", true, node->GetLocalRotation()));
        a_Container.Add("nodes", node);
        a_AssetsContainer->assets.push_back(node);
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

static inline void ParseAnimations(const rapidjson::Document& document, GLTF::Container& a_Container, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!document.HasMember("animations")) return;
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
                auto& node = a_Container.Get<SG::Node>("nodes", GLTF::Parse<int>(target, "node"));
                const auto path = GLTF::Parse<std::string>(target, "path", true, "");
                const auto input  = a_Container.Get<SG::BufferAccessor>("accessors", GLTF::Parse<int>(sampler, "input"));
                const auto output = a_Container.Get<SG::BufferAccessor>("accessors", GLTF::Parse<int>(sampler, "output"));
                if (path == "translation") {
                    auto newChannel = GenerateAnimationChannel<glm::vec3>(channelInterpolation, *output, *input);
                    newChannel.target = node;
                    newAnimation->AddChannelPosition(newChannel);
                }
                else if (path == "rotation") {
                    auto newChannel = GenerateAnimationChannel<glm::quat>(channelInterpolation, *output, *input);
                    newChannel.target = node;
                    newAnimation->AddChannelRotation(newChannel);
                }
                else if (path == "scale") {
                    auto newChannel = GenerateAnimationChannel<glm::vec3>(channelInterpolation, *output, *input);
                    newChannel.target = node;
                    newAnimation->AddChannelScale(newChannel);
                }
                else if (path == "weights") {
                    //newAnimation->GetChannelPosition().target = node;
                }
                else throw std::runtime_error("Unknown animation path");
            }
        }
        a_Container.Add("animations", newAnimation);
        a_AssetsContainer->assets.push_back(newAnimation);
    }
}

static inline void ParseSkins(const rapidjson::Document& a_Document, GLTF::Container& a_Container, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    debugLog("Start parsing Skins");
    if (!a_Document.HasMember("skins")) return;
    for (const auto& gltfSkin : a_Document["skins"].GetArray()) {
        auto skin(std::make_shared<SG::Mesh::Skin>());
        if (gltfSkin.HasMember("name"))
            skin->SetName(gltfSkin["name"].GetString());
        if (auto inverseBindMatrices = GLTF::Parse(gltfSkin, "inverseBindMatrices", true, -1); inverseBindMatrices > -1)
            skin->SetInverseBindMatrices(*a_Container.Get<SG::BufferAccessor>("accessors", inverseBindMatrices));
        if (gltfSkin.HasMember("joints")) {
            for (const auto& joint : gltfSkin["joints"].GetArray())
                skin->AddJoint(a_Container.Get<SG::Node>("nodes", joint.GetInt()));
        }
        a_Container.Add("skins", skin);
        a_AssetsContainer->assets.push_back(skin);
    }
    debugLog("Done parsing Skins");
}

static inline void ParseScenes(const rapidjson::Value& a_JSONValue, GLTF::Container& a_Container, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!a_JSONValue.HasMember("scenes")) return;
    for (const auto& gltfScene : a_JSONValue["scenes"].GetArray()) {
        std::cout << "found scene" << std::endl;
        auto scene = std::make_shared<SG::Scene>();
        for (const auto& node : gltfScene["nodes"].GetArray()) {
            a_Container.Get<SG::Node>("nodes", node.GetInt())->SetParent(scene);
            std::cout << a_Container.Get<SG::Node>("nodes", node.GetInt())->GetName() << " is part of Scene " << scene->GetName() << std::endl;
        }
        for (const auto& animation : a_Container.Get("animations")) {
            scene->AddAnimation(std::static_pointer_cast<SG::Animation>(animation));
            std::cout << (animation ? animation->GetName() : "nullptr") << std::endl;
        }
        a_Container.Add("scenes", scene);
        a_AssetsContainer->assets.push_back(scene);
    }
}

static inline auto ParseLights(const rapidjson::Value& a_JSONValue)
{
    std::vector<std::shared_ptr<SG::Light>> lightsVector;
    const auto& extensions(a_JSONValue.FindMember("extensions"));
    if (extensions != a_JSONValue.MemberEnd()) {
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

static inline void ParseImages(const std::filesystem::path path, const rapidjson::Document& a_JSONValue, GLTF::Container& a_Container, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!a_JSONValue.HasMember("images")) return;
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
                imageAsset->assets.push_back(std::make_shared<SG::Image>());
                imageAsset->SetLoaded(true);
            } else {
                const auto mimeType = GLTF::Parse<std::string>(gltfImage, "mimeType");
                imageAsset->SetUri(std::string("data:") + mimeType + ",");
                imageAsset->parsingOptions.data.useBufferView = true;
                imageAsset->SetBufferView(a_Container.Get<SG::BufferView>("bufferViews", bufferViewIndex));
            }
        }
        assets.push_back(imageAsset);
    }
    std::vector<Parser::ParsingFuture> futures;
    for (const auto& asset : assets) futures.push_back(Parser::AddParsingTask(asset));
    for (auto& future : futures) {
        if (future.wait_for(std::chrono::seconds(10)) != std::future_status::ready) {
            debugLog("future timedout");
            continue;
        }
        auto asset = future.get();
        if (asset->GetLoaded()) a_Container.Add("images", asset->Get<SG::Image>().front());
        else debugLog("Error while parsing" + std::string(asset->GetUri()));
    }
}

static inline void SetParenting(const rapidjson::Document& a_Document, GLTF::Container& a_Container)
{
    if (!a_Document.HasMember("nodes")) return;
    //Build parenting relationship
    auto nodeIndex = 0;
    for (const auto& gltfNode : a_Document["nodes"].GetArray()) {
        auto node = a_Container.Get<SG::Node>("nodes", nodeIndex);
        auto meshIndex = GLTF::Parse(gltfNode, "mesh", true, -1);
        auto skinIndex = GLTF::Parse(gltfNode, "skin", true, -1);
        auto cameraIndex = GLTF::Parse(gltfNode, "camera", true, -1);
        if (meshIndex > -1) {
            auto mesh(a_Container.Get<SG::Mesh>("meshes", meshIndex));
            mesh->SetParent(std::static_pointer_cast<SG::NodeGroup>(node));
            if (skinIndex > -1) mesh->SetSkin(a_Container.Get<SG::Mesh::Skin>("skins", skinIndex));
        }
        if (cameraIndex > -1) {
            auto& camera = a_Container.Get<SG::Camera>("cameras", cameraIndex);
            camera->SetParent(std::static_pointer_cast<SG::NodeGroup>(node));
        }
        if (gltfNode.HasMember("children")) {
            for (const auto& child : gltfNode["children"].GetArray()) {
                const auto& childNode = a_Container.Get<SG::Node>("nodes", child.GetInt());
                childNode->SetParent(std::static_pointer_cast<SG::NodeGroup>(node));
                std::cout << "Node parenting " << node->GetName() << " -> " << childNode->GetName() << std::endl;
            }
        }
        nodeIndex++;
    }
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
    ParseNodes(document, container, a_Container);
    ParseCameras(document, container, a_Container);
    ParseBuffers(path, document, container, a_Container);
    ParseBufferViews(document, container, a_Container);
    ParseImages(path, document, container, a_Container);
    ParseTextureSamplers(document, container, a_Container);
    ParseTextures(document, container, a_Container);
    ParseMaterials(document, container, a_Container);
    ParseBufferAccessors(document, container, a_Container);
    ParseMeshes(document, container, a_Container);
    ParseSkins(document, container, a_Container);
    ParseAnimations(document, container, a_Container);
    ParseScenes(document, container, a_Container);
    SetParenting(document, container);
    a_Container->SetAssetType("model/gltf+json");
    a_Container->SetLoaded(true);
    return a_Container;
}
}

