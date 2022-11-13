/*
* @Author: gpinchon
* @Date:   2020-08-07 18:36:53
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-11 16:17:13
*/

#include <glm/ext.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>

#include <Animations/Animation.hpp>
#include <Animations/Channel.hpp>
#include <Assets/Asset.hpp>
#include <Assets/Parser.hpp>
#include <Assets/BinaryData.hpp>
#include <Assets/Image.hpp>
#include <Buffer/Accessor.hpp>
#include <Buffer/View.hpp>
#include <Cameras/Camera.hpp>
#include <Debug.hpp>
#include <Material/Standard.hpp>
#include <Shapes/Geometry.hpp>
#include <Shapes/Mesh/Mesh.hpp>
#include <Shapes/Mesh/MeshSkin.hpp>
#include <Parser/InternalTools.hpp>
#include <Nodes/Scene.hpp>
#include <Nodes/Renderable.hpp>
#include <Texture/Texture2D.hpp>
#include <Texture/Sampler.hpp>
#include <Material/MetallicRoughness.hpp>
#include <Material/SpecularGlossiness.hpp>

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

namespace TabGraph::GLTF {
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
        return Textures::Sampler::Filter::Nearest;
    case TextureFilter::Linear:
        return Textures::Sampler::Filter::Linear;
    case TextureFilter::NearestMipmapNearest:
        return Textures::Sampler::Filter::NearestMipmapNearest;
    case TextureFilter::LinearMipmapNearest:
        return Textures::Sampler::Filter::LinearMipmapNearest;
    case TextureFilter::NearestMipmapLinear:
        return Textures::Sampler::Filter::NearestMipmapLinear;
    case TextureFilter::LinearMipmapLinear:
        return Textures::Sampler::Filter::LinearMipmapLinear;
    default:
        throw std::runtime_error("Unknown Texture filter");
    }
}
static inline auto GetWrap(const TextureWrap& wrap) {
    switch (wrap)
    {
    case TextureWrap::ClampToEdge:
        return Textures::Sampler::Wrap::ClampToEdge;
    case TextureWrap::MirroredRepeat:
        return Textures::Sampler::Wrap::MirroredRepeat;
    case TextureWrap::Repeat:
        return Textures::Sampler::Wrap::Repeat;
    default:
        throw std::runtime_error("Unknown Texture Wrap mode");
    }
}

static inline Buffer::View::Type GetBufferViewType(const BufferViewType& type)
{
    switch (type) {
    case BufferViewType::Array:
        return Buffer::View::Type::Array;
    case BufferViewType::ElementArray:
        return Buffer::View::Type::ElementArray;
    default:
        return Buffer::View::Type::Unknown;
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
        return Buffer::Accessor::ComponentType::Int8;
    case (ComponentType::GLTFUByte):
        return Buffer::Accessor::ComponentType::Uint8;
    case (ComponentType::GLTFShort):
        return Buffer::Accessor::ComponentType::Int16;
    case (ComponentType::GLTFUShort):
        return Buffer::Accessor::ComponentType::Uint16;
    case (ComponentType::GLTFUInt):
        return Buffer::Accessor::ComponentType::Int32;
    case (ComponentType::GLTFFloat):
        return Buffer::Accessor::ComponentType::Float32;
    default:
        throw std::runtime_error("Unknown Accessor component type");
    }
}
static inline auto GetGeometryDrawingMode(DrawingMode mode)
{
    switch (mode) {
    case DrawingMode::Points:
        return Shapes::Geometry::DrawingMode::Points;
    case DrawingMode::Lines:
        return Shapes::Geometry::DrawingMode::Lines;
    case DrawingMode::LineLoop:
        return Shapes::Geometry::DrawingMode::LineLoop;
    case DrawingMode::LineStrip:
        return Shapes::Geometry::DrawingMode::LineStrip;
    case DrawingMode::Triangles:
        return Shapes::Geometry::DrawingMode::Triangles;
    case DrawingMode::TriangleStrip:
        return Shapes::Geometry::DrawingMode::TriangleStrip;
    case DrawingMode::TriangleFan:
        return Shapes::Geometry::DrawingMode::TriangleFan;
    default:
        return Shapes::Geometry::DrawingMode::Unknown;
    }
}
}

