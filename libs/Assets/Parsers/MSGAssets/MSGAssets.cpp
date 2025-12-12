#include <MSG/Animation.hpp>
#include <MSG/Animation/Channel.hpp>
#include <MSG/Assets/Asset.hpp>
#include <MSG/Assets/Parser.hpp>
#include <MSG/Buffer.hpp>
#include <MSG/Buffer/Accessor.hpp>
#include <MSG/Buffer/View.hpp>
#include <MSG/Camera.hpp>
#include <MSG/Debug.hpp>
#include <MSG/FogArea.hpp>
#include <MSG/Image.hpp>
#include <MSG/ImageUtils.hpp>
#include <MSG/Light/PunctualLight.hpp>
#include <MSG/LodsGenerator.hpp>
#include <MSG/Material.hpp>
#include <MSG/Material/Extension/Base.hpp>
#include <MSG/Material/Extension/MetallicRoughness.hpp>
#include <MSG/Material/Extension/Sheen.hpp>
#include <MSG/Material/Extension/SpecularGlossiness.hpp>
#include <MSG/Material/TextureInfo.hpp>
#include <MSG/MaterialSet.hpp>
#include <MSG/Mesh.hpp>
#include <MSG/Mesh/Primitive.hpp>
#include <MSG/Mesh/Skin.hpp>
#include <MSG/Sampler.hpp>
#include <MSG/Scene.hpp>
#include <MSG/Texture.hpp>
#include <MSG/TextureUtils.hpp>
#include <MSG/ThreadPool.hpp>
#include <MSG/Tools/Base.hpp>
#include <MSG/Tools/ScopedTimer.hpp>

#include <nlohmann/json.hpp>

#include <glm/ext.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/string_cast.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <numbers>
#include <strstream>

using json = nlohmann::json;

namespace glm {
template <typename T, qualifier Q>
void to_json(json& a_JSON, const glm::qua<T, Q>& a_Val)
{
    a_JSON = json::array();
    for (uint8_t i = 0; i < a_Val.length(); i++)
        a_JSON.push_back(a_Val[i]);
}
template <typename T, qualifier Q>
void from_json(const json& a_JSON, glm::qua<T, Q>& a_Val)
{
    a_Val[0] = a_JSON[0];
    a_Val[1] = a_JSON[1];
    a_Val[2] = a_JSON[2];
    a_Val[3] = a_JSON[3];
}
template <size_t S, typename T, qualifier Q>
void to_json(json& a_JSON, const glm::vec<S, T, Q>& a_Val)
{
    a_JSON = json::array();
    for (uint8_t i = 0; i < a_Val.length(); i++)
        a_JSON.push_back(a_Val[i]);
}
template <size_t S, typename T, qualifier Q>
void from_json(const json& a_JSON, glm::vec<S, T, Q>& a_Val)
{
    for (uint8_t i = 0; i < a_Val.length(); i++)
        a_Val[i] = a_JSON[i];
}
template <size_t Cols, size_t Rows, typename T, qualifier Q>
void to_json(json& a_JSON, const glm::mat<Cols, Rows, T, Q>& a_Val)
{
    a_JSON = json::array();
    for (uint8_t i = 0; i < a_Val.length(); i++)
        a_JSON.push_back(a_Val[i]);
}
template <size_t Cols, size_t Rows, typename T, qualifier Q>
void from_json(const json& a_JSON, glm::mat<Cols, Rows, T, Q>& a_Val)
{
    for (uint8_t i = 0; i < a_Val.length(); i++)
        a_Val[i] = a_JSON[i];
}
}

