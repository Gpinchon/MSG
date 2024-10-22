/*
 * @Author: gpinchon
 * @Date:   2020-08-07 18:36:53
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-02-11 16:17:13
 */
#include <Assets/Asset.hpp>
#include <Assets/Parser.hpp>

#include <SG/Component/Camera.hpp>
#include <SG/Component/Light/PunctualLight.hpp>
#include <SG/Component/Mesh.hpp>
#include <SG/Component/MeshSkin.hpp>
#include <SG/Core/Buffer/Accessor.hpp>
#include <SG/Core/Buffer/Buffer.hpp>
#include <SG/Core/Buffer/View.hpp>
#include <SG/Core/Image/Image2D.hpp>
#include <SG/Core/Material.hpp>
#include <SG/Core/Material/Extension/Base.hpp>
#include <SG/Core/Material/Extension/MetallicRoughness.hpp>
#include <SG/Core/Material/Extension/Sheen.hpp>
#include <SG/Core/Material/Extension/SpecularGlossiness.hpp>
#include <SG/Core/Material/Extension/Unlit.hpp>
#include <SG/Core/Material/TextureInfo.hpp>
#include <SG/Core/Primitive.hpp>
#include <SG/Core/PrimitiveOptimizer.hpp>
#include <SG/Core/Texture/Sampler.hpp>
#include <SG/Core/Texture/Texture.hpp>
#include <SG/Entity/Node.hpp>
#include <SG/Scene/Animation.hpp>
#include <SG/Scene/Animation/Channel.hpp>
#include <SG/Scene/Scene.hpp>

#include <Tools/Debug.hpp>
#include <Tools/ScopedTimer.hpp>
#include <Tools/ThreadPool.hpp>

#include <nlohmann/json.hpp>

#include <glm/ext.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/string_cast.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>

using json = nlohmann::json;

namespace TabGraph::Assets {
namespace GLTF {
    struct Dictionary {
        void Add(const std::string& a_TypeName, const std::shared_ptr<SG::Object> a_Object)
        {
            objects[a_TypeName].push_back(a_Object);
        }
        auto& Get(const std::string& a_TypeName)
        {
            return objects[a_TypeName];
        }
        template <typename T>
        auto Get(const std::string& a_TypeName, const size_t& a_Index) const
        {
            if (const auto& obj = objects.at(a_TypeName).at(a_Index); obj->IsCompatible(typeid(T)))
                return std::static_pointer_cast<T>(obj);
            throw std::runtime_error("Incompatible types");
        }
        std::shared_ptr<SG::Sampler> defaultSampler = std::make_shared<SG::Sampler>();
        Tools::SparseSet<SG::TextureSampler, 4096> textureSamplers;
        Tools::SparseSet<SG::Component::Mesh, 4096> meshes;
        Tools::SparseSet<SG::Component::MeshSkin, 4096> skins;
        Tools::SparseSet<SG::Component::Camera, 4096> cameras;
        Tools::SparseSet<SG::Component::PunctualLight, 4096> lights;
        Tools::SparseSet<SG::BufferAccessor, 8192> bufferAccessors;
        std::map<std::string, Tools::SparseSet<ECS::DefaultRegistry::EntityRefType, 4096>> entities;
        std::map<std::string, std::vector<std::shared_ptr<SG::Object>>> objects;
    };
    enum class ComponentType {
        GLTFByte   = 5120,
        GLTFUByte  = 5121,
        GLTFShort  = 5122,
        GLTFUShort = 5123,
        GLTFUInt   = 5125,
        GLTFFloat  = 5126,
        MaxValue
    };
    enum class TextureWrap {
        ClampToEdge    = 33071,
        MirroredRepeat = 33648,
        Repeat         = 10497
    };
    enum class TextureFilter {
        Nearest              = 9728,
        Linear               = 9729,
        NearestMipmapNearest = 9984,
        LinearMipmapNearest  = 9985,
        NearestMipmapLinear  = 9986,
        LinearMipmapLinear   = 9987
    };
    enum class BufferViewType {
        Array        = 34962,
        ElementArray = 34963
    };
    enum class DrawingMode {
        Points        = 0,
        Lines         = 1,
        LineLoop      = 2,
        LineStrip     = 3,
        Triangles     = 4,
        TriangleStrip = 5,
        TriangleFan   = 6
    };

    static inline auto GetFilter(TextureFilter filter)
    {
        switch (filter) {
        case TextureFilter::Nearest:
            return SG::Sampler::Filter::Nearest;
        case TextureFilter::Linear:
            return SG::Sampler::Filter::Linear;
        case TextureFilter::NearestMipmapNearest:
            return SG::Sampler::Filter::NearestMipmapNearest;
        case TextureFilter::LinearMipmapNearest:
            return SG::Sampler::Filter::LinearMipmapNearest;
        case TextureFilter::NearestMipmapLinear:
            return SG::Sampler::Filter::NearestMipmapLinear;
        case TextureFilter::LinearMipmapLinear:
            return SG::Sampler::Filter::LinearMipmapLinear;
        default:
            throw std::runtime_error("Unknown Texture filter");
        }
    }

    static inline auto GetWrap(const TextureWrap& wrap)
    {
        switch (wrap) {
        case TextureWrap::ClampToEdge:
            return SG::Sampler::Wrap::ClampToEdge;
        case TextureWrap::MirroredRepeat:
            return SG::Sampler::Wrap::MirroredRepeat;
        case TextureWrap::Repeat:
            return SG::Sampler::Wrap::Repeat;
        default:
            throw std::runtime_error("Unknown Texture Wrap mode");
        }
    }