using namespace TabGraph;

void ParseGLTF(std::shared_ptr<Assets::Asset>);

auto GLTFMimeExtension {
    Assets::Parser::AddMimeExtension("model/gltf+json", ".gltf")
};

auto GLTFMimesParsers {
    Assets::Parser::Add("model/gltf+json", ParseGLTF)
};

Assets::Uri CreateUri(const std::filesystem::path& parentPath, const std::string& dataPath)
{
    auto bufferPath { std::filesystem::path(dataPath) };
    if (bufferPath.string().rfind("data:", 0) == 0)
        return Assets::Uri(bufferPath.string());
    else {
        if (!bufferPath.is_absolute())
            bufferPath = parentPath / bufferPath;
        return Assets::Uri(bufferPath);
    }
}

static inline auto ParseCameras(const rapidjson::Document& document)
{
    std::vector<std::shared_ptr<Cameras::Camera>> cameraVector;
    if (!document.HasMember("cameras")) return cameraVector;
    auto cameraIndex(0);
    for (const auto& camera : document["cameras"].GetArray()) {
        auto newCamera(std::make_shared<Cameras::Camera>("Camera" + std::to_string(cameraIndex)));
        if (std::string(camera["type"].GetString()) == "perspective") {
            auto perspective(camera["perspective"].GetObject());
            if (perspective.HasMember("zfar")) {
                Cameras::Projection::Perspective projection;
                projection.zfar = perspective["zfar"].GetFloat();
                if (perspective.HasMember("znear"))
                    projection.znear = perspective["znear"].GetFloat();
                if (perspective.HasMember("yfov"))
                    projection.fov = glm::degrees(perspective["yfov"].GetFloat());
                newCamera->SetProjection(projection);
            }
            else {
                Cameras::Projection::PerspectiveInfinite projection;
                if (perspective.HasMember("znear"))
                    projection.znear = perspective["znear"].GetFloat();
                if (perspective.HasMember("yfov"))
                    projection.fov = glm::degrees(perspective["yfov"].GetFloat());
                newCamera->SetProjection(projection);
            }
        }
        else if (std::string(camera["type"].GetString()) == "orthographic") {
            Cameras::Projection::Orthographic projection;
            newCamera->SetProjection(projection);
        }
        cameraVector.push_back(newCamera);
        cameraIndex++;
    }
    return cameraVector;
}

