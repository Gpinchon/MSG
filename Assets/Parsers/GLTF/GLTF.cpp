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
#include <SG/Material/Standard.hpp>
#include <SG/Material/MetallicRoughness.hpp>
#include <SG/Material/SpecularGlossiness.hpp>
#include <SG/Shape/Geometry.hpp>
#include <SG/Shape/Mesh.hpp>
#include <SG/Shape/MeshSkin.hpp>
#include <SG/Node/Scene.hpp>
#include <SG/Node/Renderable.hpp>
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
}


static inline auto ParseCameras(const rapidjson::Document& document)
{
    std::vector<std::shared_ptr<SG::Camera>> cameraVector;
    if (!document.HasMember("cameras")) return cameraVector;
    auto cameraIndex(0);
    for (const auto& camera : document["cameras"].GetArray()) {
        auto newCamera(std::make_shared<SG::Camera>("Camera" + std::to_string(cameraIndex)));
        if (std::string(camera["type"].GetString()) == "perspective") {
            auto perspective(camera["perspective"].GetObject());
            if (perspective.HasMember("zfar")) {
                SG::CameraProjection::Perspective projection;
                projection.zfar = perspective["zfar"].GetFloat();
                if (perspective.HasMember("znear"))
                    projection.znear = perspective["znear"].GetFloat();
                if (perspective.HasMember("yfov"))
                    projection.fov = glm::degrees(perspective["yfov"].GetFloat());
                newCamera->SetProjection(projection);
            }
            else {
                SG::CameraProjection::PerspectiveInfinite projection;
                if (perspective.HasMember("znear"))
                    projection.znear = perspective["znear"].GetFloat();
                if (perspective.HasMember("yfov"))
                    projection.fov = glm::degrees(perspective["yfov"].GetFloat());
                newCamera->SetProjection(projection);
            }
        }
        else if (std::string(camera["type"].GetString()) == "orthographic") {
            SG::CameraProjection::Orthographic projection;
            newCamera->SetProjection(projection);
        }
        cameraVector.push_back(newCamera);
        cameraIndex++;
    }
    return cameraVector;
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

static inline auto ParseTextures(const rapidjson::Document& document, std::vector<std::shared_ptr<SG::Image>>& images, std::shared_ptr<Asset> container)
{
    debugLog("Start parsing textures");
    std::vector<std::shared_ptr<SG::Texture2D>> textureVector;
    if (!document.HasMember("textures")) return textureVector;
    auto samplers = ParseTextureSamplers(document);
    auto textureIndex(0);
    for (const auto& textureValue : document["textures"].GetArray()) {
        auto texture = std::make_shared<SG::Texture2D>();
        if (textureValue.HasMember("source")) {
            auto source(textureValue["source"].GetInt());
            texture->SetImage(images.at(source));
        }
        if (textureValue.HasMember("sampler")) {
            auto sampler(samplers.at(textureValue["sampler"].GetInt()));
            texture->SetSampler(sampler);
        }
        texture->SetCompressed(container->parsingOptions.texture.compress);
        texture->SetCompressionQuality(container->parsingOptions.texture.compressionQuality);
        textureVector.push_back(texture);
        textureIndex++;
    }
    debugLog("Done parsing textures");
    return textureVector;
}

static inline auto ParseMaterialExtensions(const std::vector<std::shared_ptr<SG::Texture2D>>& textures, const rapidjson::Value& materialValue, std::shared_ptr<SG::Material> material)
{
    if (!materialValue.HasMember("extensions")) return;
    for (const auto& extension : materialValue["extensions"].GetObject()) {
        if (std::string(extension.name.GetString()) == "KHR_materials_pbrSpecularGlossiness") {
            const auto& pbrSpecularGlossiness = extension.value;
            SG::SpecularGlossinessParameters specularGlossiness;
            if (pbrSpecularGlossiness.HasMember("diffuseFactor")) {
                auto diffuseFactor(pbrSpecularGlossiness["diffuseFactor"].GetArray());
                specularGlossiness.SetDiffuse(glm::vec3(
                    diffuseFactor[0].GetFloat(),
                    diffuseFactor[1].GetFloat(),
                    diffuseFactor[2].GetFloat()));
                specularGlossiness.SetOpacity(diffuseFactor[3].GetFloat());
            }
            if (pbrSpecularGlossiness.HasMember("specularFactor")) {
                auto diffuseFactor(pbrSpecularGlossiness["specularFactor"].GetArray());
                specularGlossiness.SetSpecular(glm::vec3(
                    diffuseFactor[0].GetFloat(),
                    diffuseFactor[1].GetFloat(),
                    diffuseFactor[2].GetFloat()));
            }
            if (pbrSpecularGlossiness.HasMember("glossinessFactor")) {
                auto glossinessFactor(pbrSpecularGlossiness["glossinessFactor"].GetFloat());
                specularGlossiness.SetGlossiness(glossinessFactor);
            }
            if (pbrSpecularGlossiness.HasMember("diffuseTexture")) {
                auto textureObject(pbrSpecularGlossiness["diffuseTexture"].GetObject());
                specularGlossiness.SetTextureDiffuse(textures.at(textureObject["index"].GetInt()));
            }
            if (pbrSpecularGlossiness.HasMember("specularGlossinessTexture")) {
                auto textureObject(pbrSpecularGlossiness["specularGlossinessTexture"].GetObject());
                specularGlossiness.SetTextureSpecularGlossiness(textures.at(textureObject["index"].GetInt()));
            }
            material->AddParameters(specularGlossiness);
        }
    }
}

static inline auto ParseMaterials(const rapidjson::Document& document, std::vector<std::shared_ptr<SG::Texture2D>>& textures)
{
    debugLog("Start parsing materials");
    //auto textureVector = ParseTextures(path, document);
    std::vector<std::shared_ptr<SG::Material>> materialVector;
    if (!document.HasMember("materials")) return materialVector;
    auto materialIndex(0);
    for (const auto& materialValue : document["materials"].GetArray()) {
        auto material(std::make_shared<SG::Material>("Material " + std::to_string(materialIndex)));
        SG::StandardParameters standardParameters;
        if (materialValue.HasMember("name"))
            material->SetName(materialValue["name"].GetString());
        if (materialValue.HasMember("alphaCutoff"))
            standardParameters.SetOpacityCutoff(materialValue["alphaCutoff"].GetFloat());
        if (materialValue.HasMember("alphaMode")) {
            std::string alphaMode = materialValue["alphaMode"].GetString();
            if (alphaMode == "Opaque")
                standardParameters.SetOpacityMode(SG::StandardParameters::OpacityMode::Opaque);
            if (alphaMode == "MASK")
                standardParameters.SetOpacityMode(SG::StandardParameters::OpacityMode::Mask);
            if (alphaMode == "BLEND")
                standardParameters.SetOpacityMode(SG::StandardParameters::OpacityMode::Blend);
        }

        if (materialValue.HasMember("doubleSided"))
            standardParameters.SetDoubleSided(materialValue["doubleSided"].GetBool());
        if (materialValue.HasMember("emissiveFactor")) {
            auto emissiveFactor(materialValue["emissiveFactor"].GetArray());
            standardParameters.SetEmissive(glm::vec3(
                emissiveFactor[0].GetFloat(),
                emissiveFactor[1].GetFloat(),
                emissiveFactor[2].GetFloat()));
        }
        if (materialValue.HasMember("normalTexture")) {
            auto textureObject(materialValue["normalTexture"].GetObject());
            standardParameters.SetTextureNormal(textures.at(textureObject["index"].GetInt()));
        }
        if (materialValue.HasMember("emissiveTexture")) {
            auto textureObject(materialValue["emissiveTexture"].GetObject());
            standardParameters.SetTextureEmissive(textures.at(textureObject["index"].GetInt()));
        }
        if (materialValue.HasMember("occlusionTexture")) {
            auto textureObject(materialValue["occlusionTexture"].GetObject());
            standardParameters.SetTextureAO(textures.at(textureObject["index"].GetInt()));
        }
        material->AddParameters(standardParameters);
        if (materialValue.HasMember("pbrMetallicRoughness")) {
            auto pbrMetallicRoughness(materialValue["pbrMetallicRoughness"].GetObject());
            SG::MetallicRoughnessParameters mraParameters{};
            mraParameters.SetRoughness(1);
            mraParameters.SetMetallic(1);
            if (pbrMetallicRoughness.HasMember("metallicRoughnessTexture")) {
                auto textureObject(pbrMetallicRoughness["metallicRoughnessTexture"].GetObject());
                mraParameters.SetTextureMetallicRoughness(textures.at(textureObject["index"].GetInt()));
            }
            if (pbrMetallicRoughness.HasMember("baseColorFactor")) {
                auto baseColor(pbrMetallicRoughness["baseColorFactor"].GetArray());
                mraParameters.SetAlbedo(glm::vec3(baseColor[0].GetFloat(),
                    baseColor[1].GetFloat(),
                    baseColor[2].GetFloat()));
                mraParameters.SetOpacity(baseColor[3].GetFloat());
            }
            if (pbrMetallicRoughness.HasMember("metallicFactor"))
                mraParameters.SetMetallic(pbrMetallicRoughness["metallicFactor"].GetFloat());
            if (pbrMetallicRoughness.HasMember("roughnessFactor"))
                mraParameters.SetRoughness(pbrMetallicRoughness["roughnessFactor"].GetFloat());
            if (pbrMetallicRoughness.HasMember("baseColorTexture")) {
                auto textureObject(pbrMetallicRoughness["baseColorTexture"].GetObject());
                mraParameters.SetTextureAlbedo(textures.at(textureObject["index"].GetInt()));
            }
            material->AddParameters(mraParameters);
        }
        ParseMaterialExtensions(textures, materialValue, material);
        materialVector.push_back(material);
        materialIndex++;
    }
    debugLog("Done parsing materials");
    return materialVector;
}

static inline std::vector<std::shared_ptr<SG::Buffer>> ParseBuffers(const std::filesystem::path path, const rapidjson::Document& document, std::shared_ptr<Asset> container)
{
    debugLog("Start parsing buffers");
    if (!document.HasMember("buffers")) return {};
    std::vector<std::shared_ptr<Asset>> assetVector;
    for (const auto& bufferValue : document["buffers"].GetArray()) {
        auto asset { std::make_shared<Asset>() };
        asset->parsingOptions = container->parsingOptions;
        asset->SetUri(GLTF::CreateUri(path.parent_path(), bufferValue["uri"].GetString()));
        if (bufferValue.HasMember("name"))
            asset->SetName(bufferValue["name"].GetString());
        assetVector.push_back(asset);
    }
    std::vector<std::shared_ptr<SG::Buffer>> buffers;
    std::vector<std::future<std::shared_ptr<Asset>>> parsingFuture;
    for (const auto& asset : assetVector) parsingFuture.push_back(Parser::AddParsingTask(asset));
    for (auto& future : parsingFuture) buffers.push_back(future.get()->Get<SG::Buffer>().front());
    debugLog("Done parsing buffers");
    return buffers;
}

static inline auto ParseBufferViews(const rapidjson::Document& document, std::vector<std::shared_ptr<SG::Buffer>> buffers)
{
    debugLog("Start parsing bufferViews");
    std::vector<std::shared_ptr<SG::BufferView>> bufferViewVector;
    if (!document.HasMember("bufferViews")) return bufferViewVector;
    for (const auto& bufferViewValue : document["bufferViews"].GetArray()) {
        auto bufferView(std::make_shared<SG::BufferView>());
        bufferView->SetBuffer(buffers.at(bufferViewValue["buffer"].GetInt()));
        bufferView->SetByteLength(bufferViewValue["byteLength"].GetInt());
        if (bufferViewValue.HasMember("name"))
            bufferView->SetName(bufferViewValue["name"].GetString());
        if (bufferViewValue.HasMember("byteOffset"))
            bufferView->SetByteOffset(bufferViewValue["byteOffset"].GetInt());
        if (bufferViewValue.HasMember("byteStride"))
            bufferView->SetByteStride(bufferViewValue["byteStride"].GetInt());
        //if (bufferViewValue.HasMember("target"))
        //    bufferView->SetType(GLTF::GetBufferViewType(GLTF::BufferViewType(bufferViewValue["target"].GetInt())));
        bufferViewVector.push_back(bufferView);
    }
    debugLog("Done parsing bufferViews");
    return bufferViewVector;
}

static inline auto ParseBufferAccessors(const rapidjson::Document& document, std::vector<std::shared_ptr<SG::BufferView>>& bufferViews)
{
    debugLog("Start parsing bufferAccessors");
    std::vector<SG::BufferAccessor> bufferAccessorVector;
    if (!document.HasMember("accessors"))
        return bufferAccessorVector;
    auto bufferAccessorIndex(0);
    for (const auto& bufferAccessorValue : document["accessors"].GetArray()) {
        size_t byteOffset{ 0 };
        std::shared_ptr<SG::BufferView> bufferView;
        if (bufferAccessorValue.HasMember("bufferView"))
            bufferView = bufferViews.at(bufferAccessorValue["bufferView"].GetInt());
        if (bufferAccessorValue.HasMember("byteOffset"))
            byteOffset = bufferAccessorValue["byteOffset"].GetInt();
        SG::BufferAccessor bufferAccessor{
            bufferView,
            byteOffset,
            static_cast<size_t>(bufferAccessorValue["count"].GetInt()),
            GLTF::GetAccessorComponentType(GLTF::ComponentType(bufferAccessorValue["componentType"].GetInt())),
            static_cast<uint8_t>(GLTF::GetAccessorComponentNbr(bufferAccessorValue["type"].GetString())),
        };
        if (bufferAccessorValue.HasMember("name"))
            bufferAccessor.SetName(bufferAccessorValue["name"].GetString());
        if (bufferAccessorValue.HasMember("normalized"))
            bufferAccessor.SetNormalized(bufferAccessorValue["normalized"].GetBool());
        bufferAccessorVector.push_back(bufferAccessor);
        bufferAccessorIndex++;
    }
    debugLog("Done parsing bufferAccessors");
    return bufferAccessorVector;
}

static inline auto ParseMeshes(const rapidjson::Document& document, const std::vector<std::shared_ptr<SG::Material>>& materials, const std::vector<SG::BufferAccessor>& bufferAccessors)
{
    debugLog("Start parsing meshes");
    std::vector<std::shared_ptr<SG::Mesh>> meshVector;
    auto meshesItr(document.FindMember("meshes"));
    if (meshesItr == document.MemberEnd()) {
        debugLog("No meshes found");
        return meshVector;
    }
    auto defaultMaterial(std::make_shared<SG::Material>("defaultMaterial"));
    defaultMaterial->AddParameters(SG::StandardParameters());
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
                    if (size_t(material.GetInt()) >= materials.size())
                        std::cerr << "Material index " << material.GetInt() << " out of bound " << materials.size() << std::endl;
                    materialPtr = materials.at(material.GetInt());
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
        meshVector.push_back(currentMesh);
    }
    debugLog("Done parsing meshes");
    return meshVector;
}