    static inline auto GetAccessorComponentNbr(const std::string& a_type)
    {
        if (a_type == "SCALAR")
            return 1u;
        else if (a_type == "VEC2")
            return 2u;
        else if (a_type == "VEC3")
            return 3u;
        else if (a_type == "VEC4")
            return 4u;
        else if (a_type == "MAT2")
            return 4u;
        else if (a_type == "MAT3")
            return 9u;
        else if (a_type == "MAT4")
            return 16u;
        else
            throw std::runtime_error("Unknown Buffer Accessor type");
    }

    static inline auto GetAccessorComponentType(const ComponentType& a_componentType)
    {
        using enum SG::DataType;
        switch (a_componentType) {
        case ComponentType::GLTFByte:
            return Int8;
        case ComponentType::GLTFUByte:
            return Uint8;
        case ComponentType::GLTFShort:
            return Int16;
        case ComponentType::GLTFUShort:
            return Uint16;
        case ComponentType::GLTFUInt:
            return Uint32;
        case ComponentType::GLTFFloat:
            return Float32;
        default:
            return Unknown;
        }
    }

    static inline auto GetGeometryDrawingMode(DrawingMode mode)
    {
        using enum SG::Primitive::DrawingMode;
        switch (mode) {
        case DrawingMode::Points:
            return Points;
        case DrawingMode::Lines:
            return Lines;
        case DrawingMode::LineLoop:
            return LineLoop;
        case DrawingMode::LineStrip:
            return LineStrip;
        case DrawingMode::Triangles:
            return Triangles;
        case DrawingMode::TriangleStrip:
            return TriangleStrip;
        case DrawingMode::TriangleFan:
            return TriangleFan;
        default:
            return Unknown;
        }
    }

    Uri CreateUri(const std::filesystem::path& parentPath, const std::string& dataPath)
    {
        auto bufferPath { std::filesystem::path(dataPath) };
        if (bufferPath.string().rfind("data:", 0) == 0)
            return Uri(bufferPath.string());
        else {
            if (!bufferPath.is_absolute())
                bufferPath = parentPath / bufferPath;
            return Uri(bufferPath);
        }
    }

    template <typename T>
    constexpr T Parse(const json& a_Value, const std::string& a_Name, bool a_Optional = false, const T& a_Default = {})
    {
        if (a_Value.contains(a_Name))
            return a_Value[a_Name];
        else if (!a_Optional)
            throw std::runtime_error("Could not find value " + a_Name);
        return a_Default;
    }

    template <>
    std::string Parse(const json& a_Value, const std::string& a_Name, bool a_Optional, const std::string& a_Default)
    {
        if (a_Value.contains(a_Name))
            return a_Value[a_Name];
        else if (!a_Optional)
            throw std::runtime_error("Could not find value " + a_Name);
        return a_Default;
    }

    template <glm::length_t L, typename T, glm::qualifier Q>
    glm::vec<L, T, Q> ParseVec(const json& a_Value, const std::string& a_Name, bool a_Optional, const glm::vec<L, T, Q>& a_Default)
    {
        if (a_Value.contains(a_Name)) {
            glm::vec<L, T, Q> ret;
            auto vector = a_Value[a_Name].get<std::vector<T>>();
            for (unsigned i = 0; i < L; ++i)
                ret[i] = vector[i];
            return ret;
        } else if (!a_Optional)
            throw std::runtime_error("Could not find value " + a_Name);
        return a_Default;
    }

    template <>
    glm::vec3 Parse(const json& a_Value, const std::string& a_Name, bool a_Optional, const glm::vec3& a_Default)
    {
        return ParseVec(a_Value, a_Name, a_Optional, a_Default);
    }

    template <>
    glm::vec4 Parse(const json& a_Value, const std::string& a_Name, bool a_Optional, const glm::vec4& a_Default)
    {
        return ParseVec(a_Value, a_Name, a_Optional, a_Default);
    }

    template <>
    glm::quat Parse(const json& a_Value, const std::string& a_Name, bool a_Optional, const glm::quat& a_Default)
    {
        if (a_Value.contains(a_Name)) {
            auto vector = a_Value[a_Name].get<std::vector<float>>();
            // We HAVE to do it this way because of GLM strange order for quaternions
            glm::quat quat;
            quat[0] = vector[0];
            quat[1] = vector[1];
            quat[2] = vector[2];
            quat[3] = vector[3];
            return quat;
        } else if (!a_Optional)
            throw std::runtime_error("Could not find value " + a_Name);
        return a_Default;
    }
}

struct ImageSampleFunctorI {
    virtual ~ImageSampleFunctorI()                       = default;
    virtual glm::vec4 operator()(const glm::vec3& a_UVW) = 0;
};

struct ImageSampleFunctor : ImageSampleFunctorI {
    ImageSampleFunctor(const std::shared_ptr<SG::Image>& a_Image)
        : image(a_Image)
    {
    }
    virtual glm::vec4 operator()(const glm::vec3& a_UVW) override
    {
        return image->LoadNorm(a_UVW);
    }
    const std::shared_ptr<SG::Image> image;
};

struct ConstImageSampleFunctor : ImageSampleFunctorI {
    ConstImageSampleFunctor(const glm::vec4& a_Value)
        : value(a_Value)
    {
    }
    virtual glm::vec4 operator()(const glm::vec3&) override
    {
        return value;
    }
    const glm::vec4 value;
};

static inline void ParseCameras(const json& document, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!document.contains("cameras"))
        return;
#ifndef NDEBUG
    auto timer = Tools::ScopedTimer("Parsing cameras");
#endif
    size_t cameraIndex = 0;
    for (const auto& gltfCamera : document["cameras"]) {
        SG::Component::Camera camera;
        if (gltfCamera["type"] == "perspective") {
            if (gltfCamera["perspective"].contains("zfar")) {
                SG::Component::Projection::Perspective projection;
                projection.zfar   = GLTF::Parse(gltfCamera["perspective"], "zfar", false, projection.zfar);
                projection.znear  = GLTF::Parse(gltfCamera["perspective"], "znear", true, projection.znear);
                projection.fov    = GLTF::Parse(gltfCamera["perspective"], "fov", true, projection.fov);
                camera.projection = projection;
            } else {
                SG::Component::Projection::PerspectiveInfinite projection;
                projection.znear  = GLTF::Parse(gltfCamera["perspective"], "znear", true, projection.znear);
                projection.fov    = glm::degrees(GLTF::Parse(gltfCamera["perspective"], "yfov", true, glm::radians(projection.fov)));
                camera.projection = projection;
            }
        } else if (gltfCamera["type"] == "orthographic") {
            SG::Component::Projection::Orthographic projection;
            camera.projection = projection;
        }
        camera.name = GLTF::Parse(gltfCamera, "name", true, std::string(camera.name));
        a_Dictionary.cameras.insert(cameraIndex, camera);
        ++cameraIndex;
    }
}