namespace Msg {
void from_json(const json& a_JSON, MaterialExtensionBase::AlphaMode& a_Val)
{
    using enum MaterialExtensionBase::AlphaMode;
    if (a_JSON == "Opaque")
        a_Val = Opaque;
    else if (a_JSON == "Mask")
        a_Val = Mask;
    else if (a_JSON == "Blend")
        a_Val = Blend;
    else {
        MSGErrorFatal("Unknown projection type !");
        a_Val = MaterialExtensionBase::AlphaMode(-1);
    }
}

void from_json(const json& a_JSON, TextureType& a_Val)
{
    using enum TextureType;
    if (a_JSON == "Texture1D")
        a_Val = Texture1D;
    else if (a_JSON == "Texture1DArray")
        a_Val = Texture1DArray;
    else if (a_JSON == "Texture2D")
        a_Val = Texture2D;
    else if (a_JSON == "Texture2DArray")
        a_Val = Texture2DArray;
    else if (a_JSON == "Texture2DMultisample")
        a_Val = Texture2DMultisample;
    else if (a_JSON == "Texture2DMultisampleArray")
        a_Val = Texture2DMultisampleArray;
    else if (a_JSON == "Texture3D")
        a_Val = Texture3D;
    else if (a_JSON == "TextureBuffer")
        a_Val = TextureBuffer;
    else if (a_JSON == "TextureCubemap")
        a_Val = TextureCubemap;
    else if (a_JSON == "TextureCubemapArray")
        a_Val = TextureCubemapArray;
    else if (a_JSON == "TextureRectangle")
        a_Val = TextureRectangle;
    else {
        MSGErrorFatal("Unknown projection type !");
        a_Val = TextureType(-1);
    }
}

void from_json(const json& a_JSON, PixelSizedFormat& a_Val)
{
    using enum PixelSizedFormat;
    if (a_JSON == "Uint8_NormalizedR")
        a_Val = Uint8_NormalizedR;
    else if (a_JSON == "Uint8_NormalizedRG")
        a_Val = Uint8_NormalizedRG;
    else if (a_JSON == "Uint8_NormalizedRGB")
        a_Val = Uint8_NormalizedRGB;
    else if (a_JSON == "Uint8_NormalizedRGBA")
        a_Val = Uint8_NormalizedRGBA;
    else if (a_JSON == "Int8_NormalizedR")
        a_Val = Int8_NormalizedR;
    else if (a_JSON == "Int8_NormalizedRG")
        a_Val = Int8_NormalizedRG;
    else if (a_JSON == "Int8_NormalizedRGB")
        a_Val = Int8_NormalizedRGB;
    else if (a_JSON == "Int8_NormalizedRGBA")
        a_Val = Int8_NormalizedRGBA;
    else if (a_JSON == "Uint8_R")
        a_Val = Uint8_R;
    else if (a_JSON == "Uint8_RG")
        a_Val = Uint8_RG;
    else if (a_JSON == "Uint8_RGB")
        a_Val = Uint8_RGB;
    else if (a_JSON == "Uint8_RGBA")
        a_Val = Uint8_RGBA;
    else if (a_JSON == "Int8_R")
        a_Val = Int8_R;
    else if (a_JSON == "Int8_RG")
        a_Val = Int8_RG;
    else if (a_JSON == "Int8_RGB")
        a_Val = Int8_RGB;
    else if (a_JSON == "Int8_RGBA")
        a_Val = Int8_RGBA;
    else if (a_JSON == "Uint16_NormalizedR")
        a_Val = Uint16_NormalizedR;
    else if (a_JSON == "Uint16_NormalizedRG")
        a_Val = Uint16_NormalizedRG;
    else if (a_JSON == "Uint16_NormalizedRGB")
        a_Val = Uint16_NormalizedRGB;
    else if (a_JSON == "Uint16_NormalizedRGBA")
        a_Val = Uint16_NormalizedRGBA;
    else if (a_JSON == "Int16_NormalizedR")
        a_Val = Int16_NormalizedR;
    else if (a_JSON == "Int16_NormalizedRG")
        a_Val = Int16_NormalizedRG;
    else if (a_JSON == "Int16_NormalizedRGB")
        a_Val = Int16_NormalizedRGB;
    else if (a_JSON == "Int16_NormalizedRGBA")
        a_Val = Int16_NormalizedRGBA;
    else if (a_JSON == "Uint16_R")
        a_Val = Uint16_R;
    else if (a_JSON == "Uint16_RG")
        a_Val = Uint16_RG;
    else if (a_JSON == "Uint16_RGB")
        a_Val = Uint16_RGB;
    else if (a_JSON == "Uint16_RGBA")
        a_Val = Uint16_RGBA;
    else if (a_JSON == "Int16_R")
        a_Val = Int16_R;
    else if (a_JSON == "Int16_RG")
        a_Val = Int16_RG;
    else if (a_JSON == "Int16_RGB")
        a_Val = Int16_RGB;
    else if (a_JSON == "Int16_RGBA")
        a_Val = Int16_RGBA;
    else if (a_JSON == "Uint32_R")
        a_Val = Uint32_R;
    else if (a_JSON == "Uint32_RG")
        a_Val = Uint32_RG;
    else if (a_JSON == "Uint32_RGB")
        a_Val = Uint32_RGB;
    else if (a_JSON == "Uint32_RGBA")
        a_Val = Uint32_RGBA;
    else if (a_JSON == "Int32_R")
        a_Val = Int32_R;
    else if (a_JSON == "Int32_RG")
        a_Val = Int32_RG;
    else if (a_JSON == "Int32_RGB")
        a_Val = Int32_RGB;
    else if (a_JSON == "Int32_RGBA")
        a_Val = Int32_RGBA;
    else if (a_JSON == "Float16_R")
        a_Val = Float16_R;
    else if (a_JSON == "Float16_RG")
        a_Val = Float16_RG;
    else if (a_JSON == "Float16_RGB")
        a_Val = Float16_RGB;
    else if (a_JSON == "Float16_RGBA")
        a_Val = Float16_RGBA;
    else if (a_JSON == "Float32_R")
        a_Val = Float32_R;
    else if (a_JSON == "Float32_RG")
        a_Val = Float32_RG;
    else if (a_JSON == "Float32_RGB")
        a_Val = Float32_RGB;
    else if (a_JSON == "Float32_RGBA")
        a_Val = Float32_RGBA;
    else if (a_JSON == "Depth16")
        a_Val = Depth16;
    else if (a_JSON == "Depth24")
        a_Val = Depth24;
    else if (a_JSON == "Depth32")
        a_Val = Depth32;
    else if (a_JSON == "Depth32F")
        a_Val = Depth32F;
    else if (a_JSON == "Depth24_Stencil8")
        a_Val = Depth24_Stencil8;
    else if (a_JSON == "Depth32F_Stencil8")
        a_Val = Depth32F_Stencil8;
    else if (a_JSON == "Stencil8")
        a_Val = Stencil8;
    else if (a_JSON == "DXT5_RGBA")
        a_Val = DXT5_RGBA;
    else {
        MSGErrorFatal("Unknown projection type !");
        a_Val = PixelSizedFormat(-1);
    }
}

void from_json(const json& a_JSON, LightType& a_Val)
{
    using enum LightType;
    if (a_JSON == "Point")
        a_Val = Point;
    else if (a_JSON == "Spot")
        a_Val = Spot;
    else if (a_JSON == "Directional")
        a_Val = Directional;
    else if (a_JSON == "IBL")
        a_Val = IBL;
    else {
        MSGErrorFatal("Unknown projection type !");
        a_Val = LightType(-1);
    }
}

void from_json(const json& a_JSON, LightShadowPrecision& a_Val)
{
    using enum LightShadowPrecision;
    if (a_JSON == "High")
        a_Val = High;
    else if (a_JSON == "Medium")
        a_Val = Medium;
    else if (a_JSON == "Low")
        a_Val = Low;
    else {
        MSGErrorFatal("Unknown projection type !");
        a_Val = LightShadowPrecision(-1);
    }
}

void from_json(const json& a_JSON, CameraProjectionType& a_Val)
{
    using enum CameraProjectionType;
    if (a_JSON == "PerspectiveInfinite")
        a_Val = PerspectiveInfinite;
    else if (a_JSON == "Perspective")
        a_Val = Perspective;
    else if (a_JSON == "Orthographic")
        a_Val = Orthographic;
    else {
        MSGErrorFatal("Unknown projection type !");
        a_Val = CameraProjectionType(-1);
    }
}
void from_json(const json& a_JSON, MeshPrimitive::DrawingMode& a_Val)
{
    using enum MeshPrimitive::DrawingMode;
    if (a_JSON == "Points")
        a_Val = Points;
    else if (a_JSON == "Lines")
        a_Val = Lines;
    else if (a_JSON == "LineStrip")
        a_Val = LineStrip;
    else if (a_JSON == "LineLoop")
        a_Val = LineLoop;
    else if (a_JSON == "Polygon")
        a_Val = Polygon;
    else if (a_JSON == "Triangles")
        a_Val = Triangles;
    else if (a_JSON == "TriangleStrip")
        a_Val = TriangleStrip;
    else if (a_JSON == "TriangleFan")
        a_Val = TriangleFan;
    else if (a_JSON == "Quads")
        a_Val = Quads;
    else if (a_JSON == "QuadStrip")
        a_Val = QuadStrip;
    a_Val = Unknown;
}

void from_json(const json& a_JSON, FogAreaOp& a_Val)
{
    using enum FogAreaOp;
    if (a_JSON == "Add")
        a_Val = Add;
    else if (a_JSON == "Replace")
        a_Val = Replace;
    else {
        MSGErrorFatal("Unknown fog area op value");
        a_Val = FogAreaOp(-1);
    }
}

void from_json(const json& a_JSON, ShapeCombinationOp& a_Val)
{
    using enum ShapeCombinationOp;
    if (a_JSON == "Add")
        a_Val = Add;
    else if (a_JSON == "Substract")
        a_Val = Substract;
    else if (a_JSON == "Intersect")
        a_Val = Intersect;
    else if (a_JSON == "Xor")
        a_Val = Xor;
    else {
        MSGErrorFatal("Unknown shame combination op value");
        a_Val = ShapeCombinationOp(-1);
    }
}

void from_json(const json& a_JSON, VolumetricFogSettings& a_Val)
{
    if (a_JSON.contains("minDistance"))
        a_Val.minDistance = a_JSON["minDistance"];
    if (a_JSON.contains("maxDistance"))
        a_Val.maxDistance = a_JSON["maxDistance"];
    if (a_JSON.contains("depthExponent"))
        a_Val.depthExp = a_JSON["depthExponent"];
    if (a_JSON.contains("noiseDensityOffset"))
        a_Val.noiseDensityOffset = a_JSON["noiseDensityOffset"];
    if (a_JSON.contains("noiseDensityScale"))
        a_Val.noiseDensityScale = a_JSON["noiseDensityScale"];
    if (a_JSON.contains("noiseDensityIntensity"))
        a_Val.noiseDensityIntensity = a_JSON["noiseDensityIntensity"];
    if (a_JSON.contains("noiseDensityMaxDist"))
        a_Val.noiseDensityMaxDist = a_JSON["noiseDensityMaxDist"];
}
void from_json(const json& a_JSON, FogSettings& a_Val)
{
    if (a_JSON.contains("scattering"))
        a_Val.globalScattering = a_JSON["scattering"];
    if (a_JSON.contains("emissive"))
        a_Val.globalEmissive = a_JSON["emissive"];
    if (a_JSON.contains("phaseG"))
        a_Val.globalPhaseG = a_JSON["phaseG"];
    if (a_JSON.contains("extinction"))
        a_Val.globalExtinction = a_JSON["extinction"];
    if (a_JSON.contains("fogBackground"))
        a_Val.fogBackground = a_JSON["fogBackground"];
    if (a_JSON.contains("volumetricSettings"))
        a_Val.volumetricFog = a_JSON["volumetricSettings"];
}

void from_json(const json& a_JSON, FogArea& a_Val)
{
    if (a_JSON.contains("scattering"))
        a_Val.SetScattering(a_JSON["scattering"]);
    if (a_JSON.contains("emissive"))
        a_Val.SetEmissive(a_JSON["emissive"]);
    if (a_JSON.contains("extinction"))
        a_Val.SetExtinction(a_JSON["extinction"]);
    if (a_JSON.contains("phaseG"))
        a_Val.SetPhaseG(a_JSON["phaseG"]);
    if (a_JSON.contains("attenuationExponent"))
        a_Val.SetAttenuationExp(a_JSON["attenuationExponent"]);
    if (a_JSON.contains("op"))
        a_Val.SetOp(a_JSON["op"]);
    for (auto& jShapeComb : a_JSON["shapes"]) {
        auto& shapeComb  = a_Val.emplace_back();
        auto& jShape     = jShapeComb["shape"];
        auto& jShapeData = jShape["data"];
        if (jShape["type"] == "Cube") {
            Cube cube;
            cube.center   = jShapeData["center"];
            cube.halfSize = jShapeData["halfSize"];
            shapeComb     = cube;
        } else if (jShape["type"] == "Sphere") {
            Sphere sphere;
            sphere.center = jShapeData["center"];
            sphere.radius = jShapeData["radius"];
            shapeComb     = sphere;
        } else
            MSGErrorFatal("Unknown shape type");
        if (jShapeComb.contains("op"))
            shapeComb.op = jShapeComb["op"];
    }
}
}