static inline auto ParseNodes(const rapidjson::Document& document)
{
    std::vector<std::shared_ptr<SG::Node>> nodeVector;
    auto nodeItr(document.FindMember("nodes"));
    if (nodeItr == document.MemberEnd())
        return nodeVector;
    int nodeIndex = 0;
    for (const auto& node : nodeItr->value.GetArray()) {
        std::shared_ptr<SG::Node> newNode;
        if (node.HasMember("children") || node.HasMember("camera") || node.HasMember("mesh"))
            newNode = std::make_shared<SG::NodeGroup>("NodeGroup_" + std::to_string(nodeIndex));
        else
            newNode = std::make_shared<SG::Node>("Node_" + std::to_string(nodeIndex));
        auto transform(newNode);
        if (node.HasMember("name"))
            newNode->SetName(node["name"].GetString());
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
        if (node.HasMember("translation")) {
            const auto& position(node["translation"].GetArray());
            glm::vec3 positionVec3{};
            positionVec3[0] = position[0].GetFloat();
            positionVec3[1] = position[1].GetFloat();
            positionVec3[2] = position[2].GetFloat();
            transform->SetLocalPosition(positionVec3);
        }
        if (node.HasMember("rotation")) {
            const auto& rotation(node["rotation"].GetArray());
            glm::quat rotationQuat{};
            rotationQuat[0] = rotation[0].GetFloat();
            rotationQuat[1] = rotation[1].GetFloat();
            rotationQuat[2] = rotation[2].GetFloat();
            rotationQuat[3] = rotation[3].GetFloat();
            transform->SetLocalRotation(glm::normalize(rotationQuat));
        }
        if (node.HasMember("scale")) {
            const auto& scale(node["scale"].GetArray());
            glm::vec3 scaleVec3{};
            scaleVec3[0] = scale[0].GetFloat();
            scaleVec3[1] = scale[1].GetFloat();
            scaleVec3[2] = scale[2].GetFloat();
            transform->SetLocalScale(scaleVec3);
        }
        nodeVector.push_back(newNode);
        nodeIndex++;
    }
    return nodeVector;
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

static inline auto ParseAnimations(const rapidjson::Document& document, const std::vector<std::shared_ptr<SG::Node>>& nodes, const std::vector<SG::BufferAccessor>& bufferAccessors)
{
    std::vector<std::shared_ptr<SG::Animation>> animations;
    if (!document.HasMember("animations"))
        return animations;
    for (const auto& animation : document["animations"].GetArray()) {
        auto newAnimation(std::make_shared<SG::Animation>());
        if (animation.HasMember("name"))
            newAnimation->SetName(animation["name"].GetString());
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
                auto& node(nodes.at(target["node"].GetInt()));
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
        animations.push_back(newAnimation);
    }
    return animations;
}

static inline auto ParseSkins(const rapidjson::Document& document, const std::vector<std::shared_ptr<SG::Node>>& nodes, const std::vector<SG::BufferAccessor>& bufferAccessors)
{
    debugLog("Start parsing Skins");
    std::vector<std::shared_ptr<SG::Mesh::Skin>> skins;
    if (!document.HasMember("skins"))
        return skins;
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
                newSkin->AddJoint(nodes.at(joint.GetInt()));
        }
        skins.push_back(newSkin);
    }
    debugLog("Done parsing Skins");
    return skins;
}