static inline void ParseSamplers(const json& document, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!document.contains("samplers"))
        return;
#ifndef NDEBUG
    auto timer = Tools::ScopedTimer("Parsing samplers");
#endif
    for (const auto& gltfSampler : document["samplers"]) {
        auto sampler   = std::make_shared<SG::Sampler>();
        auto magFilter = GLTF::TextureFilter(GLTF::Parse(gltfSampler, "magFilter", true, int(GLTF::TextureFilter::Linear)));
        auto minFilter = GLTF::TextureFilter(GLTF::Parse(gltfSampler, "minFilter", true, int(GLTF::TextureFilter::Linear)));
        auto wrapS     = GLTF::TextureWrap(GLTF::Parse(gltfSampler, "wrapS", true, int(GLTF::TextureWrap::Repeat)));
        auto wrapT     = GLTF::TextureWrap(GLTF::Parse(gltfSampler, "wrapT", true, int(GLTF::TextureWrap::Repeat)));
        auto wrapR     = GLTF::TextureWrap(GLTF::Parse(gltfSampler, "wrapR", true, int(GLTF::TextureWrap::Repeat)));
        sampler->SetMagFilter(GLTF::GetFilter(magFilter));
        sampler->SetMinFilter(GLTF::GetFilter(minFilter));
        sampler->SetWrapS(GLTF::GetWrap(wrapS));
        sampler->SetWrapT(GLTF::GetWrap(wrapT));
        sampler->SetWrapR(GLTF::GetWrap(wrapR));
        a_Dictionary.Add("samplers", sampler);
    }
}

static inline void ParseTextureSamplers(const json& document, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!document.contains("textures"))
        return;
#ifndef NDEBUG
    auto timer = Tools::ScopedTimer("Parsing textures");
#endif
    uint32_t textureSamplerIndex = 0;
    for (const auto& textureValue : document["textures"]) {
        auto textureSampler = SG::TextureSampler();
        const auto source   = GLTF::Parse(textureValue, "source", true, -1);
        const auto sampler  = GLTF::Parse(textureValue, "sampler", true, -1);
        if (source > -1)
            textureSampler.texture = a_Dictionary.Get<SG::Texture>("images", source);
        if (sampler > -1)
            textureSampler.sampler = a_Dictionary.Get<SG::Sampler>("samplers", sampler);
        else
            textureSampler.sampler = a_Dictionary.defaultSampler;
        textureSampler.texture->SetCompressed(a_AssetsContainer->parsingOptions.texture.compress);
        textureSampler.texture->SetCompressionQuality(a_AssetsContainer->parsingOptions.texture.compressionQuality);
        a_Dictionary.textureSamplers.insert(textureSamplerIndex, textureSampler);
        textureSamplerIndex++;
    }
}

static inline auto ParseTextureInfo(GLTF::Dictionary& a_Dictionary, const json& a_JSON)
{
    SG::TextureInfo texture;
    texture.textureSampler = a_Dictionary.textureSamplers.at(GLTF::Parse<int>(a_JSON, "index"));
    texture.texCoord       = GLTF::Parse(a_JSON, "texCoord", true, texture.texCoord);
    return texture;
}

static inline auto ParseSpecularGlossiness(GLTF::Dictionary& a_Dictionary, const json& extension)
{
    SG::SpecularGlossinessExtension specGloss;
    specGloss.diffuseFactor    = GLTF::Parse(extension, "diffuseFactor", true, specGloss.diffuseFactor);
    specGloss.specularFactor   = GLTF::Parse(extension, "specularFactor", true, specGloss.specularFactor);
    specGloss.glossinessFactor = GLTF::Parse(extension, "glossinessFactor", true, specGloss.glossinessFactor);
    if (extension.contains("diffuseTexture")) {
        specGloss.diffuseTexture = ParseTextureInfo(a_Dictionary, extension["diffuseTexture"]);
    }
    if (extension.contains("specularGlossinessTexture")) {
        specGloss.specularGlossinessTexture = ParseTextureInfo(a_Dictionary, extension["specularGlossinessTexture"]);
    }
    return specGloss;
}

static inline auto ParseSheen(GLTF::Dictionary& a_Dictionary, const json& a_Extension)
{
    SG::SheenExtension sheen;
    sheen.colorFactor     = GLTF::Parse(a_Extension, "sheenColorFactor", true, sheen.colorFactor);
    sheen.roughnessFactor = GLTF::Parse(a_Extension, "sheenRoughnessFactor", true, sheen.roughnessFactor);
    if (a_Extension.contains("sheenColorTexture")) {
        sheen.colorTexture = ParseTextureInfo(a_Dictionary, a_Extension["sheenColorTexture"]);
    }
    if (a_Extension.contains("sheenRoughnessTexture")) {
        sheen.roughnessTexture = ParseTextureInfo(a_Dictionary, a_Extension["sheenRoughnessTexture"]);
    }
    return sheen;
}