static inline auto ParseTextureSamplers(const rapidjson::Document& document)
{
    std::vector<std::shared_ptr<Textures::Sampler>> samplerVector;
    if (!document.HasMember("samplers")) return samplerVector;
    for (const auto& sampler : document["samplers"].GetArray()) {
        auto newSampler{ std::make_shared<Textures::Sampler>() };
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

static inline auto ParseTextures(const rapidjson::Document& document, std::vector<std::shared_ptr<Assets::Asset>>& images, std::shared_ptr<Assets::Asset> container)
{
    debugLog("Start parsing textures");
    std::vector<std::shared_ptr<Textures::Texture2D>> textureVector;
    if (!document.HasMember("textures")) return textureVector;
    auto samplers = ParseTextureSamplers(document);
    auto textureIndex(0);
    for (const auto& textureValue : document["textures"].GetArray()) {
        std::shared_ptr<Textures::Texture2D> texture = nullptr;
        if (textureValue.HasMember("source")) {
            auto source(textureValue["source"].GetInt());
            auto image { images.at(source) };
            texture = std::make_shared<Textures::Texture2D>(image);
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

static inline auto ParseMaterialExtensions(const std::vector<std::shared_ptr<Textures::Texture2D>>& textures, const rapidjson::Value& materialValue, std::shared_ptr<Material::Standard> material)
{
    if (!materialValue.HasMember("extensions")) return;
    for (const auto& extension : materialValue["extensions"].GetObject()) {
        if (std::string(extension.name.GetString()) == "KHR_materials_pbrSpecularGlossiness") {
            const auto& pbrSpecularGlossiness = extension.value;
            auto materialExtension = std::make_shared<Material::Extensions::SpecularGlossiness>();
            material->AddExtension(materialExtension);
            if (pbrSpecularGlossiness.HasMember("diffuseFactor")) {
                auto diffuseFactor(pbrSpecularGlossiness["diffuseFactor"].GetArray());
                materialExtension->SetDiffuse(glm::vec3(
                    diffuseFactor[0].GetFloat(),
                    diffuseFactor[1].GetFloat(),
                    diffuseFactor[2].GetFloat()));
                materialExtension->SetOpacity(diffuseFactor[3].GetFloat());
            }
            if (pbrSpecularGlossiness.HasMember("specularFactor")) {
                auto diffuseFactor(pbrSpecularGlossiness["specularFactor"].GetArray());
                materialExtension->SetSpecular(glm::vec3(
                    diffuseFactor[0].GetFloat(),
                    diffuseFactor[1].GetFloat(),
                    diffuseFactor[2].GetFloat()));
            }
            if (pbrSpecularGlossiness.HasMember("glossinessFactor")) {
                auto glossinessFactor(pbrSpecularGlossiness["glossinessFactor"].GetFloat());
                materialExtension->SetGlossiness(glossinessFactor);
            }
            if (pbrSpecularGlossiness.HasMember("diffuseTexture")) {
                auto textureObject(pbrSpecularGlossiness["diffuseTexture"].GetObject());
                materialExtension->SetTextureDiffuse(textures.at(textureObject["index"].GetInt()));
            }
            if (pbrSpecularGlossiness.HasMember("specularGlossinessTexture")) {
                auto textureObject(pbrSpecularGlossiness["specularGlossinessTexture"].GetObject());
                materialExtension->SetTextureSpecularGlossiness(textures.at(textureObject["index"].GetInt()));
            }
        }
    }
}

static inline auto ParseMaterials(const rapidjson::Document& document, std::vector<std::shared_ptr<Textures::Texture2D>>& textures)
{
    debugLog("Start parsing materials");
    //auto textureVector = ParseTextures(path, document);
    std::vector<std::shared_ptr<Material::Standard>> materialVector;
    if (!document.HasMember("materials")) return materialVector;
    auto materialIndex(0);
    for (const auto& materialValue : document["materials"].GetArray()) {
        auto material(std::make_shared<Material::Standard>("Material " + std::to_string(materialIndex)));
        if (materialValue.HasMember("name"))
            material->SetName(materialValue["name"].GetString());
        if (materialValue.HasMember("alphaCutoff"))
            material->SetOpacityCutoff(materialValue["alphaCutoff"].GetFloat());
        if (materialValue.HasMember("alphaMode")) {
            std::string alphaMode = materialValue["alphaMode"].GetString();
            if (alphaMode == "Opaque")
                material->SetOpacityMode(Material::OpacityMode::Opaque);
            if (alphaMode == "MASK")
                material->SetOpacityMode(Material::OpacityMode::Mask);
            if (alphaMode == "BLEND")
                material->SetOpacityMode(Material::OpacityMode::Blend);
        }

        if (materialValue.HasMember("doubleSided"))
            material->SetDoubleSided(materialValue["doubleSided"].GetBool());
        if (materialValue.HasMember("emissiveFactor")) {
            auto emissiveFactor(materialValue["emissiveFactor"].GetArray());
            material->SetEmissive(glm::vec3(emissiveFactor[0].GetFloat(),
                emissiveFactor[1].GetFloat(),
                emissiveFactor[2].GetFloat()));
        }
        if (materialValue.HasMember("normalTexture")) {
            auto textureObject(materialValue["normalTexture"].GetObject());
            material->SetTextureNormal(textures.at(textureObject["index"].GetInt()));
        }
        if (materialValue.HasMember("emissiveTexture")) {
            auto textureObject(materialValue["emissiveTexture"].GetObject());
            material->SetTextureEmissive(textures.at(textureObject["index"].GetInt()));
        }
        if (materialValue.HasMember("occlusionTexture")) {
            auto textureObject(materialValue["occlusionTexture"].GetObject());
            material->SetTextureAO(textures.at(textureObject["index"].GetInt()));
        }
        if (materialValue.HasMember("pbrMetallicRoughness")) {
            auto pbrMetallicRoughness(materialValue["pbrMetallicRoughness"].GetObject());
            auto materialExtension = std::make_shared<Material::Extensions::MetallicRoughness>();
            material->AddExtension(materialExtension);
            materialExtension->SetRoughness(1);
            materialExtension->SetMetallic(1);
            if (pbrMetallicRoughness.HasMember("metallicRoughnessTexture")) {
                auto textureObject(pbrMetallicRoughness["metallicRoughnessTexture"].GetObject());
                materialExtension->SetTextureMetallicRoughness(textures.at(textureObject["index"].GetInt()));
            }
            if (pbrMetallicRoughness.HasMember("baseColorFactor")) {
                auto baseColor(pbrMetallicRoughness["baseColorFactor"].GetArray());
                materialExtension->SetBaseColor(glm::vec3(baseColor[0].GetFloat(),
                    baseColor[1].GetFloat(),
                    baseColor[2].GetFloat()));
                materialExtension->SetOpacity(baseColor[3].GetFloat());
            }
            if (pbrMetallicRoughness.HasMember("metallicFactor"))
                materialExtension->SetMetallic(pbrMetallicRoughness["metallicFactor"].GetFloat());
            if (pbrMetallicRoughness.HasMember("roughnessFactor"))
                materialExtension->SetRoughness(pbrMetallicRoughness["roughnessFactor"].GetFloat());
            if (pbrMetallicRoughness.HasMember("baseColorTexture")) {
                auto textureObject(pbrMetallicRoughness["baseColorTexture"].GetObject());
                materialExtension->SetTextureBaseColor(textures.at(textureObject["index"].GetInt()));
            }
        }
        ParseMaterialExtensions(textures, materialValue, material);
        materialVector.push_back(material);
        materialIndex++;
    }
    debugLog("Done parsing materials");
    return materialVector;
}

static inline auto ParseBuffers(const std::filesystem::path path, const rapidjson::Document& document, std::shared_ptr<Assets::Asset> container)
{
    debugLog("Start parsing buffers");
    std::vector<std::shared_ptr<Assets::Asset>> bufferVector;
    if (!document.HasMember("buffers")) return bufferVector;
    for (const auto& bufferValue : document["buffers"].GetArray()) {
        auto asset { std::make_shared<Assets::Asset>() };
        asset->parsingOptions = container->parsingOptions;
        auto uri = CreateUri(path.parent_path(), bufferValue["uri"].GetString());
        asset->SetUri(uri);
        if (bufferValue.HasMember("name"))
            asset->SetName(bufferValue["name"].GetString());
        bufferVector.push_back(asset);
    }
    debugLog("Done parsing buffers");
    return bufferVector;
}

static inline auto ParseBufferViews(const rapidjson::Document& document, std::vector<std::shared_ptr<Assets::Asset>> buffers)
{
    debugLog("Start parsing bufferViews");
    std::vector<std::shared_ptr<Buffer::View>> bufferViewVector;
    if (!document.HasMember("bufferViews")) return bufferViewVector;
    for (const auto& bufferViewValue : document["bufferViews"].GetArray()) {
        auto bufferView(std::make_shared<Buffer::View>(
            bufferViewValue["byteLength"].GetInt(),
            buffers.at(bufferViewValue["buffer"].GetInt())));
        if (bufferViewValue.HasMember("name"))
            bufferView->SetName(bufferViewValue["name"].GetString());
        if (bufferViewValue.HasMember("byteOffset"))
            bufferView->SetByteOffset(bufferViewValue["byteOffset"].GetInt());
        if (bufferViewValue.HasMember("byteStride"))
            bufferView->SetByteStride(bufferViewValue["byteStride"].GetInt());
        if (bufferViewValue.HasMember("target"))
            bufferView->SetType(GLTF::GetBufferViewType(GLTF::BufferViewType(bufferViewValue["target"].GetInt())));
        bufferViewVector.push_back(bufferView);
    }
    debugLog("Done parsing bufferViews");
    return bufferViewVector;
}

static inline auto ParseBufferAccessors(const rapidjson::Document& document, std::vector<std::shared_ptr<Buffer::View>>& bufferViews)
{
    debugLog("Start parsing bufferAccessors");
    std::vector<Buffer::Accessor> bufferAccessorVector;
    if (!document.HasMember("accessors"))
        return bufferAccessorVector;
    auto bufferAccessorIndex(0);
    for (const auto& bufferAccessorValue : document["accessors"].GetArray()) {
        size_t byteOffset{ 0 };
        std::shared_ptr<Buffer::View> bufferView;
        if (bufferAccessorValue.HasMember("bufferView"))
            bufferView = bufferViews.at(bufferAccessorValue["bufferView"].GetInt());
        if (bufferAccessorValue.HasMember("byteOffset"))
            byteOffset = bufferAccessorValue["byteOffset"].GetInt();
        Buffer::Accessor bufferAccessor{
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

static inline auto ParseMeshes(const rapidjson::Document& document, const std::vector<std::shared_ptr<Material::Standard>>& materials, const std::vector<Buffer::Accessor>& bufferAccessors)
{
    debugLog("Start parsing meshes");
    std::vector<std::shared_ptr<Shapes::Mesh>> meshVector;
    auto meshesItr(document.FindMember("meshes"));
    if (meshesItr == document.MemberEnd()) {
        debugLog("No meshes found");
        return meshVector;
    }
    auto defaultMaterial(std::make_shared<Material::Standard>("defaultMaterial"));
    for (const auto& mesh : meshesItr->value.GetArray()) {
        debugLog("Found new mesh");
        auto currentMesh(std::make_shared<Shapes::Mesh>());
        if (mesh.HasMember("name"))
            currentMesh->SetName(mesh["name"].GetString());
        if (mesh.HasMember("primitives")) {
            for (const auto& primitive : mesh["primitives"].GetArray()) {
                debugLog("Found new primitive");
                auto geometry(std::make_shared<Shapes::Geometry>());
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
                        auto accessorKey(Shapes::Geometry::GetAccessorKey(attributeName));
                        if (accessorKey == Shapes::Geometry::AccessorKey::Invalid) {
                            debugLog("Invalid Accessor Key : " + attributeName);
                        }
                        else {
                            geometry->SetAccessor(accessorKey, accessor);
                            accessor->GetBufferView()->SetType(Buffer::View::Type::Array);
                        }*/
                    }
                }
                if (primitive.HasMember("indices")) {
                    auto &accessor(bufferAccessors.at(primitive["indices"].GetInt()));
                    geometry->SetIndices(accessor);
                    accessor.GetBufferView()->SetType(Buffer::View::Type::ElementArray);
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

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/string_cast.hpp>

static inline auto ParseNodes(const rapidjson::Document& document)
{
    std::vector<std::shared_ptr<Nodes::Node>> nodeVector;
    auto nodeItr(document.FindMember("nodes"));
    if (nodeItr == document.MemberEnd())
        return nodeVector;
    int nodeIndex = 0;
    for (const auto& node : nodeItr->value.GetArray()) {
        std::shared_ptr<Nodes::Node> newNode;
        if (node.HasMember("children") || node.HasMember("camera") || node.HasMember("mesh"))
            newNode = std::make_shared<Nodes::Group>("NodeGroup_" + std::to_string(nodeIndex));
        else
            newNode = std::make_shared<Nodes::Node>("Node_" + std::to_string(nodeIndex));
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
auto ConvertTo(const Buffer::Accessor& accessor) {
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
static inline auto GenerateAnimationChannel(Animations::Interpolation interpolation, const Buffer::Accessor& keyFramesValues, const Buffer::Accessor& timings) {
    Animations::Channel<T> newChannel;
    if (interpolation == Animations::Interpolation::CubicSpline) {
        for (auto i = 0u; i < keyFramesValues.GetSize(); i += 3) {
            Animations::Channel<T>::KeyFrame keyFrame;
            keyFrame.inputTangent = keyFramesValues.at<glm::vec3>(static_cast<size_t>(i) + 0);
            keyFrame.value = keyFramesValues.at<glm::vec3>(static_cast<size_t>(i) + 1);
            keyFrame.outputTangent = keyFramesValues.at<glm::vec3>(static_cast<size_t>(i) + 2);
            keyFrame.time = timings.at<float>(i / 3);
            newChannel.InsertKeyFrame(keyFrame);
        }
    }
    else {
        for (auto i = 0u; i < keyFramesValues.GetSize(); ++i) {
            Animations::Channel<T>::KeyFrame keyFrame;
            keyFrame.value = keyFramesValues.at<glm::vec3>(i);
            keyFrame.time = timings.at<float>(i);
            newChannel.InsertKeyFrame(keyFrame);
        }
    }
    return newChannel;
}

static inline auto ParseAnimations(const rapidjson::Document& document, const std::vector<std::shared_ptr<Nodes::Node>>& nodes, const std::vector<Buffer::Accessor>& bufferAccessors)
{
    std::vector<std::shared_ptr<Animations::Animation>> animations;
    if (!document.HasMember("animations"))
        return animations;
    for (const auto& animation : document["animations"].GetArray()) {
        auto newAnimation(std::make_shared<Animations::Animation>());
        if (animation.HasMember("name"))
            newAnimation->SetName(animation["name"].GetString());
        for (const auto& channel : animation["channels"].GetArray()) {
            auto &sampler{ animation["samplers"].GetArray()[channel["sampler"].GetInt()] };
            Animations::Interpolation channelInterpolation{ Animations::Interpolation::Linear };
            if (sampler.HasMember("interpolation")) {
                std::string interpolation(sampler["interpolation"].GetString());
                if (interpolation == "LINEAR")
                    channelInterpolation = Animations::Interpolation::Linear;
                else if (interpolation == "STEP")
                    channelInterpolation = Animations::Interpolation::Step;
                else if (interpolation == "CUBICSPLINE")
                    channelInterpolation = Animations::Interpolation::CubicSpline;
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

static inline auto ParseSkins(const rapidjson::Document& document, const std::vector<std::shared_ptr<Nodes::Node>>& nodes, const std::vector<Buffer::Accessor>& bufferAccessors)
{
    debugLog("Start parsing Skins");
    std::vector<std::shared_ptr<Shapes::Mesh::Skin>> skins;
    if (!document.HasMember("skins"))
        return skins;
    for (const auto& skin : document["skins"].GetArray()) {
        auto newSkin(std::make_shared<Shapes::Mesh::Skin>());
        if (skin.HasMember("name"))
            newSkin->SetName(skin["name"].GetString());
        if (skin.HasMember("inverseBindMatrices")) {
            auto accessor{ bufferAccessors.at(skin["inverseBindMatrices"].GetInt()) };
            accessor.GetBufferView()->SetStorage(Buffer::View::Storage::CPU);
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
    const std::vector<std::shared_ptr<Nodes::Node>>& nodes,
    const std::vector<std::shared_ptr<Shapes::Mesh>>& meshes,
    const std::vector<std::shared_ptr<Shapes::Mesh::Skin>>& meshSkins,
    const std::vector<std::shared_ptr<Cameras::Camera>>& cameras)
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
            auto renderable{ std::make_shared<Nodes::Renderable>() };
            if (gltfNode.HasMember("skin"))
                mesh->SetSkin(meshSkins.at(gltfNode["skin"].GetInt()));
            renderable->Add(mesh);
            renderable->SetParent(std::static_pointer_cast<Nodes::Group>(node));            
        }
        if (gltfNode.HasMember("camera")) {
            auto &camera = cameras.at(gltfNode["camera"].GetInt());
            camera->SetParent(std::static_pointer_cast<Nodes::Group>(node));
        }
        if (gltfNode.HasMember("children")) {
            for (const auto& child : gltfNode["children"].GetArray()) {
                auto &childNode = nodes.at(child.GetInt());
                nodes.at(child.GetInt())->SetParent(std::static_pointer_cast<Nodes::Group>(node));
                std::cout << "Node parenting " << node->GetName() << " -> " << nodes.at(child.GetInt())->GetName() << std::endl;
            }
        }
        nodeIndex++;
    }
}

static inline auto ParseScenes(
    const rapidjson::Document& document,
    const std::vector<std::shared_ptr<Nodes::Node>>& nodes,
    const std::vector<std::shared_ptr<Animations::Animation>>& animations)
{
    std::vector<std::shared_ptr<Nodes::Scene>> sceneVector;
    auto scenes(document["scenes"].GetArray());
    int sceneIndex = 0;
    for (const auto& scene : scenes) {
        std::cout << "found scene" << std::endl;
        auto newScene(std::make_shared<Nodes::Scene>(std::to_string(sceneIndex)));
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

#include "Light/DirectionalLight.hpp"

static inline auto ParseLights(const rapidjson::Document& document)
{
    std::vector<std::shared_ptr<Lights::Light>> lightsVector;
    const auto& extensions(document.FindMember("extensions"));
    if (extensions != document.MemberEnd()) {
        const auto& KHR_lights_punctual = extensions->value.FindMember("KHR_lights_punctual");
        if (KHR_lights_punctual != extensions->value.MemberEnd()) {
            const auto& lights = KHR_lights_punctual->value.FindMember("lights");
            if (lights != KHR_lights_punctual->value.MemberEnd()) {
                for (const auto& light : lights->value.GetArray()) {
                    std::shared_ptr<Lights::Light> newLight;
                    if (light.HasMember("type")) {
                        auto type = light["type"].GetString();
                        if (type == "directional")
                            newLight = std::make_shared<Lights::DirectionalLight>();
                        lightsVector.emplace_back(newLight);
                    }
                }
            }
        }
    }
    return lightsVector;
}

static inline auto ParseImages(const std::filesystem::path path, const rapidjson::Document& document, std::vector<std::shared_ptr<Buffer::View>>& bufferViews, std::shared_ptr<Assets::Asset> container)
{
    std::vector<std::shared_ptr<Assets::Asset>> imagesVector;
    auto imagesItr(document.FindMember("images"));
    if (imagesItr == document.MemberEnd())
        return imagesVector;
    for (const auto& gltfImagee : imagesItr->value.GetArray()) {
        auto imageUriItr(gltfImagee.FindMember("uri"));
        if (imageUriItr == gltfImagee.MemberEnd()) {
            auto imageAsset { std::make_shared<Assets::Asset>() };
            auto imageBufferViewItr(gltfImagee.FindMember("bufferView"));
            imageAsset->parsingOptions = container->parsingOptions;
            if (imageBufferViewItr == gltfImagee.MemberEnd()) {
                imageAsset->assets.push_back(std::make_shared<Assets::Image>(glm::ivec2(1), Pixel::SizedFormat::Uint8_NormalizedRGB));
                imageAsset->SetLoaded(true);
            } else {
                imageAsset->SetUri(std::string("data:") + gltfImagee["mimeType"].GetString() + ",");
                auto bufferViewIndex { imageBufferViewItr->value.GetInt() };
                imageAsset->assets.push_back(bufferViews.at(bufferViewIndex));
                bufferViews.at(bufferViewIndex)->SetStorage(Buffer::View::Storage::CPU);
            }
            //TODO : learn how to use bufferView and mimeType
            imagesVector.push_back(imageAsset);
            continue;
        }
        auto uri = CreateUri(path.parent_path(), imageUriItr->value.GetString());
        auto imageAsset = std::make_shared<Assets::Asset>(uri);
        imageAsset->parsingOptions = container->parsingOptions;
        imagesVector.push_back(imageAsset);
    }
    return imagesVector;
}

void ParseGLTF(std::shared_ptr<Assets::Asset> container)
{
    auto path = container->GetUri().DecodePath();
    std::cout << path << std::endl;
    std::ifstream file(path);
    rapidjson::IStreamWrapper streamWrapper(file);
    rapidjson::Document document;
    rapidjson::ParseResult parseResult(document.ParseStream(streamWrapper));
    if (!parseResult) {
        debugLog("Invalid file !");
        return;
        //return container;
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
}