static inline auto SetParenting(const rapidjson::Document& document,
    const std::vector<std::shared_ptr<SG::Node>>& nodes,
    const std::vector<std::shared_ptr<SG::Mesh>>& meshes,
    const std::vector<std::shared_ptr<SG::Mesh::Skin>>& meshSkins,
    const std::vector<std::shared_ptr<SG::Camera>>& cameras)
{
    auto nodeItr(document.FindMember("nodes"));
    if (nodeItr == document.MemberEnd())
        return;
    //Build parenting relationship
    auto nodeIndex = 0;
    for (const auto& gltfNode : nodeItr->value.GetArray()) {
        auto node(nodes.at(nodeIndex));
        if (gltfNode.HasMember("mesh")) {
            auto mesh(meshes.at(gltfNode["mesh"].GetInt()));
            auto renderable{ std::make_shared<SG::Renderable>() };
            if (gltfNode.HasMember("skin"))
                mesh->SetSkin(meshSkins.at(gltfNode["skin"].GetInt()));
            renderable->Add(mesh);
            renderable->SetParent(std::static_pointer_cast<SG::NodeGroup>(node));            
        }
        if (gltfNode.HasMember("camera")) {
            auto &camera = cameras.at(gltfNode["camera"].GetInt());
            camera->SetParent(std::static_pointer_cast<SG::NodeGroup>(node));
        }
        if (gltfNode.HasMember("children")) {
            for (const auto& child : gltfNode["children"].GetArray()) {
                auto &childNode = nodes.at(child.GetInt());
                nodes.at(child.GetInt())->SetParent(std::static_pointer_cast<SG::NodeGroup>(node));
                std::cout << "Node parenting " << node->GetName() << " -> " << nodes.at(child.GetInt())->GetName() << std::endl;
            }
        }
        nodeIndex++;
    }
}