static inline void ParseMaterialExtensions(GLTF::Dictionary& a_Dictionary, const json& a_Extensions, std::shared_ptr<SG::Material> a_Material)
{
    if (a_Extensions.contains("KHR_materials_pbrSpecularGlossiness"))
        a_Material->AddExtension(ParseSpecularGlossiness(a_Dictionary, a_Extensions["KHR_materials_pbrSpecularGlossiness"]));
    if (a_Extensions.contains("KHR_materials_sheen"))
        a_Material->AddExtension(ParseSheen(a_Dictionary, a_Extensions["KHR_materials_sheen"]));
    if (a_Extensions.contains("KHR_materials_unlit"))
        a_Material->AddExtension(SG::UnlitExtension {});
}

static inline auto ParseMetallicRoughness(GLTF::Dictionary& a_Dictionary, const json& a_Extension)
{
    SG::MetallicRoughnessExtension mra {};
    mra.colorFactor     = GLTF::Parse(a_Extension, "baseColorFactor", true, mra.colorFactor);
    mra.metallicFactor  = GLTF::Parse(a_Extension, "metallicFactor", true, mra.metallicFactor);
    mra.roughnessFactor = GLTF::Parse(a_Extension, "roughnessFactor", true, mra.roughnessFactor);
    if (a_Extension.contains("baseColorTexture"))
        mra.colorTexture = ParseTextureInfo(a_Dictionary, a_Extension["baseColorTexture"]);
    if (a_Extension.contains("metallicRoughnessTexture"))
        mra.metallicRoughnessTexture = ParseTextureInfo(a_Dictionary, a_Extension["metallicRoughnessTexture"]);
    return mra;
}

static inline auto ParseBaseExtension(GLTF::Dictionary& a_Dictionary, const json& a_Extension)
{
    SG::BaseExtension base;
    base.alphaCutoff    = GLTF::Parse(a_Extension, "alphaCutoff", true, base.alphaCutoff);
    base.doubleSided    = GLTF::Parse(a_Extension, "doubleSided", true, base.doubleSided);
    base.emissiveFactor = GLTF::Parse(a_Extension, "emissiveFactor", true, base.emissiveFactor);
    auto alphaMode      = GLTF::Parse<std::string>(a_Extension, "alphaMode", true, "OPAQUE");
    if (alphaMode == "OPAQUE")
        base.alphaMode = SG::BaseExtension::AlphaMode::Opaque;
    else if (alphaMode == "MASK")
        base.alphaMode = SG::BaseExtension::AlphaMode::Mask;
    else if (alphaMode == "BLEND")
        base.alphaMode = SG::BaseExtension::AlphaMode::Blend;
    if (a_Extension.contains("normalTexture")) {
        const auto& texInfo           = a_Extension["normalTexture"];
        SG::NormalTextureInfo texture = ParseTextureInfo(a_Dictionary, a_Extension["normalTexture"]);
        texture.scale                 = GLTF::Parse(texInfo, "scale", true, texture.scale);
        base.normalTexture            = texture;
    }
    if (a_Extension.contains("emissiveTexture"))
        base.emissiveTexture = ParseTextureInfo(a_Dictionary, a_Extension["emissiveTexture"]);
    if (a_Extension.contains("occlusionTexture")) {
        SG::OcclusionTextureInfo texture = ParseTextureInfo(a_Dictionary, a_Extension["occlusionTexture"]);
        texture.strength                 = GLTF::Parse(a_Extension["occlusionTexture"], "strength", true, texture.strength);
        base.occlusionTexture            = texture;
    }
    return base;
}

static inline void ParseMaterials(const json& document, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!document.contains("materials"))
        return;
#ifndef NDEBUG
    auto timer = Tools::ScopedTimer("Parsing materials");
#endif
    for (const auto& materialValue : document["materials"]) {
        auto material = std::make_shared<SG::Material>();
        material->SetName(GLTF::Parse(materialValue, "name", true, std::string(material->GetName())));
        material->AddExtension(ParseBaseExtension(a_Dictionary, materialValue));
        if (materialValue.contains("pbrMetallicRoughness"))
            material->AddExtension(ParseMetallicRoughness(a_Dictionary, materialValue["pbrMetallicRoughness"]));
        if (materialValue.contains("extensions"))
            ParseMaterialExtensions(a_Dictionary, materialValue["extensions"], material);
        a_Dictionary.Add("materials", material);
    }
}

static inline void ParseBuffers(const std::filesystem::path& path, const json& document, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!document.contains("buffers"))
        return;
#ifndef NDEBUG
    auto timer = Tools::ScopedTimer("Parsing buffers");
#endif
    std::vector<std::shared_ptr<Asset>> assetVector;
    for (const auto& bufferValue : document["buffers"]) {
        auto asset { std::make_shared<Asset>() };
        asset->parsingOptions = a_AssetsContainer->parsingOptions;
        asset->SetUri(GLTF::CreateUri(path.parent_path(), bufferValue["uri"]));
        asset->SetName(GLTF::Parse(bufferValue, "name", true, std::string(asset->GetName())));
        assetVector.push_back(asset);
    }
    std::vector<std::future<std::shared_ptr<Asset>>> parsingFuture;
    for (const auto& asset : assetVector)
        parsingFuture.push_back(Parser::AddParsingTask(asset));
    for (auto& future : parsingFuture) {
        std::shared_ptr<SG::Buffer> buffer = future.get()->Get<SG::Buffer>().front();
        a_Dictionary.Add("buffers", buffer);
    }
}