namespace Msg::Assets {
struct MSGAssetsContainer {
    std::shared_ptr<Asset> asset;
    std::unordered_map<std::string, std::shared_ptr<Asset>> externals;
    std::vector<ECS::DefaultRegistry::EntityRefType> entities;
};

template <typename T>
T QueryComponent(MSGAssetsContainer& a_Container, const std::string_view& a_Name)
{
    for (auto& [id, name, component] : a_Container.asset->GetECSRegistry()->GetView<Core::Name, T>()) {
        if (name == a_Name)
            return component;
    }
    MSGErrorFatal("Could not find entity named " + std::string(a_Name));
    return {};
}

template <typename T>
std::shared_ptr<T> GetExternal(MSGAssetsContainer& a_Container, const Uri& a_URI)
{
    auto& external = a_Container.externals.at(a_URI.GetPath());
    return external->GetByName<T>(a_URI.GetQuery()).front();
}

static Uri CreateUri(const std::shared_ptr<Asset>& a_Asset, const std::string& a_DataPath)
{
    Uri uri(a_DataPath);
    if (uri.GetScheme() == "external")
        return uri;
    auto resourcePath = std::filesystem::path(uri.DecodePath());
    if (resourcePath.string().rfind("data:", 0) == 0)
        return Uri(resourcePath.string());
    else {
        if (!resourcePath.is_absolute()) {
            auto proximate = std::filesystem::proximate(resourcePath, a_Asset->GetParentPath());
            resourcePath   = a_Asset->GetParentPath() / proximate;
        }
        return Uri(resourcePath);
    }
    return Uri(a_DataPath);
}

template <typename T>
std::shared_ptr<T> GetFromURI(MSGAssetsContainer& a_Container, const json& a_JSON)
{
    std::shared_ptr<T> result;
    Uri uri = CreateUri(a_Container.asset, a_JSON["uri"]);
    if (uri.GetScheme() == "external") // this result is sourced from outside
        result = GetExternal<T>(a_Container, uri);
    else {
        auto asset = Parser::Parse(CreateAsset(uri, a_Container.asset));
        if (asset->GetLoaded()) {
            result = asset->Get<T>().front();
            if (a_JSON.contains("name"))
                result->SetName(a_JSON["name"]);

        } else
            MSGErrorWarning("Failed to load " + std::string(typeid(T).name()));
    }
    return result;
}

static MaterialTextureInfo ParseTextureInfo(
    std::vector<std::shared_ptr<Texture>>& a_Textures,
    std::vector<std::shared_ptr<Sampler>>& a_Samplers,
    const json& a_JSON)
{
    MaterialTextureInfo info;
    if (a_JSON.contains("texture"))
        info.textureSampler.texture = a_Textures.at(a_JSON["texture"]);
    if (a_JSON.contains("sampler"))
        info.textureSampler.sampler = a_Samplers.at(a_JSON["sampler"]);
    if (a_JSON.contains("textureCoordinates"))
        info.texCoord = a_JSON["textureCoordinates"];
    if (a_JSON.contains("transformScale"))
        info.transform.scale = a_JSON["transformScale"];
    if (a_JSON.contains("transformOffset"))
        info.transform.offset = a_JSON["transformOffset"];
    if (a_JSON.contains("transformRotation"))
        info.transform.rotation = a_JSON["transformRotation"];
    return info;
}

template <typename T = std::byte>
static std::vector<T> BufferViewToVector(const std::shared_ptr<BufferView>& a_BV)
{
    auto& buffer    = *a_BV->GetBuffer();
    auto byteOffset = a_BV->GetByteOffset();
    auto byteStride = a_BV->GetByteStride() > 0 ? a_BV->GetByteStride() : sizeof(std::byte);
    std::vector<std::byte> bytes(a_BV->GetByteLength());
    for (uint64_t i = 0, j = byteOffset; i < bytes.size(); i++, j += byteStride)
        bytes.at(i) = bytes.at(j);
    assert(bytes.size() % sizeof(T) == 0);
    std::vector<T> result(bytes.size() / sizeof(T));
    for (uint64_t i = 0; i < result.size(); i++)
        result.at(i) = *reinterpret_cast<T*>(std::addressof(bytes.at(i * sizeof(T))));
    return result;
}

template <>
static std::vector<std::byte> BufferViewToVector(const std::shared_ptr<BufferView>& a_BV)
{
    auto& buffer    = *a_BV->GetBuffer();
    auto byteOffset = a_BV->GetByteOffset();
    auto byteStride = a_BV->GetByteStride() > 0 ? a_BV->GetByteStride() : sizeof(std::byte);
    std::vector<std::byte> bytes(a_BV->GetByteLength());
    for (uint64_t i = 0, j = byteOffset; i < bytes.size(); i++, j += byteStride)
        bytes.at(i) = bytes.at(j);
    return bytes;
}

static std::shared_ptr<Asset> CreateAsset(const Uri& a_Uri, const std::shared_ptr<Asset>& a_ParentAsset)
{
    auto asset = std::make_shared<Asset>(*a_ParentAsset);
    asset->SetLoaded(false);
    asset->SetObjects({}); // don't inherit the parent's objects
    asset->SetUri(a_Uri);
    return asset;
}

template <typename T>
void ParseCameraData(T& a_Data, const json& a_JSON)
{
    static_assert(false && "Unknown projection type");
}

template <>
void ParseCameraData(CameraProjectionPerspectiveInfinite& a_Data, const json& a_JSON)
{
    if (a_JSON.contains("fov"))
        a_Data.fov = a_JSON["fov"];
    if (a_JSON.contains("aspectRatio"))
        a_Data.aspectRatio = a_JSON["aspectRatio"];
    if (a_JSON.contains("znear"))
        a_Data.znear = a_JSON["znear"];
}

template <>
void ParseCameraData(CameraProjectionPerspective& a_Data, const json& a_JSON)
{
    if (a_JSON.contains("fov"))
        a_Data.fov = a_JSON["fov"];
    if (a_JSON.contains("aspectRatio"))
        a_Data.aspectRatio = a_JSON["aspectRatio"];
    if (a_JSON.contains("znear"))
        a_Data.znear = a_JSON["znear"];
    if (a_JSON.contains("zfar"))
        a_Data.zfar = a_JSON["zfar"];
}

template <>
void ParseCameraData(CameraProjectionOrthographic& a_Data, const json& a_JSON)
{
    if (a_JSON.contains("bottom"))
        a_Data.bottom = a_JSON["bottom"];
    if (a_JSON.contains("right"))
        a_Data.right = a_JSON["right"];
    if (a_JSON.contains("bottom"))
        a_Data.bottom = a_JSON["bottom"];
    if (a_JSON.contains("top"))
        a_Data.top = a_JSON["top"];
    if (a_JSON.contains("znear"))
        a_Data.znear = a_JSON["znear"];
    if (a_JSON.contains("zfar"))
        a_Data.zfar = a_JSON["zfar"];
}

template <typename T>
void ParseLightData(T& a_Data,
    const std::vector<std::shared_ptr<Texture>>& a_Textures,
    const std::vector<std::shared_ptr<Sampler>>& a_Samplers,
    const json& a_JSON)
{
    static_assert(false && "Unknown light type");
}

template <>
void ParseLightData(LightBase& a_Data,
    const std::vector<std::shared_ptr<Texture>>& a_Textures,
    const std::vector<std::shared_ptr<Sampler>>& a_Samplers,
    const json& a_JSON)
{
    if (a_JSON.contains("color"))
        a_Data.color = a_JSON["color"];
    if (a_JSON.contains("intensity"))
        a_Data.intensity = a_JSON["intensity"];
    if (a_JSON.contains("falloff"))
        a_Data.falloff = a_JSON["falloff"];
    if (a_JSON.contains("priority"))
        a_Data.priority = a_JSON["priority"];
    if (a_JSON.contains("shadowSettings")) {
        LightShadowSettings shadowSettings;
        auto& jShadowSettings = a_JSON["shadowSettings"];
        if (jShadowSettings.contains("castShadow"))
            shadowSettings.castShadow = jShadowSettings["castShadow"];
        if (jShadowSettings.contains("shadowPrecision"))
            shadowSettings.precision = jShadowSettings["shadowPrecision"];
        if (jShadowSettings.contains("bias"))
            shadowSettings.bias = jShadowSettings["bias"];
        if (jShadowSettings.contains("normalBias"))
            shadowSettings.normalBias = jShadowSettings["normalBias"];
        if (jShadowSettings.contains("blurRadius"))
            shadowSettings.blurRadius = jShadowSettings["blurRadius"];
        if (jShadowSettings.contains("resolution"))
            shadowSettings.resolution = jShadowSettings["resolution"];
        if (jShadowSettings.contains("cascadeCount"))
            shadowSettings.cascadeCount = jShadowSettings["cascadeCount"];
        a_Data.shadowSettings = shadowSettings;
    }
}

template <>
void ParseLightData(LightPoint& a_Data,
    const std::vector<std::shared_ptr<Texture>>& a_Textures,
    const std::vector<std::shared_ptr<Sampler>>& a_Samplers,
    const json& a_JSON)
{
    ParseLightData((LightBase&)a_Data, a_Textures, a_Samplers, a_JSON);
    if (a_JSON.contains("range"))
        a_Data.range = a_JSON["range"];
}

template <>
void ParseLightData(LightSpot& a_Data,
    const std::vector<std::shared_ptr<Texture>>& a_Textures,
    const std::vector<std::shared_ptr<Sampler>>& a_Samplers,
    const json& a_JSON)
{
    ParseLightData((LightBase&)a_Data, a_Textures, a_Samplers, a_JSON);
    if (a_JSON.contains("range"))
        a_Data.range = a_JSON["range"];
    if (a_JSON.contains("innerConeAngle"))
        a_Data.innerConeAngle = a_JSON["innerConeAngle"];
    if (a_JSON.contains("outerConeAngle"))
        a_Data.outerConeAngle = a_JSON["outerConeAngle"];
}

template <>
void ParseLightData(LightDirectional& a_Data,
    const std::vector<std::shared_ptr<Texture>>& a_Textures,
    const std::vector<std::shared_ptr<Sampler>>& a_Samplers,
    const json& a_JSON)
{
    ParseLightData((LightBase&)a_Data, a_Textures, a_Samplers, a_JSON);
    if (a_JSON.contains("halfSize"))
        a_Data.halfSize = a_JSON["halfSize"];
}

template <>
void ParseLightData(LightIBL& a_Data,
    const std::vector<std::shared_ptr<Texture>>& a_Textures,
    const std::vector<std::shared_ptr<Sampler>>& a_Samplers,
    const json& a_JSON)
{
    auto texture = a_Textures[a_JSON["specular"]];
    if (texture->GetType() == TextureType::Texture2D) {
        auto cubemap = CubemapFromEqui(texture->front()->GetPixelDescriptor(), 512, 512, *texture->front());
        texture      = std::make_shared<Texture>(TextureType::TextureCubemap, std::make_shared<Image>(cubemap));
        TextureGenerateMipmaps(*texture);
    }
    a_Data = LightIBL({ 64, 64 }, texture);
    ParseLightData((LightBase&)a_Data, a_Textures, a_Samplers, a_JSON);
    if (a_JSON.contains("halfSize")) {
        a_Data.halfSize         = a_JSON["halfSize"];
        a_Data.innerBoxHalfSize = a_Data.halfSize;
    }
    if (a_JSON.contains("innerBoxOffset"))
        a_Data.innerBoxOffset = a_JSON["innerBoxOffset"];
    if (a_JSON.contains("innerBoxHalfSize"))
        a_Data.innerBoxHalfSize = a_JSON["innerBoxHalfSize"];
    if (a_JSON.contains("boxProjection"))
        a_Data.boxProjection = a_JSON["boxProjection"];
    if (a_JSON.contains("specularSampler"))
        a_Data.specular.sampler = a_Samplers[a_JSON["specularSampler"]];
    if (a_JSON.contains("irradianceCoefficients")) {
        auto& jCoeffs = a_JSON["irradianceCoefficients"];
        for (uint8_t i = 0; i < 16; i++)
            a_Data.irradianceCoefficients[i] = jCoeffs[i];
    }
}

static void PreCreateEntities(MSGAssetsContainer& a_Container, const json& a_JSON)
{
    if (!a_JSON.contains("entities"))
        return;
    for (auto& jEntity : a_JSON["entities"]) {
        auto entity = a_Container.asset->GetECSRegistry()->CreateEntity();
        a_Container.entities.emplace_back(entity);
    }
}

static void ParseExternals(MSGAssetsContainer& a_Container, const json& a_JSON)
{
    if (!a_JSON.contains("externals"))
        return;
    std::vector<Parser::ParsingFuture> futures;
    for (auto& jExternal : a_JSON["externals"]) {
        auto& name                  = jExternal["name"];
        auto& uri                   = jExternal["uri"];
        auto asset                  = CreateAsset(CreateUri(a_Container.asset, uri), a_Container.asset);
        a_Container.externals[name] = asset;
        futures.push_back(Parser::AddParsingTask(asset));
    }
    for (auto& future : futures) {
        if (auto asset = future.get(); asset == nullptr || !asset->GetLoaded()) {
            std::string uri = asset != nullptr ? std::string(asset->GetUri()) : "";
            MSGErrorWarning("Error while parsing asset " + uri);
        }
    }
}

static void ParseBuffers(MSGAssetsContainer& a_Container, const json& a_JSON)
{
    if (!a_JSON.contains("buffers"))
        return;
    for (auto& jBuffer : a_JSON["buffers"]) {
        auto buffer = GetFromURI<Buffer>(a_Container, jBuffer);
        MSGCheckErrorFatal(buffer == nullptr, "Error while parsing buffer !");
        a_Container.asset->AddObject(buffer);
    }
}

static void ParseBufferViews(MSGAssetsContainer& a_Container, const json& a_JSON)
{
    if (!a_JSON.contains("bufferViews"))
        return;
    auto buffers = a_Container.asset->GetCompatible<Buffer>();
    for (auto& jBufferView : a_JSON["bufferViews"]) {
        std::shared_ptr<BufferView> bufferView;
        if (jBufferView.contains("uri")) {
            bufferView = GetFromURI<BufferView>(a_Container, jBufferView);
            MSGCheckErrorFatal(bufferView == nullptr, "Error while parsing buffer view !");
        } else
            bufferView = std::make_shared<BufferView>();
        if (jBufferView.contains("name"))
            bufferView->SetName(jBufferView["name"]);
        if (jBufferView.contains("buffer"))
            bufferView->SetBuffer(buffers[jBufferView["buffer"]]);
        if (jBufferView.contains("size"))
            bufferView->SetByteLength(jBufferView["size"]);
        if (jBufferView.contains("size"))
            bufferView->SetByteOffset(jBufferView["offset"]);
        a_Container.asset->AddObject(bufferView);
    }
}

static void ParseImages(MSGAssetsContainer& a_Container, const json& a_JSON)
{
    if (!a_JSON.contains("images"))
        return;
    auto bufferViews = a_Container.asset->GetCompatible<BufferView>();
    for (auto& jImage : a_JSON["images"]) {
        std::shared_ptr<Image> image;
        if (jImage.contains("uri")) {
            image = GetFromURI<Image>(a_Container, jImage);
            MSGCheckErrorFatal(image == nullptr, "Error while parsing image !");
        } else
            image = std::make_shared<Image>();
        if (jImage.contains("name"))
            image->SetName(jImage["name"]);
        if (jImage.contains("pixelFormat"))
            image->SetPixelDescriptor(PixelSizedFormat(jImage["pixelFormat"]));
        if (jImage.contains("size"))
            image->SetSize(jImage["size"]);
        if (jImage.contains("bufferView"))
            image->SetStorage(BufferViewToVector(bufferViews[jImage["bufferView"]]));
        a_Container.asset->AddObject(image);
    }
}

static void ParseTextures(MSGAssetsContainer& a_Container, const json& a_JSON)
{
    if (!a_JSON.contains("textures"))
        return;
    auto images = a_Container.asset->GetCompatible<Image>();
    for (auto& jTexture : a_JSON["textures"]) {
        std::shared_ptr<Texture> texture;
        if (jTexture.contains("uri")) {
            texture = GetFromURI<Texture>(a_Container, jTexture);
            MSGCheckErrorFatal(texture == nullptr, "Error while parsing texture !");
        } else
            texture = std::make_shared<Texture>();
        if (jTexture.contains("name"))
            texture->SetName(jTexture["name"]);
        if (jTexture.contains("type"))
            texture->SetType(jTexture["type"]);
        for (auto& jImage : jTexture["images"])
            texture->emplace_back(images[jImage]);
        if (jTexture.contains("pixelFormat"))
            texture->SetPixelDescriptor(PixelSizedFormat(jTexture["pixelFormat"]));
        else
            texture->SetPixelDescriptor(texture->front()->GetPixelDescriptor());
        if (jTexture.contains("size"))
            texture->SetSize(jTexture["size"]);
        else
            texture->SetSize(texture->front()->GetSize());
        a_Container.asset->AddObject(texture);
    }
}

static void ParseMaterials(MSGAssetsContainer& a_Container, const json& a_JSON)
{
    if (!a_JSON.contains("materials"))
        return;
    auto textures = a_Container.asset->GetCompatible<Texture>();
    auto samplers = a_Container.asset->GetCompatible<Sampler>();
    for (auto& jMaterial : a_JSON["materials"]) {
        std::shared_ptr<Material> material;
        if (jMaterial.contains("uri")) {
            material = GetFromURI<Material>(a_Container, jMaterial);
            MSGCheckErrorFatal(material == nullptr, "Error while parsing material !");
        } else
            material = std::make_shared<Material>();
        if (jMaterial.contains("name"))
            material->SetName(jMaterial["name"]);
        if (jMaterial.contains("baseExtension")) {
            auto& jExtension = jMaterial["baseExtension"];
            MaterialExtensionBase extension;
            if (jExtension.contains("normalTextureInfo")) {
                auto& jTextureInfo = jExtension["normalTextureInfo"];
                auto& textureInfo  = extension.normalTexture;
                textureInfo        = ParseTextureInfo(textures, samplers, jTextureInfo);
                if (jTextureInfo.contains("scale"))
                    textureInfo.scale = jTextureInfo["scale"];
            }
            if (jExtension.contains("occlusionTextureInfo")) {
                auto& jTextureInfo = jExtension["occlusionTextureInfo"];
                auto& textureInfo  = extension.occlusionTexture;
                textureInfo        = ParseTextureInfo(textures, samplers, jTextureInfo);
                if (jTextureInfo.contains("strength"))
                    textureInfo.strength = jTextureInfo["strength"];
            }
            if (jExtension.contains("emissiveTextureInfo")) {
                auto& jTextureInfo = jExtension["emissiveTextureInfo"];
                auto& textureInfo  = extension.emissiveTexture;
                textureInfo        = ParseTextureInfo(textures, samplers, jTextureInfo);
            }
            if (jExtension.contains("emissiveFactor"))
                extension.emissiveFactor = glm::vec3(jExtension["emissiveFactor"][0], jExtension["emissiveFactor"][1], jExtension["emissiveFactor"][2]);
            if (jExtension.contains("alphaMode"))
                extension.alphaMode = jExtension["alphaMode"];
            if (jExtension.contains("alphaCutoff"))
                extension.alphaCutoff = jExtension["alphaCutoff"];
            if (jExtension.contains("doubleSided"))
                extension.doubleSided = jExtension["doubleSided"];
            if (jExtension.contains("unlit"))
                extension.unlit = jExtension["unlit"];
            material->AddExtension(extension);
        }
        if (jMaterial.contains("metallicRoughnessExtension")) {
            auto& jExtension = jMaterial["metallicRoughnessExtension"];
            MaterialExtensionMetallicRoughness extension;
            if (jExtension.contains("colorTextureInfo")) {
                auto& jTextureInfo = jExtension["colorTextureInfo"];
                auto& textureInfo  = extension.colorTexture;
                textureInfo        = ParseTextureInfo(textures, samplers, jTextureInfo);
            }
            if (jExtension.contains("metallicRoughnessTextureInfo")) {
                auto& jTextureInfo = jExtension["metallicRoughnessTextureInfo"];
                auto& textureInfo  = extension.metallicRoughnessTexture;
                textureInfo        = ParseTextureInfo(textures, samplers, jTextureInfo);
            }
            if (jExtension.contains("colorFactor"))
                extension.colorFactor = jExtension["colorFactor"];
            if (jExtension.contains("metallicFactor"))
                extension.metallicFactor = jExtension["metallicFactor"];
            if (jExtension.contains("roughnessFactor"))
                extension.roughnessFactor = jExtension["roughnessFactor"];
            material->AddExtension(extension);
        }
        if (jMaterial.contains("specularGlossinessExtension")) {
            auto& jExtension = jMaterial["specularGlossinessExtension"];
            MaterialExtensionSpecularGlossiness extension;
            if (jExtension.contains("diffuseTextureInfo")) {
                auto& jTextureInfo = jExtension["diffuseTextureInfo"];
                auto& textureInfo  = extension.diffuseTexture;
                textureInfo        = ParseTextureInfo(textures, samplers, jTextureInfo);
            }
            if (jExtension.contains("specularGlossinessTextureInfo")) {
                auto& jTextureInfo = jExtension["specularGlossinessTextureInfo"];
                auto& textureInfo  = extension.specularGlossinessTexture;
                textureInfo        = ParseTextureInfo(textures, samplers, jTextureInfo);
            }
            if (jExtension.contains("diffuseFactor"))
                extension.diffuseFactor = jExtension["diffuseFactor"];
            if (jExtension.contains("specularFactor"))
                extension.specularFactor = jExtension["specularFactor"];
            if (jExtension.contains("glossinessFactor"))
                extension.glossinessFactor = jExtension["glossinessFactor"];
            material->AddExtension(extension);
        }
        a_Container.asset->AddObject(material);
    }
}

static void ParsePrimitives(MSGAssetsContainer& a_Container, const json& a_JSON)
{
    if (!a_JSON.contains("primitives"))
        return;
    auto bufferViews = a_Container.asset->GetCompatible<BufferView>();
    for (auto& jPrimitive : a_JSON["primitives"]) {
        std::shared_ptr<MeshPrimitive> primitive;
        if (jPrimitive.contains("uri")) {
            primitive = GetFromURI<MeshPrimitive>(a_Container, jPrimitive);
            MSGCheckErrorFatal(primitive == nullptr, "Error while parsing primitive !");
        } else
            primitive = std::make_shared<MeshPrimitive>();
        if (jPrimitive.contains("name"))
            primitive->SetName(jPrimitive["name"]);
        if (jPrimitive.contains("vertexBufferView"))
            primitive->SetVertices(BufferViewToVector<Vertex>(bufferViews[jPrimitive["vertexBufferView"]]));
        if (jPrimitive.contains("hasTexCoords"))
            primitive->SetHasTexCoord({
                bool(jPrimitive["hasTexCoords"][0]),
                bool(jPrimitive["hasTexCoords"][1]),
                bool(jPrimitive["hasTexCoords"][2]),
                bool(jPrimitive["hasTexCoords"][3]),
            });
        if (jPrimitive.contains("indiceBufferView"))
            primitive->SetIndices(BufferViewToVector<uint32_t>(bufferViews[jPrimitive["indiceBufferView"]]));
        if (jPrimitive.contains("drawingMode"))
            primitive->SetDrawingMode(jPrimitive["drawingMode"]);
        if (jPrimitive.contains("castShadow"))
            primitive->SetCastShadow(jPrimitive["castShadow"]);
        primitive->ComputeBoundingVolume();
        a_Container.asset->AddObject(primitive);
    }
}

static auto ParseMesh(MSGAssetsContainer& a_Container, const json& a_JSON)
{
    auto primitives = a_Container.asset->GetCompatible<MeshPrimitive>();
    Mesh mesh;
    if (a_JSON.contains("copyFrom"))
        mesh = QueryComponent<Mesh>(a_Container, a_JSON["copyFrom"]);
    if (a_JSON.contains("lods")) {
        for (auto& jLod : a_JSON["lods"]) {
            auto& lod          = mesh.emplace_back();
            lod.screenCoverage = jLod["screenCoverage"];
            for (auto& jPrimitive : jLod["primitives"]) {
                auto& primitive     = primitives[jPrimitive["primitive"]];
                auto& materialIndex = jPrimitive["materialIndex"];
                lod[primitive]      = materialIndex;
            }
        }
    }
    mesh.ComputeBoundingVolume();
    return mesh;
}

static auto ParseSkin(MSGAssetsContainer& a_Container, const json& a_JSON)
{
    auto bufferViews = a_Container.asset->GetCompatible<BufferView>();
    auto primitives  = a_Container.asset->GetCompatible<MeshPrimitive>();
    MeshSkin skin;
    if (a_JSON.contains("copyFrom"))
        skin = QueryComponent<MeshSkin>(a_Container, a_JSON["copyFrom"]);
    if (a_JSON.contains("jointsRadius"))
        skin.jointsRadius = a_JSON["jointsRadius"];
    for (auto& jJoint : a_JSON["joints"]) {
        auto& entity = a_Container.entities.at(jJoint);
        skin.joints.emplace_back(entity);
    }
    skin.inverseBindMatrices = BufferViewToVector<glm::mat4>(bufferViews[a_JSON["inverseBindMatrices"]]);
    skin.ComputeBoundingVolume();
    return skin;
}

static auto ParsePunctualLight(MSGAssetsContainer& a_Container, const json& a_JSON)
{
    PunctualLight light;
    if (a_JSON.contains("copyFrom"))
        light = QueryComponent<PunctualLight>(a_Container, a_JSON["copyFrom"]);
    LightType lightType = a_JSON["type"];
    if (light.GetType() != lightType) {
        // light type is not of this type, initialize it
        switch (lightType) {
        case LightType::Point:
            light = LightPoint {};
            break;
        case LightType::Spot:
            light = LightSpot {};
            break;
        case LightType::Directional:
            light = LightDirectional {};
            break;
        case LightType::IBL:
            light = LightIBL {};
            break;
        }
    }
    if (a_JSON.contains("data")) {
        auto textures = a_Container.asset->GetCompatible<Texture>();
        auto samplers = a_Container.asset->GetCompatible<Sampler>();
        std::visit(
            [&json = a_JSON["data"], &textures, &samplers](auto& a_Data) {
                ParseLightData(a_Data, textures, samplers, json);
            },
            light);
    }
    return light;
}

static FogArea ParseFogArea(MSGAssetsContainer& a_Container, const json& a_JSON)
{
    if (a_JSON.contains("copyFrom"))
        return QueryComponent<FogArea>(a_Container, a_JSON["copyFrom"]);
    else
        return a_JSON;
}

static auto ParseTransform(MSGAssetsContainer& a_Container, const json& a_JSON)
{
    Transform transform;
    if (a_JSON.contains("copyFrom"))
        transform = QueryComponent<Transform>(a_Container, a_JSON["copyFrom"]);
    if (a_JSON.contains("up"))
        transform.SetLocalUp(a_JSON["up"]);
    if (a_JSON.contains("right"))
        transform.SetLocalRight(a_JSON["right"]);
    if (a_JSON.contains("forward"))
        transform.SetLocalForward(a_JSON["forward"]);
    if (a_JSON.contains("position"))
        transform.SetLocalPosition(a_JSON["position"]);
    if (a_JSON.contains("scale"))
        transform.SetLocalPosition(a_JSON["scale"]);
    if (a_JSON.contains("rotation"))
        transform.SetLocalRotation(a_JSON["rotation"]);
    return transform;
}

static auto ParseCamera(MSGAssetsContainer& a_Container, const json& a_JSON)
{
    Camera camera;
    if (a_JSON.contains("copyFrom"))
        camera = QueryComponent<Camera>(a_Container, a_JSON["copyFrom"]);
    if (a_JSON.contains("projection")) {
        auto& jProj                   = a_JSON["projection"];
        CameraProjectionType projType = jProj["type"];
        if (camera.projection.GetType() != projType) {
            switch (projType) {
            case CameraProjectionType::PerspectiveInfinite:
                camera.projection = CameraProjectionPerspectiveInfinite {};
                break;
            case CameraProjectionType::Perspective:
                camera.projection = CameraProjectionPerspective {};
                break;
            case CameraProjectionType::Orthographic:
                camera.projection = CameraProjectionOrthographic {};
                break;
            }
        }
        if (jProj.contains("data"))
            std::visit([&jData = jProj["data"]](auto& a_Data) {
                ParseCameraData(a_Data, jData);
            },
                camera.projection);
    }
    // TODO parse camera settings too
    return camera;
}

static void ParseEntities(MSGAssetsContainer& a_Container, const json& a_JSON)
{
    if (!a_JSON.contains("entities"))
        return;
    size_t entityIndex = 0;
    for (auto& jEntity : a_JSON["entities"]) {
        auto& entity = a_Container.entities.at(entityIndex);
        auto& bv     = entity.AddComponent<BoundingVolume>();
        if (jEntity.contains("name"))
            entity.AddComponent<Core::Name>(std::string(jEntity["name"]));
        if (jEntity.contains("punctualLight")) {
            auto& light = entity.AddComponent<PunctualLight>(ParsePunctualLight(a_Container, jEntity["punctualLight"]));
            bv += BoundingVolume(glm::vec3(0), light.GetHalfSize());
        }
        if (jEntity.contains("mesh")) {
            auto& mesh = entity.AddComponent<Mesh>(ParseMesh(a_Container, jEntity["mesh"]));
            bv += mesh.boundingVolume;
        }
        if (jEntity.contains("skin")) {
            auto& skin = entity.AddComponent<MeshSkin>(ParseSkin(a_Container, jEntity["skin"]));
            bv += skin.ComputeBoundingVolume();
        }
        if (jEntity.contains("camera")) {
            entity.AddComponent<Camera>(ParseCamera(a_Container, jEntity["camera"]));
        }
        if (jEntity.contains("transform")) {
            auto& transform = entity.AddComponent<Transform>(ParseTransform(a_Container, jEntity["transform"]));
            bv.center       = transform.GetLocalPosition();
        }
        if (jEntity.contains("fogArea"))
            entity.AddComponent<FogArea>(ParseFogArea(a_Container, jEntity["fogArea"]));
        if (jEntity.contains("parent")) {
            auto& parent = a_Container.entities.at(jEntity["parent"]);
            if (!parent.HasComponent<Children>())
                parent.AddComponent<Children>();
            parent.GetComponent<Children>().insert(entity);
            entity.AddComponent<Parent>(parent);
        }
        entityIndex++;
    }
}

static void ParseScenes(MSGAssetsContainer& a_Container, const json& a_JSON)
{
    if (!a_JSON.contains("scenes"))
        return;
    auto textures = a_Container.asset->GetCompatible<Texture>();
    auto samplers = a_Container.asset->GetCompatible<Sampler>();
    for (auto& jScene : a_JSON["scenes"]) {
        std::shared_ptr<Scene> scene;
        if (jScene.contains("uri")) {
            scene = GetFromURI<Scene>(a_Container, jScene);
            MSGCheckErrorFatal(scene == nullptr, "Error while parsing scene !");
        } else
            scene = std::make_shared<Scene>(a_Container.asset->GetECSRegistry());
        if (jScene.contains("name"))
            scene->SetName(jScene["name"]);
        if (jScene.contains("backgroundColor"))
            scene->SetBackgroundColor(jScene["backgroundColor"]);
        if (jScene.contains("skybox")) {
            auto& jSkybox = jScene["skybox"];
            TextureSampler textureSampler;
            textureSampler.texture = textures[jSkybox["texture"]];
            textureSampler.sampler = jSkybox.contains("sampler") ? samplers[jSkybox["sampler"]] : nullptr;
            if (textureSampler.texture->GetType() == TextureType::Texture2D) {
                auto cubemap = CubemapFromEqui(
                    textureSampler.texture->GetPixelDescriptor(),
                    512, 512, *textureSampler.texture->front());
                textureSampler.texture = std::make_shared<Texture>(TextureType::TextureCubemap, std::make_shared<Image>(std::move(cubemap)));
            }
            scene->SetSkybox(textureSampler);
        }
        if (jScene.contains("fogSettings"))
            scene->SetFogSettings(jScene["fogSettings"]);
        if (jScene.contains("camera"))
            scene->SetCamera(scene->GetEntityByName(jScene["camera"]));
        if (jScene.contains("entities")) {
            for (auto& jEntity : jScene["entities"]) {
                auto& entity = a_Container.entities.at(jEntity);
                if (!entity.HasComponent<Parent>())
                    entity.AddComponent<Parent>();
                scene->AddEntity(entity);
            }
        }
        scene->Update();
        a_Container.asset->AddObject(scene);
    }
}

static void ParseAnimations(MSGAssetsContainer& a_Container, const json& a_JSON)
{
    if (!a_JSON.contains("animations"))
        return;
    for (auto& jAnimation : a_JSON["animations"]) {
        std::shared_ptr<Animation> animation;
        if (jAnimation.contains("uri"))
            animation = GetFromURI<Animation>(a_Container, jAnimation);
        a_Container.asset->AddObject(animation);
    }
}

std::shared_ptr<Asset> ParseFromStream(const std::shared_ptr<Asset>& a_Asset, std::istream& a_Stream)
{
#ifdef MSG_DEBUG
    auto timer = Tools::ScopedTimer("Parsing MSGAssets");
#endif
    json j;
    MSGAssetsContainer container;
    try {
        j = json::parse(a_Stream, nullptr, true);
    } catch (const std::exception& e) {
        MSGErrorWarning(e.what());
        return a_Asset;
    }
    container.asset = a_Asset;
    PreCreateEntities(container, j);
    ParseExternals(container, j);
    ParseBuffers(container, j);
    ParseBufferViews(container, j);
    ParseImages(container, j);
    ParseTextures(container, j);
    ParseMaterials(container, j);
    ParsePrimitives(container, j);
    ParseEntities(container, j);
    ParseScenes(container, j);
    ParseAnimations(container, j);
    return a_Asset;
}

std::shared_ptr<Asset> ParseFromFile(const std::shared_ptr<Asset>& a_Asset)
{
    std::ifstream stream(a_Asset->GetUri().DecodePath(), std::ios_base::binary);
    a_Asset->SetParentPath(a_Asset->GetUri().DecodePath().parent_path());
    return ParseFromStream(a_Asset, stream);
}

std::shared_ptr<Asset> ParseFromBinary(const std::shared_ptr<Asset>& a_Container)
{
    std::vector<std::byte> binary = DataUri(a_Container->GetUri()).Decode();
    auto stream                   = std::istrstream(reinterpret_cast<const char*>(binary.data()), binary.size());
    return ParseFromStream(a_Container, stream);
}

std::shared_ptr<Assets::Asset> ParseMSGAssets(const std::shared_ptr<Assets::Asset>& a_Container)
{
    auto& uri = a_Container->GetUri();
    if (uri.GetScheme() == "data") {
        return ParseFromBinary(a_Container);
    } else
        return ParseFromFile(a_Container);
    return a_Container;
}
}