static inline auto ParseScenes(
    const rapidjson::Document& document,
    const std::vector<std::shared_ptr<SG::Node>>& nodes,
    const std::vector<std::shared_ptr<SG::Animation>>& animations)
{
    std::vector<std::shared_ptr<SG::Scene>> sceneVector;
    auto scenes(document["scenes"].GetArray());
    int sceneIndex = 0;
    for (const auto& scene : scenes) {
        std::cout << "found scene" << std::endl;
        auto newScene(std::make_shared<SG::Scene>(std::to_string(sceneIndex)));
        for (const auto& node : scene["nodes"].GetArray()) {
            nodes.at(node.GetInt())->SetParent(newScene);
            std::cout << nodes.at(node.GetInt())->GetName() << " is part of Scene " << newScene->GetName() << std::endl;
        }
        for (const auto& animation : animations) {
            newScene->AddAnimation(animation);
            std::cout << (animation ? animation->GetName() : "nullptr") << std::endl;
        }
        sceneVector.push_back(newScene);
        sceneIndex++;
    }
    return sceneVector;
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

static inline std::vector<std::shared_ptr<SG::Image>> ParseImages(const std::filesystem::path path, const rapidjson::Document& document, std::vector<std::shared_ptr<SG::BufferView>>& bufferViews, std::shared_ptr<Asset> container)
{
    auto imagesItr(document.FindMember("images"));
    if (imagesItr == document.MemberEnd())
        return {};
    std::vector<std::shared_ptr<Asset>> assets;
    for (const auto& gltfImagee : imagesItr->value.GetArray()) {
        auto imageUriItr(gltfImagee.FindMember("uri"));
        if (imageUriItr == gltfImagee.MemberEnd()) {
            auto imageAsset { std::make_shared<Asset>() };
            auto imageBufferViewItr(gltfImagee.FindMember("bufferView"));
            imageAsset->parsingOptions = container->parsingOptions;
            if (imageBufferViewItr == gltfImagee.MemberEnd()) {
                auto image = std::make_shared<SG::Image>();
                imageAsset->assets.push_back(image);
                imageAsset->SetLoaded(true);
            } else {
                imageAsset->SetUri(std::string("data:") + gltfImagee["mimeType"].GetString() + ",");
                auto bufferViewIndex { imageBufferViewItr->value.GetInt() };
                imageAsset->assets.push_back(bufferViews.at(bufferViewIndex));
            }
            assets.push_back(imageAsset);
            continue;
        }
        auto uri = GLTF::CreateUri(path.parent_path(), imageUriItr->value.GetString());
        auto imageAsset = std::make_shared<Asset>(uri);
        imageAsset->parsingOptions = container->parsingOptions;
        assets.push_back(imageAsset);
    }
    std::vector<std::shared_ptr<SG::Image>> images;
    std::vector<Parser::ParsingFuture> futures;
    for (const auto& asset : assets) futures.push_back(Parser::AddParsingTask(asset));
    for (auto& future : futures) images.push_back(future.get()->Get<SG::Image>().front());
    return images;
}

std::shared_ptr<Asset> ParseGLTF(const std::shared_ptr<Asset>& container)
{
    auto path = container->GetUri().DecodePath();
    std::cout << path << std::endl;
    std::ifstream file(path);
    rapidjson::IStreamWrapper streamWrapper(file);
    rapidjson::Document document;
    rapidjson::ParseResult parseResult(document.ParseStream(streamWrapper));
    if (!parseResult) {
        debugLog("Invalid file !");
        return container;
    }
    auto nodes { ParseNodes(document) };
    auto cameras { ParseCameras(document) };
    auto buffers { ParseBuffers(path, document, container) };
    auto bufferViews { ParseBufferViews(document, buffers) };
    auto bufferAccessors { ParseBufferAccessors(document, bufferViews) };
    auto images { ParseImages(path, document, bufferViews, container) };
    auto textures { ParseTextures(document, images, container) };
    auto materials { ParseMaterials(document, textures) };
    auto meshes { ParseMeshes(document, materials, bufferAccessors) };
    auto skins { ParseSkins(document, nodes, bufferAccessors) };
    auto animations { ParseAnimations(document, nodes, bufferAccessors) };
    auto scenes { ParseScenes(document, nodes, animations) };
    for (const auto& node : nodes)
        container->assets.push_back(node);
    for (const auto& camera : cameras)
        container->assets.push_back(camera);
    for (const auto& buffer : buffers)
        container->assets.push_back(buffer);
    for (const auto& bufferView : bufferViews)
        container->assets.push_back(bufferView);
    for (const auto& image : images)
        container->assets.push_back(image);
    for (const auto& texture : textures)
        container->assets.push_back(texture);
    for (const auto& material : materials)
        container->assets.push_back(material);
    for (const auto& meshe : meshes)
        container->assets.push_back(meshe);
    for (const auto& skin : skins)
        container->assets.push_back(skin);
    for (const auto& animation : animations)
        container->assets.push_back(animation);
    for (const auto& scene : scenes)
        container->assets.push_back(scene);
    SetParenting(document, nodes, meshes, skins, cameras);
    container->SetLoaded(true);
    return container;
}
}