static inline void ParseBufferViews(const json& document, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>&)
{
    if (!document.contains("bufferViews"))
        return;
#ifndef NDEBUG
    auto timer = Tools::ScopedTimer("Parsing bufferViews");
#endif
    for (const auto& bufferViewValue : document["bufferViews"]) {
        auto bufferView(std::make_shared<SG::BufferView>());
        const auto buffer = a_Dictionary.Get<SG::Buffer>("buffers", bufferViewValue["buffer"]);
        bufferView->SetBuffer(buffer);
        bufferView->SetByteLength(bufferViewValue["byteLength"]);
        bufferView->SetName(GLTF::Parse(bufferViewValue, "name", true, std::string(bufferView->GetName())));
        bufferView->SetByteOffset(GLTF::Parse(bufferViewValue, "byteOffset", true, bufferView->GetByteOffset()));
        bufferView->SetByteStride(GLTF::Parse(bufferViewValue, "byteStride", true, bufferView->GetByteStride()));
        // if (bufferViewValue.contains("target"))
        //     bufferView->SetType(GLTF::GetBufferViewType(GLTF::BufferViewType(bufferViewValue["target"].GetInt())));
        a_Dictionary.Add("bufferViews", bufferView);
    }
}

static inline void ParseBufferAccessors(const json& a_JSON, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>&)
{
    if (!a_JSON.contains("accessors"))
        return;
#ifndef NDEBUG
    auto timer = Tools::ScopedTimer("Parsing accessors");
#endif
    size_t accessorIndex = 0;
    for (const auto& gltfbufferAccessor : a_JSON["accessors"]) {
        SG::BufferAccessor bufferAccessor;
        std::shared_ptr<SG::BufferView> bufferView;
        bufferAccessor.SetName(GLTF::Parse(gltfbufferAccessor, "name", true, std::string(bufferAccessor.GetName())));
        bufferAccessor.SetSize(GLTF::Parse<size_t>(gltfbufferAccessor, "count"));
        bufferAccessor.SetComponentNbr(GLTF::GetAccessorComponentNbr(GLTF::Parse<std::string>(gltfbufferAccessor, "type")));
        bufferAccessor.SetComponentType(GLTF::GetAccessorComponentType(GLTF::ComponentType(GLTF::Parse<int>(gltfbufferAccessor, "componentType"))));
        if (const auto bufferViewIndex = GLTF::Parse(gltfbufferAccessor, "bufferView", true, -1); bufferViewIndex > -1)
            bufferAccessor.SetBufferView(a_Dictionary.Get<SG::BufferView>("bufferViews", bufferViewIndex));
        bufferAccessor.SetByteOffset(GLTF::Parse(gltfbufferAccessor, "byteOffset", true, bufferAccessor.GetByteOffset()));
        bufferAccessor.SetNormalized(GLTF::Parse(gltfbufferAccessor, "normalized", true, bufferAccessor.GetNormalized()));
        a_Dictionary.bufferAccessors.insert(accessorIndex, bufferAccessor);
        ++accessorIndex;
    }
}

static inline void ParseMeshes(const json& a_JSON, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>&)
{
    if (!a_JSON.contains("meshes"))
        return;
#ifndef NDEBUG
    auto timer = Tools::ScopedTimer("Parsing meshes");
#endif
    size_t meshIndex = 0;
    auto defaultMaterial(std::make_shared<SG::Material>("defaultMaterial"));
    for (const auto& gltfMesh : a_JSON["meshes"]) {
        SG::Component::Mesh mesh;
        mesh.name = GLTF::Parse(gltfMesh, "name", true, std::string(mesh.name));
        if (gltfMesh.contains("primitives")) {
            for (const auto& primitive : gltfMesh["primitives"]) {
                auto geometry(std::make_shared<SG::Primitive>());
                auto material { defaultMaterial };
                if (const auto materialIndex = GLTF::Parse(primitive, "material", true, -1); materialIndex > -1)
                    material = a_Dictionary.Get<SG::Material>("materials", materialIndex);
                auto accessorIndex = GLTF::Parse(primitive, "indices", true, -1);
                if (accessorIndex > -1)
                    geometry->SetIndices(a_Dictionary.bufferAccessors.at(accessorIndex));
                geometry->SetDrawingMode(GLTF::GetGeometryDrawingMode(GLTF::DrawingMode(GLTF::Parse(primitive, "mode", true, int(GLTF::DrawingMode::Triangles)))));
                if (primitive.contains("attributes")) {
                    const auto& attributes = primitive["attributes"];
                    const auto COLOR_0     = GLTF::Parse(attributes, "COLOR_0", true, -1);
                    const auto JOINTS_0    = GLTF::Parse(attributes, "JOINTS_0", true, -1);
                    const auto NORMAL      = GLTF::Parse(attributes, "NORMAL", true, -1);
                    const auto POSITION    = GLTF::Parse(attributes, "POSITION", true, -1);
                    const auto TANGENT     = GLTF::Parse(attributes, "TANGENT", true, -1);
                    const auto TEXCOORD_0  = GLTF::Parse(attributes, "TEXCOORD_0", true, -1);
                    const auto TEXCOORD_1  = GLTF::Parse(attributes, "TEXCOORD_1", true, -1);
                    const auto TEXCOORD_2  = GLTF::Parse(attributes, "TEXCOORD_2", true, -1);
                    const auto TEXCOORD_3  = GLTF::Parse(attributes, "TEXCOORD_3", true, -1);
                    const auto WEIGHTS_0   = GLTF::Parse(attributes, "WEIGHTS_0", true, -1);
                    if (COLOR_0 > -1)
                        geometry->SetColors(a_Dictionary.bufferAccessors.at(COLOR_0));
                    if (JOINTS_0 > -1)
                        geometry->SetJoints(a_Dictionary.bufferAccessors.at(JOINTS_0));
                    if (NORMAL > -1)
                        geometry->SetNormals(a_Dictionary.bufferAccessors.at(NORMAL));
                    if (POSITION > -1) {
                        geometry->SetPositions(a_Dictionary.bufferAccessors.at(POSITION));
                        geometry->ComputeBoundingVolume();
                    }
                    if (TEXCOORD_0 > -1)
                        geometry->SetTexCoord0(a_Dictionary.bufferAccessors.at(TEXCOORD_0));
                    if (TEXCOORD_1 > -1)
                        geometry->SetTexCoord1(a_Dictionary.bufferAccessors.at(TEXCOORD_1));
                    if (TEXCOORD_2 > -1)
                        geometry->SetTexCoord2(a_Dictionary.bufferAccessors.at(TEXCOORD_2));
                    if (TEXCOORD_3 > -1)
                        geometry->SetTexCoord3(a_Dictionary.bufferAccessors.at(TEXCOORD_3));
                    if (WEIGHTS_0 > -1)
                        geometry->SetWeights(a_Dictionary.bufferAccessors.at(WEIGHTS_0));
                    if (TANGENT > -1)
                        geometry->SetTangent(a_Dictionary.bufferAccessors.at(TANGENT));
                    else
                        geometry->GenerateTangents();
                }
                // SG::PrimitiveOptimizer optimizer(geometry);
                // geometry                  = optimizer(0.1f);
                mesh.primitives[geometry] = material;
            }
            mesh.ComputeBoundingVolume();
        }
        a_Dictionary.meshes.insert(meshIndex, mesh);
        ++meshIndex;
    }
}

