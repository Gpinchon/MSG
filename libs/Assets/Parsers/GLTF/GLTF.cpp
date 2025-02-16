#include <MSG/Animation.hpp>
#include <MSG/Animation/Channel.hpp>
#include <MSG/Assets/Asset.hpp>
#include <MSG/Assets/Parser.hpp>
#include <MSG/Buffer.hpp>
#include <MSG/Buffer/Accessor.hpp>
#include <MSG/Buffer/View.hpp>
#include <MSG/Camera.hpp>
#include <MSG/Cubemap.hpp>
#include <MSG/Entity/Node.hpp>
#include <MSG/Image2D.hpp>
#include <MSG/Light/PunctualLight.hpp>
#include <MSG/LodsGenerator.hpp>
#include <MSG/Material.hpp>
#include <MSG/Material/Extension/Base.hpp>
#include <MSG/Material/Extension/MetallicRoughness.hpp>
#include <MSG/Material/Extension/Sheen.hpp>
#include <MSG/Material/Extension/SpecularGlossiness.hpp>
#include <MSG/Material/Extension/Unlit.hpp>
#include <MSG/Material/TextureInfo.hpp>
#include <MSG/Mesh.hpp>
#include <MSG/Mesh/Primitive.hpp>
#include <MSG/Mesh/Skin.hpp>
#include <MSG/Sampler.hpp>
#include <MSG/Scene.hpp>
#include <MSG/Texture.hpp>
#include <MSG/Tools/Debug.hpp>
#include <MSG/Tools/ScopedTimer.hpp>
#include <MSG/Tools/ThreadPool.hpp>

#include <nlohmann/json.hpp>

#include <glm/ext.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/string_cast.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>

using json = nlohmann::json;

namespace MSG::Assets {
namespace GLTF {
    struct Dictionary {
        void Add(const std::string& a_TypeName, const std::shared_ptr<Core::Object> a_Object)
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
        std::shared_ptr<Sampler> defaultSampler = std::make_shared<Sampler>();
        Tools::SparseSet<TextureSampler, 4096> textureSamplers;
        Tools::SparseSet<Mesh, 4096> meshes;
        Tools::SparseSet<MeshLods, 4096> lods;
        Tools::SparseSet<MeshSkin, 4096> skins;
        Tools::SparseSet<Camera, 4096> cameras;
        Tools::SparseSet<PunctualLight, 4096> lights;
        Tools::SparseSet<BufferAccessor, 8192> bufferAccessors;
        std::map<std::string, Tools::SparseSet<ECS::DefaultRegistry::EntityRefType, 4096>> entities;
        std::map<std::string, std::vector<std::shared_ptr<Core::Object>>> objects;
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
            return Sampler::Filter::Nearest;
        case TextureFilter::Linear:
            return Sampler::Filter::Linear;
        case TextureFilter::NearestMipmapNearest:
            return Sampler::Filter::NearestMipmapNearest;
        case TextureFilter::LinearMipmapNearest:
            return Sampler::Filter::LinearMipmapNearest;
        case TextureFilter::NearestMipmapLinear:
            return Sampler::Filter::NearestMipmapLinear;
        case TextureFilter::LinearMipmapLinear:
            return Sampler::Filter::LinearMipmapLinear;
        default:
            throw std::runtime_error("Unknown Texture filter");
        }
    }