static inline void ParseNodes(const json& a_JSON, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!a_JSON.contains("nodes"))
        return;
#ifndef NDEBUG
    auto timer = Tools::ScopedTimer("Parsing nodes");
#endif
    size_t nodeIndex = 0;
    for (const auto& gltfNode : a_JSON["nodes"]) {
        auto entity     = SG::Node::Create(a_AssetsContainer->GetECSRegistry());
        auto& transform = entity.template GetComponent<SG::Component::Transform>();
        auto& name      = entity.template GetComponent<SG::Component::Name>();
        name            = GLTF::Parse(gltfNode, "name", true, std::string(name));
        if (gltfNode.contains("matrix")) {
            glm::mat4 matrix {};
            auto jsonMatrix = gltfNode["matrix"].get<std::vector<float>>();
            for (unsigned i(0); i < jsonMatrix.size() && i < glm::uint32_t(matrix.length() * 4); i++)
                matrix[i / 4][i % 4] = jsonMatrix[i];
            glm::vec3 scale;
            glm::quat rotation;
            glm::vec3 translation;
            glm::vec3 skew;
            glm::vec4 perspective;
            glm::decompose(matrix, scale, rotation, translation, skew, perspective);
            transform.SetLocalPosition(translation);
            transform.SetLocalRotation(rotation);
            transform.SetLocalScale(scale);
        } else {
            transform.SetLocalPosition(GLTF::Parse(gltfNode, "translation", true, transform.GetLocalPosition()));
            transform.SetLocalRotation(GLTF::Parse(gltfNode, "rotation", true, transform.GetLocalRotation()));
            transform.SetLocalScale(GLTF::Parse(gltfNode, "scale", true, transform.GetLocalScale()));
        }

        a_Dictionary.entities["nodes"].insert(nodeIndex, entity);
        ++nodeIndex;
    }
}

template <typename T, int I>
auto ConvertTo(const SG::BufferAccessor& accessor)
{
    struct DataStruct {
        T data[I];
    };
    std::vector<DataStruct> data;
    for (auto& d : static_cast<SG::TypedBufferAccessor<DataStruct>>(accessor)) {
        data.push_back(d);
    }
    return data;
}

template <typename T>
static inline auto GenerateAnimationChannel(SG::AnimationInterpolation interpolation, const SG::TypedBufferAccessor<T>& keyFramesValues, const SG::TypedBufferAccessor<float>& timings)
{
    SG::AnimationChannel<T> newChannel;
    if (interpolation == SG::AnimationInterpolation::CubicSpline) {
        for (auto i = 0u; i < keyFramesValues.GetSize(); i += 3) {
            typename SG::AnimationChannel<T>::KeyFrame keyFrame;
            keyFrame.inputTangent  = keyFramesValues.at(static_cast<size_t>(i) + 0);
            keyFrame.value         = keyFramesValues.at(static_cast<size_t>(i) + 1);
            keyFrame.outputTangent = keyFramesValues.at(static_cast<size_t>(i) + 2);
            keyFrame.time          = timings.at(i / 3);
            newChannel.InsertKeyFrame(keyFrame);
        }
    } else {
        for (auto i = 0u; i < keyFramesValues.GetSize(); ++i) {
            typename SG::AnimationChannel<T>::KeyFrame keyFrame;
            keyFrame.value = keyFramesValues.at(i);
            keyFrame.time  = timings.at(i);
            newChannel.InsertKeyFrame(keyFrame);
        }
    }
    return newChannel;
}

static inline void ParseAnimations(const json& document, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!document.contains("animations"))
        return;
#ifndef NDEBUG
    auto timer = Tools::ScopedTimer("Parsing animations");
#endif
    for (const auto& gltfAnimation : document["animations"]) {
        auto newAnimation(std::make_shared<SG::Animation>());
        newAnimation->SetName(GLTF::Parse(gltfAnimation, "name", true, std::string(newAnimation->GetName())));
        for (const auto& channel : gltfAnimation["channels"]) {
            auto& sampler { gltfAnimation["samplers"][channel["sampler"].get<int>()] };
            SG::AnimationInterpolation channelInterpolation { SG::AnimationInterpolation::Linear };
            if (sampler.contains("interpolation")) {
                if (sampler["interpolation"] == "LINEAR")
                    channelInterpolation = SG::AnimationInterpolation::Linear;
                else if (sampler["interpolation"] == "STEP")
                    channelInterpolation = SG::AnimationInterpolation::Step;
                else if (sampler["interpolation"] == "CUBICSPLINE")
                    channelInterpolation = SG::AnimationInterpolation::CubicSpline;
            }
            if (channel.contains("target")) {
                auto& target(channel["target"]);
                auto& entity       = a_Dictionary.entities["nodes"].at(GLTF::Parse<int>(target, "node"));
                const auto path    = GLTF::Parse<std::string>(target, "path", true, "");
                const auto& input  = a_Dictionary.bufferAccessors.at(GLTF::Parse<int>(sampler, "input"));
                const auto& output = a_Dictionary.bufferAccessors.at(GLTF::Parse<int>(sampler, "output"));
                if (path == "translation") {
                    auto newChannel   = GenerateAnimationChannel<glm::vec3>(channelInterpolation, output, input);
                    newChannel.target = entity;
                    newAnimation->AddChannelPosition(newChannel);
                } else if (path == "rotation") {
                    auto newChannel   = GenerateAnimationChannel<glm::quat>(channelInterpolation, output, input);
                    newChannel.target = entity;
                    newAnimation->AddChannelRotation(newChannel);
                } else if (path == "scale") {
                    auto newChannel   = GenerateAnimationChannel<glm::vec3>(channelInterpolation, output, input);
                    newChannel.target = entity;
                    newAnimation->AddChannelScale(newChannel);
                } else if (path == "weights") {
                    // newAnimation->GetChannelPosition().target = node;
                } else
                    throw std::runtime_error("Unknown animation path");
            }
        }
        a_Dictionary.Add("animations", newAnimation);
        a_AssetsContainer->AddObject(newAnimation);
    }
}

static inline void ParseSkins(const json& a_JSON, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!a_JSON.contains("skins"))
        return;
#ifndef NDEBUG
    auto timer = Tools::ScopedTimer("Parsing skins");
#endif
    size_t skinIndex = 0;
    for (const auto& gltfSkin : a_JSON["skins"]) {
        SG::Component::MeshSkin skin;
        skin.SetName(GLTF::Parse(gltfSkin, "name", true, std::string(skin.GetName())));
        if (auto inverseBindMatrices = GLTF::Parse(gltfSkin, "inverseBindMatrices", true, -1); inverseBindMatrices > -1)
            skin.inverseBindMatrices = a_Dictionary.bufferAccessors.at(inverseBindMatrices);
        if (gltfSkin.contains("joints")) {
            for (const auto& joint : gltfSkin["joints"])
                skin.AddJoint(a_Dictionary.entities["nodes"].at(joint));
        }
        a_Dictionary.skins.insert(skinIndex, skin);
        ++skinIndex;
    }
}

static inline void ParseScenes(const json& a_JSON, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!a_JSON.contains("scenes"))
        return;
#ifndef NDEBUG
    auto timer = Tools::ScopedTimer("Parsing scenes");
#endif
    for (const auto& gltfScene : a_JSON["scenes"]) {
        auto scene = std::make_shared<SG::Scene>(a_AssetsContainer->GetECSRegistry());
        for (const auto& node : gltfScene["nodes"]) {
            scene->AddEntity(a_Dictionary.entities["nodes"].at(node));
        }
        a_Dictionary.Add("scenes", scene);
        a_AssetsContainer->AddObject(scene);
    }
}

static inline void Parse_KHR_lights_punctual(const json& a_JSON, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>& a_AssetsContainer)
{
#ifndef NDEBUG
    auto timer = Tools::ScopedTimer("Parsing lights");
#endif
    size_t lightIndex = 0;
    for (const auto& gltfLight : a_JSON["lights"]) {
        SG::Component::PunctualLight light;
        if (gltfLight.contains("type")) {
            if (gltfLight["type"] == "spot") {
                SG::Component::LightSpot lightSpot;
                if (gltfLight.contains("spot")) {
                    lightSpot.innerConeAngle = GLTF::Parse(gltfLight["spot"], "innerConeAngle", true, lightSpot.innerConeAngle);
                    lightSpot.outerConeAngle = GLTF::Parse(gltfLight["spot"], "outerConeAngle", true, lightSpot.outerConeAngle);
                }
                lightSpot.range = GLTF::Parse(gltfLight, "range", true, lightSpot.range);
                light           = lightSpot;
            } else if (gltfLight["type"] == "directional") {
                light = SG::Component::LightDirectional();
            } else if (gltfLight["type"] == "point") {
                SG::Component::LightPoint lightPoint;
                lightPoint.range = GLTF::Parse(gltfLight, "range", true, lightPoint.range);
                light            = lightPoint;
            }
        }
        light.name = GLTF::Parse(gltfLight, "name", true, std::string(light.name));
        std::visit([&gltfLight](auto& a_Data) {
            a_Data.color     = GLTF::Parse(gltfLight, "color", true, a_Data.color);
            a_Data.intensity = GLTF::Parse(gltfLight, "intensity", true, a_Data.intensity) / 100.f;
        },
            light);
        a_Dictionary.lights.insert(lightIndex, light);
        ++lightIndex;
    }
}

static inline void ParseGLTFExtensions(const json& a_JSON, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!a_JSON.contains("extensions"))
        return;
#ifndef NDEBUG
    auto timer = Tools::ScopedTimer("Parsing extensions");
#endif
    auto& extensions = a_JSON["extensions"];
    if (extensions.contains("KHR_lights_punctual"))
        Parse_KHR_lights_punctual(extensions["KHR_lights_punctual"], a_Dictionary, a_AssetsContainer);
}

static inline void ParseImages(const std::filesystem::path path, const json& document, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!document.contains("images"))
        return;
#ifndef NDEBUG
    auto timer = Tools::ScopedTimer("Parsing images");