    static inline auto GetWrap(const TextureWrap& wrap)
    {
        switch (wrap) {
        case TextureWrap::ClampToEdge:
            return Sampler::Wrap::ClampToEdge;
        case TextureWrap::MirroredRepeat:
            return Sampler::Wrap::MirroredRepeat;
        case TextureWrap::Repeat:
            return Sampler::Wrap::Repeat;
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
        using enum Core::DataType;
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
        using enum MeshPrimitive::DrawingMode;
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

static inline void ParseCameras(const json& document, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!document.contains("cameras"))
        return;
#ifndef NDEBUG
    auto timer = Tools::ScopedTimer("Parsing cameras");
#endif
    size_t cameraIndex = 0;
    for (const auto& gltfCamera : document["cameras"]) {
        Camera camera;
        if (gltfCamera["type"] == "perspective") {
            if (gltfCamera["perspective"].contains("zfar")) {
                CameraProjection::Perspective projection;
                projection.zfar   = GLTF::Parse(gltfCamera["perspective"], "zfar", false, projection.zfar);
                projection.znear  = GLTF::Parse(gltfCamera["perspective"], "znear", true, projection.znear);
                projection.fov    = GLTF::Parse(gltfCamera["perspective"], "fov", true, projection.fov);
                camera.projection = projection;
            } else {
                CameraProjection::PerspectiveInfinite projection;
                projection.znear  = GLTF::Parse(gltfCamera["perspective"], "znear", true, projection.znear);
                projection.fov    = glm::degrees(GLTF::Parse(gltfCamera["perspective"], "yfov", true, glm::radians(projection.fov)));
                camera.projection = projection;
            }
        } else if (gltfCamera["type"] == "orthographic") {
            CameraProjection::Orthographic projection;
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
        auto sampler   = std::make_shared<Sampler>();
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
        auto textureSampler = TextureSampler();
        const auto source   = GLTF::Parse(textureValue, "source", true, -1);
        const auto sampler  = GLTF::Parse(textureValue, "sampler", true, -1);
        if (source > -1)
            textureSampler.texture = a_Dictionary.Get<Texture>("images", source);
        if (sampler > -1)
            textureSampler.sampler = a_Dictionary.Get<Sampler>("samplers", sampler);
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
    MaterialTextureInfo texture;
    texture.textureSampler = a_Dictionary.textureSamplers.at(GLTF::Parse<int>(a_JSON, "index"));
    texture.texCoord       = GLTF::Parse(a_JSON, "texCoord", true, texture.texCoord);
    return texture;
}

static inline auto ParseSpecularGlossiness(GLTF::Dictionary& a_Dictionary, const json& extension)
{
    MaterialExtensionSpecularGlossiness specGloss;
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
    Core::SheenExtension sheen;
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

static inline void ParseMaterialExtensions(GLTF::Dictionary& a_Dictionary, const json& a_Extensions, std::shared_ptr<Material> a_Material)
{
    if (a_Extensions.contains("KHR_materials_pbrSpecularGlossiness"))
        a_Material->AddExtension(ParseSpecularGlossiness(a_Dictionary, a_Extensions["KHR_materials_pbrSpecularGlossiness"]));
    if (a_Extensions.contains("KHR_materials_sheen"))
        a_Material->AddExtension(ParseSheen(a_Dictionary, a_Extensions["KHR_materials_sheen"]));
    if (a_Extensions.contains("KHR_materials_unlit"))
        a_Material->AddExtension(Core::UnlitExtension {});
}

static inline auto ParseMetallicRoughness(GLTF::Dictionary& a_Dictionary, const json& a_Extension)
{
    MaterialExtensionMetallicRoughness mra {};
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
    MaterialExtensionBase base;
    base.alphaCutoff    = GLTF::Parse(a_Extension, "alphaCutoff", true, base.alphaCutoff);
    base.doubleSided    = GLTF::Parse(a_Extension, "doubleSided", true, base.doubleSided);
    base.emissiveFactor = GLTF::Parse(a_Extension, "emissiveFactor", true, base.emissiveFactor);
    auto alphaMode      = GLTF::Parse<std::string>(a_Extension, "alphaMode", true, "OPAQUE");
    if (alphaMode == "OPAQUE")
        base.alphaMode = MaterialExtensionBase::AlphaMode::Opaque;
    else if (alphaMode == "MASK")
        base.alphaMode = MaterialExtensionBase::AlphaMode::Mask;
    else if (alphaMode == "BLEND")
        base.alphaMode = MaterialExtensionBase::AlphaMode::Blend;
    if (a_Extension.contains("normalTexture")) {
        const auto& texInfo       = a_Extension["normalTexture"];
        NormalTextureInfo texture = ParseTextureInfo(a_Dictionary, a_Extension["normalTexture"]);
        texture.scale             = GLTF::Parse(texInfo, "scale", true, texture.scale);
        base.normalTexture        = texture;
    }
    if (a_Extension.contains("emissiveTexture"))
        base.emissiveTexture = ParseTextureInfo(a_Dictionary, a_Extension["emissiveTexture"]);
    if (a_Extension.contains("occlusionTexture")) {
        OcclusionTextureInfo texture = ParseTextureInfo(a_Dictionary, a_Extension["occlusionTexture"]);
        texture.strength             = GLTF::Parse(a_Extension["occlusionTexture"], "strength", true, texture.strength);
        base.occlusionTexture        = texture;
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
        auto material = std::make_shared<Material>();
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
        std::shared_ptr<Buffer> buffer = future.get()->Get<Buffer>().front();
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
        auto bufferView(std::make_shared<BufferView>());
        const auto buffer = a_Dictionary.Get<Buffer>("buffers", bufferViewValue["buffer"]);
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
        BufferAccessor bufferAccessor;
        std::shared_ptr<BufferView> bufferView;
        bufferAccessor.SetName(GLTF::Parse(gltfbufferAccessor, "name", true, std::string(bufferAccessor.GetName())));
        bufferAccessor.SetSize(GLTF::Parse<size_t>(gltfbufferAccessor, "count"));
        bufferAccessor.SetComponentNbr(GLTF::GetAccessorComponentNbr(GLTF::Parse<std::string>(gltfbufferAccessor, "type")));
        bufferAccessor.SetComponentType(GLTF::GetAccessorComponentType(GLTF::ComponentType(GLTF::Parse<int>(gltfbufferAccessor, "componentType"))));
        if (const auto bufferViewIndex = GLTF::Parse(gltfbufferAccessor, "bufferView", true, -1); bufferViewIndex > -1)
            bufferAccessor.SetBufferView(a_Dictionary.Get<BufferView>("bufferViews", bufferViewIndex));
        bufferAccessor.SetByteOffset(GLTF::Parse(gltfbufferAccessor, "byteOffset", true, bufferAccessor.GetByteOffset()));
        bufferAccessor.SetNormalized(GLTF::Parse(gltfbufferAccessor, "normalized", true, bufferAccessor.GetNormalized()));
        a_Dictionary.bufferAccessors.insert(accessorIndex, bufferAccessor);
        ++accessorIndex;
    }
}

static inline void ParseMeshes(const json& a_JSON, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>& a_Asset)
{
    if (!a_JSON.contains("meshes"))
        return;
#ifndef NDEBUG
    auto timer = Tools::ScopedTimer("Parsing meshes");
#endif
    uint64_t meshCount = 0;
    auto defaultMaterial(std::make_shared<Material>("defaultMaterial"));
    for (const auto& gltfMesh : a_JSON["meshes"]) {
        Mesh mesh(1);
        mesh.name = GLTF::Parse(gltfMesh, "name", true, std::string(mesh.name));
        if (gltfMesh.contains("primitives")) {
            for (const auto& primitive : gltfMesh["primitives"]) {
                auto geometry(std::make_shared<MeshPrimitive>());
                auto material { defaultMaterial };
                if (const auto materialIndex = GLTF::Parse(primitive, "material", true, -1); materialIndex > -1)
                    material = a_Dictionary.Get<Material>("materials", materialIndex);
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
                mesh[0][geometry] = material;
            }
            mesh.ComputeBoundingVolume();
        }
        a_Dictionary.meshes.insert(meshCount, mesh);
        meshCount++;
    }
    if (!a_Asset->parsingOptions.mesh.generateLODs)
        return;
    LodsGeneratorSettings lodsGeneratorSettings;
    lodsGeneratorSettings.lodsNbr                = a_Asset->parsingOptions.mesh.lodsNbr;
    lodsGeneratorSettings.maxCompressionError    = a_Asset->parsingOptions.mesh.lodsMaxError;
    lodsGeneratorSettings.targetCompressionRatio = a_Asset->parsingOptions.mesh.lodsCompression;
    Tools::ThreadPool tp;
    std::vector<std::future<MeshLods>> futures;
    futures.reserve(meshCount);
    for (uint64_t meshI = 0; meshI < meshCount; meshI++) {
        auto& mesh = a_Dictionary.meshes.at(meshI);
        futures.emplace_back(tp.Enqueue([&lodsGeneratorSettings, mesh]() mutable {
            return GenerateLods(mesh, lodsGeneratorSettings);
        }));
    }
    for (uint64_t meshI = 0; meshI < meshCount; meshI++) {
        a_Dictionary.lods.insert(meshI, futures.at(meshI).get());
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
        auto entity     = Entity::Node::Create(a_AssetsContainer->GetECSRegistry());
        auto& transform = entity.template GetComponent<MSG::Transform>();
        auto& name      = entity.template GetComponent<Core::Name>();
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
auto ConvertTo(const BufferAccessor& accessor)
{
    struct DataStruct {
        T data[I];
    };
    std::vector<DataStruct> data;
    for (auto& d : static_cast<BufferTypedAccessor<DataStruct>>(accessor)) {
        data.push_back(d);
    }
    return data;
}

template <typename T>
static inline auto GenerateAnimationChannel(AnimationInterpolation interpolation, const BufferTypedAccessor<T>& keyFramesValues, const BufferTypedAccessor<float>& timings)
{
    AnimationChannel<T> newChannel;
    if (interpolation == AnimationInterpolation::CubicSpline) {
        for (auto i = 0u; i < keyFramesValues.GetSize(); i += 3) {
            typename AnimationChannel<T>::KeyFrame keyFrame;
            keyFrame.inputTangent  = keyFramesValues.at(static_cast<size_t>(i) + 0);
            keyFrame.value         = keyFramesValues.at(static_cast<size_t>(i) + 1);
            keyFrame.outputTangent = keyFramesValues.at(static_cast<size_t>(i) + 2);
            keyFrame.time          = timings.at(i / 3);
            newChannel.InsertKeyFrame(keyFrame);
        }
    } else {
        for (auto i = 0u; i < keyFramesValues.GetSize(); ++i) {
            typename AnimationChannel<T>::KeyFrame keyFrame;
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
        auto newAnimation(std::make_shared<Animation>());
        newAnimation->SetName(GLTF::Parse(gltfAnimation, "name", true, std::string(newAnimation->GetName())));
        for (const auto& channel : gltfAnimation["channels"]) {
            auto& sampler { gltfAnimation["samplers"][channel["sampler"].get<int>()] };
            AnimationInterpolation channelInterpolation { AnimationInterpolation::Linear };
            if (sampler.contains("interpolation")) {
                if (sampler["interpolation"] == "LINEAR")
                    channelInterpolation = AnimationInterpolation::Linear;
                else if (sampler["interpolation"] == "STEP")
                    channelInterpolation = AnimationInterpolation::Step;
                else if (sampler["interpolation"] == "CUBICSPLINE")
                    channelInterpolation = AnimationInterpolation::CubicSpline;
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
        MeshSkin skin;
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

static inline void ParseScene_EXT_lights_image_based(MSG::Scene& a_Scene, const json& a_JSON, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    auto lightEntity = Entity::Node::Create(a_Scene.GetRegistry());
    lightEntity.AddComponent<PunctualLight>(a_Dictionary.lights.at(a_JSON["light"]));
    a_Scene.AddEntity(lightEntity);
}

static inline void ParseSceneExtensions(MSG::Scene& a_Scene, const json& a_JSON, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!a_JSON.contains("extensions"))
        return;
    auto& extensions = a_JSON["extensions"];
    if (extensions.contains("EXT_lights_image_based"))
        ParseScene_EXT_lights_image_based(a_Scene, extensions["EXT_lights_image_based"], a_Dictionary, a_AssetsContainer);
}

static inline void ParseScenes(const json& a_JSON, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>& a_AssetsContainer)
{
    if (!a_JSON.contains("scenes"))
        return;
#ifndef NDEBUG
    auto timer = Tools::ScopedTimer("Parsing scenes");
#endif
    for (const auto& gltfScene : a_JSON["scenes"]) {
        auto scene = std::make_shared<Scene>(a_AssetsContainer->GetECSRegistry());
        for (const auto& node : gltfScene["nodes"]) {
            scene->AddEntity(a_Dictionary.entities["nodes"].at(node));
        }
        ParseSceneExtensions(*scene, gltfScene, a_Dictionary, a_AssetsContainer);
        a_Dictionary.Add("scenes", scene);
        a_AssetsContainer->AddObject(scene);
    }
}

static inline void Parse_EXT_lights_image_based(const json& a_JSON, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>& a_AssetsContainer)
{
#ifndef NDEBUG
    auto timer = Tools::ScopedTimer("Parsing EXT_lights_image_based");
#endif
    size_t lightIndex = 0;
    for (const auto& gltfLight : a_JSON["lights"]) {
        PunctualLight light;
        auto& lightIBL     = light.emplace<LightIBL>();
        light.name         = GLTF::Parse(gltfLight, "name", true, std::string(light.name));
        lightIBL.intensity = GLTF::Parse(gltfLight, "intensity", true, lightIBL.intensity);
        for (auto i = 0u; i < 9; ++i) {
            auto& gltfLightVec = gltfLight["irradianceCoefficients"].at(i);
            auto& lightVec     = lightIBL.irradianceCoefficients.at(i);
            for (auto j = 0u; j < 3; ++j)
                lightVec[j] = gltfLightVec.at(j);
        }
        lightIBL.specular.texture = std::make_shared<Texture>(MSG::TextureType::TextureCubemap);
        for (const auto& gltfLightImageLevel : gltfLight["specularImages"]) {
            CubemapImageArray cubemapImages;
            for (auto i = 0u; i < 6; ++i) {
                auto& sideTexture   = a_Dictionary.Get<Texture>("images", gltfLightImageLevel[i])->front();
                cubemapImages.at(i) = *std::static_pointer_cast<Image2D>(sideTexture);
                cubemapImages.at(i).FlipY();
            }
            lightIBL.specular.texture->emplace_back(std::make_shared<Cubemap>(cubemapImages));
        }
        lightIBL.specular.texture->SetPixelDescriptor(lightIBL.specular.texture->front()->GetPixelDescriptor());
        lightIBL.specular.texture->SetSize(lightIBL.specular.texture->front()->GetSize());
        // lightIBL.specular.texture->GenerateMipmaps(); // Generate missing mipmaps just in case
        a_Dictionary.lights.insert(lightIndex, light);
        ++lightIndex;
    }
}

static inline void Parse_KHR_lights_punctual(const json& a_JSON, GLTF::Dictionary& a_Dictionary, const std::shared_ptr<Asset>& a_AssetsContainer)
{
#ifndef NDEBUG
    auto timer = Tools::ScopedTimer("Parsing KHR_lights_punctual");
#endif
    size_t lightIndex = 0;
    for (const auto& gltfLight : a_JSON["lights"]) {
        PunctualLight light;
        if (gltfLight.contains("type")) {
            if (gltfLight["type"] == "spot") {
                LightSpot lightSpot;
                if (gltfLight.contains("spot")) {
                    lightSpot.innerConeAngle = GLTF::Parse(gltfLight["spot"], "innerConeAngle", true, lightSpot.innerConeAngle);
                    lightSpot.outerConeAngle = GLTF::Parse(gltfLight["spot"], "outerConeAngle", true, lightSpot.outerConeAngle);
                }
                lightSpot.range = GLTF::Parse(gltfLight, "range", true, lightSpot.range);
                light           = lightSpot;
            } else if (gltfLight["type"] == "directional") {
                light = LightDirectional();
            } else if (gltfLight["type"] == "point") {
                LightPoint lightPoint;
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
    if (extensions.contains("EXT_lights_image_based"))
        Parse_EXT_lights_image_based(extensions["EXT_lights_image_based"], a_Dictionary, a_AssetsContainer);
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
                imageAsset->AddObject(std::make_shared<Image2D>());
                imageAsset->SetLoaded(true);
            } else {
                const auto mimeType = GLTF::Parse<std::string>(gltfImage, "mimeType");
                imageAsset->SetUri(std::string("data:") + mimeType + ",");
                imageAsset->parsingOptions.data.useBufferView = true;
                imageAsset->SetBufferView(a_Dictionary.Get<BufferView>("bufferViews", bufferViewIndex));
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
            std::shared_ptr<Image2D> image = asset->GetCompatible<Image2D>().front();
            auto texture                   = std::make_shared<Texture>(TextureType::Texture2D, image);
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
        a_Entity.template AddComponent<PunctualLight>(a_Dictionary.lights.at(a_JSON["light"]));
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
            entity.template AddComponent<Camera>(a_Dictionary.cameras.at(cameraIndex));
        }
        if (meshIndex > -1) {
            entity.template AddComponent<Mesh>(a_Dictionary.meshes.at(meshIndex));
            if (a_Dictionary.lods.contains(meshIndex)) {
                auto& lod  = a_Dictionary.lods.at(meshIndex);
                auto& mesh = entity.template GetComponent<Mesh>();
                mesh.insert(mesh.end(), lod.begin(), lod.end());
            }
        }
        if (skinIndex > -1) {
            entity.template AddComponent<MeshSkin>(a_Dictionary.skins.at(skinIndex));
        }
        if (gltfNode.contains("extensions"))
            ParseNodeExtensions(entity, gltfNode["extensions"], a_Dictionary);
        if (gltfNode.contains("children")) {
            entity.template AddComponent<Children>();
            for (const auto& child : gltfNode["children"]) {
                const auto& childEntity = a_Dictionary.entities["nodes"].at(child);
                Entity::Node::SetParent(childEntity, entity);
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
    ParseGLTFExtensions(document, *dictionary, a_AssetsContainer);
    ParseScenes(document, *dictionary, a_AssetsContainer);
    SetParenting(document, *dictionary);
    for (auto& scene : a_AssetsContainer->Get<Scene>()) {
        scene->Update();
    }

    a_AssetsContainer->SetAssetType("model/gltf+json");
    a_AssetsContainer->SetLoaded(true);
    return a_AssetsContainer;
}
}