#endif
    std::vector<std::shared_ptr<Asset>> assets;
    for (const auto& gltfImage : document["images"]) {
        auto imageAsset            = std::make_shared<Asset>();
        imageAsset->parsingOptions = a_AssetsContainer->parsingOptions;
        auto uri                   = GLTF::Parse<std::string>(gltfImage, "uri", true, "");
        if (!uri.empty()) {
            imageAsset->SetUri(GLTF::CreateUri(path.parent_path(), uri));
        } else {
            const auto bufferViewIndex = GLTF::Parse(gltfImage, "bufferView", true, -1);
            if (bufferViewIndex == -1) {
                imageAsset->AddObject(std::make_shared<SG::Image2D>());
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
    for (const auto& asset : assets)
        futures.push_back(Parser::AddParsingTask(asset));
    Tools::ThreadPool threadPool;
    for (auto& future : futures) {
        if (auto asset = future.get(); asset->GetLoaded()) {
            std::shared_ptr<SG::Image2D> image = asset->GetCompatible<SG::Image2D>().front();
            auto texture                       = std::make_shared<SG::Texture>(SG::TextureType::Texture2D, image);
            a_Dictionary.Add("images", texture);
            threadPool.PushCommand([texture, a_AssetsContainer] {
                texture->GenerateMipmaps();
                if (a_AssetsContainer->parsingOptions.texture.compress)
                    texture->Compress(a_AssetsContainer->parsingOptions.texture.compressionQuality);
            },
                false);
        } else
            debugLog("Error while parsing" + std::string(asset->GetUri()));
    }
    threadPool.Wait();
}

static inline void ParseNode_KHR_lights_punctual(const ECS::DefaultRegistry::EntityRefType a_Entity, const json& a_JSON, GLTF::Dictionary& a_Dictionary)
{
    if (a_JSON.contains("light"))
        a_Entity.template AddComponent<SG::Component::PunctualLight>(a_Dictionary.lights.at(a_JSON["light"]));
}

static inline void ParseNodeExtensions(const ECS::DefaultRegistry::EntityRefType a_Entity, const json& a_JSON, GLTF::Dictionary& a_Dictionary)
{
    if (a_JSON.contains("KHR_lights_punctual"))
        ParseNode_KHR_lights_punctual(a_Entity, a_JSON["KHR_lights_punctual"], a_Dictionary);
}

static inline void SetParenting(const json& a_JSON, GLTF::Dictionary& a_Dictionary)
{
#ifndef NDEBUG
    auto timer = Tools::ScopedTimer("Setting parenting");
#endif
    // Build parenting relationship
    auto nodeIndex = 0;
    for (const auto& gltfNode : a_JSON["nodes"]) {
        auto& entity     = a_Dictionary.entities["nodes"].at(nodeIndex);
        auto meshIndex   = GLTF::Parse(gltfNode, "mesh", true, -1);
        auto skinIndex   = GLTF::Parse(gltfNode, "skin", true, -1);
        auto cameraIndex = GLTF::Parse(gltfNode, "camera", true, -1);
        if (cameraIndex > -1) {
            entity.template AddComponent<SG::Component::Camera>(a_Dictionary.cameras.at(cameraIndex));
        }
        if (meshIndex > -1) {
            entity.template AddComponent<SG::Component::Mesh>(a_Dictionary.meshes.at(meshIndex));
        }
        if (skinIndex > -1) {
            entity.template AddComponent<SG::Component::MeshSkin>(a_Dictionary.skins.at(skinIndex));
        }
        if (gltfNode.contains("extensions"))
            ParseNodeExtensions(entity, gltfNode["extensions"], a_Dictionary);
        if (gltfNode.contains("children")) {
            entity.template AddComponent<SG::Component::Children>();
            for (const auto& child : gltfNode["children"]) {
                const auto& childEntity = a_Dictionary.entities["nodes"].at(child);
                SG::Node::SetParent(childEntity, entity);
            }
        }
        nodeIndex++;
    }
}

std::shared_ptr<Asset> ParseGLTF(const std::shared_ptr<Asset>& a_AssetsContainer)
{
#ifndef NDEBUG
    auto timer = Tools::ScopedTimer("Parsing GLTF");
#endif
    auto path = a_AssetsContainer->GetUri().DecodePath();
    std::ifstream file(path);
    json document = json::parse(file);
    if (document.empty()) {
        debugLog("Invalid file : " + path.string());
        return a_AssetsContainer;
    }
    auto& mutex = a_AssetsContainer->GetECSRegistry()->GetLock();
    std::scoped_lock lock(mutex);
    auto dictionary = std::make_unique<GLTF::Dictionary>();
    ParseGLTFExtensions(document, *dictionary, a_AssetsContainer);
    ParseCameras(document, *dictionary, a_AssetsContainer);
    ParseBuffers(path, document, *dictionary, a_AssetsContainer);
    ParseBufferViews(document, *dictionary, a_AssetsContainer);
    ParseImages(path, document, *dictionary, a_AssetsContainer);
    ParseSamplers(document, *dictionary, a_AssetsContainer);
    ParseTextureSamplers(document, *dictionary, a_AssetsContainer);
    ParseMaterials(document, *dictionary, a_AssetsContainer);
    ParseBufferAccessors(document, *dictionary, a_AssetsContainer);
    ParseMeshes(document, *dictionary, a_AssetsContainer);
    ParseNodes(document, *dictionary, a_AssetsContainer);
    ParseSkins(document, *dictionary, a_AssetsContainer);
    ParseAnimations(document, *dictionary, a_AssetsContainer);
    ParseScenes(document, *dictionary, a_AssetsContainer);
    SetParenting(document, *dictionary);
    for (auto& scene : a_AssetsContainer->Get<SG::Scene>()) {
        scene->Update();
    }

    a_AssetsContainer->SetAssetType("model/gltf+json");
    a_AssetsContainer->SetLoaded(true);
    return a_AssetsContainer;
}
}
