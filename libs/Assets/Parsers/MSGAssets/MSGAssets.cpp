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

static MeshPrimitive::DrawingMode GetDrawingMode(const std::string_view& a_Mode)
{
    using enum MeshPrimitive::DrawingMode;
    if (a_Mode == "Points")
        return Points;
    if (a_Mode == "Lines")
        return Lines;
    if (a_Mode == "LineStrip")
        return LineStrip;
    if (a_Mode == "LineLoop")
        return LineLoop;
    if (a_Mode == "Polygon")
        return Polygon;
    if (a_Mode == "Triangles")
        return Triangles;
    if (a_Mode == "TriangleStrip")
        return TriangleStrip;
    if (a_Mode == "TriangleFan")
        return TriangleFan;
    if (a_Mode == "Quads")
        return Quads;
    if (a_Mode == "QuadStrip")
        return QuadStrip;
    return Unknown;
}

static MaterialExtensionBase::AlphaMode GetAlphaMode(const std::string_view& a_Mode)
{
    using enum MaterialExtensionBase::AlphaMode;
    if (a_Mode == "Opaque")
        return Opaque;
    if (a_Mode == "Mask")
        return Mask;
    if (a_Mode == "Blend")
        return Blend;
    return MaterialExtensionBase::AlphaMode(-1);
}

static TextureType GetTextureType(const std::string_view& a_Type)
{
    using enum TextureType;
    if (a_Type == "Texture1D")
        return Texture1D;
    if (a_Type == "Texture1DArray")
        return Texture1DArray;
    if (a_Type == "Texture2D")
        return Texture2D;
    if (a_Type == "Texture2DArray")
        return Texture2DArray;
    if (a_Type == "Texture2DMultisample")
        return Texture2DMultisample;
    if (a_Type == "Texture2DMultisampleArray")
        return Texture2DMultisampleArray;
    if (a_Type == "Texture3D")
        return Texture3D;
    if (a_Type == "TextureBuffer")
        return TextureBuffer;
    if (a_Type == "TextureCubemap")
        return TextureCubemap;
    if (a_Type == "TextureCubemapArray")
        return TextureCubemapArray;
    if (a_Type == "TextureRectangle")
        return TextureRectangle;
    return Unknown;
}

static PixelSizedFormat GetPixelFormat(const std::string_view& a_Format)
{
    using enum PixelSizedFormat;
    if (a_Format == "Uint8_NormalizedR")
        return Uint8_NormalizedR;
    if (a_Format == "Uint8_NormalizedRG")
        return Uint8_NormalizedRG;
    if (a_Format == "Uint8_NormalizedRGB")
        return Uint8_NormalizedRGB;
    if (a_Format == "Uint8_NormalizedRGBA")
        return Uint8_NormalizedRGBA;
    if (a_Format == "Int8_NormalizedR")
        return Int8_NormalizedR;
    if (a_Format == "Int8_NormalizedRG")
        return Int8_NormalizedRG;
    if (a_Format == "Int8_NormalizedRGB")
        return Int8_NormalizedRGB;
    if (a_Format == "Int8_NormalizedRGBA")
        return Int8_NormalizedRGBA;
    if (a_Format == "Uint8_R")
        return Uint8_R;
    if (a_Format == "Uint8_RG")
        return Uint8_RG;
    if (a_Format == "Uint8_RGB")
        return Uint8_RGB;
    if (a_Format == "Uint8_RGBA")
        return Uint8_RGBA;
    if (a_Format == "Int8_R")
        return Int8_R;
    if (a_Format == "Int8_RG")
        return Int8_RG;
    if (a_Format == "Int8_RGB")
        return Int8_RGB;
    if (a_Format == "Int8_RGBA")
        return Int8_RGBA;
    if (a_Format == "Uint16_NormalizedR")
        return Uint16_NormalizedR;
    if (a_Format == "Uint16_NormalizedRG")
        return Uint16_NormalizedRG;
    if (a_Format == "Uint16_NormalizedRGB")
        return Uint16_NormalizedRGB;
    if (a_Format == "Uint16_NormalizedRGBA")
        return Uint16_NormalizedRGBA;
    if (a_Format == "Int16_NormalizedR")
        return Int16_NormalizedR;
    if (a_Format == "Int16_NormalizedRG")
        return Int16_NormalizedRG;
    if (a_Format == "Int16_NormalizedRGB")
        return Int16_NormalizedRGB;
    if (a_Format == "Int16_NormalizedRGBA")
        return Int16_NormalizedRGBA;
    if (a_Format == "Uint16_R")
        return Uint16_R;
    if (a_Format == "Uint16_RG")
        return Uint16_RG;
    if (a_Format == "Uint16_RGB")
        return Uint16_RGB;
    if (a_Format == "Uint16_RGBA")
        return Uint16_RGBA;
    if (a_Format == "Int16_R")
        return Int16_R;
    if (a_Format == "Int16_RG")
        return Int16_RG;
    if (a_Format == "Int16_RGB")
        return Int16_RGB;
    if (a_Format == "Int16_RGBA")
        return Int16_RGBA;
    if (a_Format == "Uint32_R")
        return Uint32_R;
    if (a_Format == "Uint32_RG")
        return Uint32_RG;
    if (a_Format == "Uint32_RGB")
        return Uint32_RGB;
    if (a_Format == "Uint32_RGBA")
        return Uint32_RGBA;
    if (a_Format == "Int32_R")
        return Int32_R;
    if (a_Format == "Int32_RG")
        return Int32_RG;
    if (a_Format == "Int32_RGB")
        return Int32_RGB;
    if (a_Format == "Int32_RGBA")
        return Int32_RGBA;
    if (a_Format == "Float16_R")
        return Float16_R;
    if (a_Format == "Float16_RG")
        return Float16_RG;
    if (a_Format == "Float16_RGB")
        return Float16_RGB;
    if (a_Format == "Float16_RGBA")
        return Float16_RGBA;
    if (a_Format == "Float32_R")
        return Float32_R;
    if (a_Format == "Float32_RG")
        return Float32_RG;
    if (a_Format == "Float32_RGB")
        return Float32_RGB;
    if (a_Format == "Float32_RGBA")
        return Float32_RGBA;
    if (a_Format == "Depth16")
        return Depth16;
    if (a_Format == "Depth24")
        return Depth24;
    if (a_Format == "Depth32")
        return Depth32;
    if (a_Format == "Depth32F")
        return Depth32F;
    if (a_Format == "Depth24_Stencil8")
        return Depth24_Stencil8;
    if (a_Format == "Depth32F_Stencil8")
        return Depth32F_Stencil8;
    if (a_Format == "Stencil8")
        return Stencil8;
    if (a_Format == "DXT5_RGBA")
        return DXT5_RGBA;
    return Unknown;
}

static LightType GetLightType(const std::string_view& a_Type)
{
    using enum LightType;
    if (a_Type == "Point")
        return Point;
    if (a_Type == "Spot")
        return Spot;
    if (a_Type == "Directional")
        return Directional;
    if (a_Type == "IBL")
        return IBL;
    return Unknown;
}

static LightShadowPrecision GetShadowPrecision(const std::string_view& a_Precision)
{
    using enum LightShadowPrecision;
    if (a_Precision == "High")
        return High;
    if (a_Precision == "Medium")
        return Medium;
    if (a_Precision == "Low")
        return Low;
    return High;
}

static FogAreaOp GetFogAreaOp(const std::string_view& a_Op)
{
    using enum FogAreaOp;
    if (a_Op == "Add")
        return Add;
    if (a_Op == "Replace")
        return Replace;
    MSGErrorFatal("Unknown fog area op value");
    return FogAreaOp(-1);
}

static ShapeCombinationOp GetShapeCombOp(const std::string_view& a_OP)
{
    using enum ShapeCombinationOp;
    if (a_OP == "Add")
        return Add;
    if (a_OP == "Substract")
        return Substract;
    if (a_OP == "Intersect")
        return Intersect;
    if (a_OP == "Xor")
        return Xor;
    MSGErrorFatal("Unknown shame combination op value");
    return ShapeCombinationOp(-1);
}

static CameraProjectionType GetProjectionType(const std::string_view& a_Type)
{
    using enum CameraProjectionType;
    if (a_Type == "PerspectiveInfinite")
        return PerspectiveInfinite;
    if (a_Type == "Perspective")
        return Perspective;
    if (a_Type == "Orthographic")
        return Orthographic;
    MSGErrorFatal("Unknown projection type !");
    return CameraProjectionType(-1);
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
void ParseLightData(LightPoint& a_Data,
    const std::vector<std::shared_ptr<Texture>>& a_Textures,
    const std::vector<std::shared_ptr<Sampler>>& a_Samplers,
    const json& a_JSON)
{
    if (a_JSON.contains("range"))
        a_Data.range = a_JSON["range"];
}

template <>
void ParseLightData(LightSpot& a_Data,
    const std::vector<std::shared_ptr<Texture>>& a_Textures,
    const std::vector<std::shared_ptr<Sampler>>& a_Samplers,
    const json& a_JSON)
{
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
    if (a_JSON.contains("halfSize"))
        a_Data.halfSize = a_JSON["halfSize"];
}

template <>
void ParseLightData(LightIBL& a_Data,
    const std::vector<std::shared_ptr<Texture>>& a_Textures,
    const std::vector<std::shared_ptr<Sampler>>& a_Samplers,
    const json& a_JSON)
{
    if (a_JSON.contains("halfSize"))
        a_Data.halfSize = a_JSON["halfSize"];
    if (a_JSON.contains("boxProjection"))
        a_Data.boxProjection = a_JSON["boxProjection"];
    auto texture = a_Textures[a_JSON["specular"]];
    if (texture->GetType() == TextureType::Texture2D) {
        auto cubemap = CubemapFromEqui(texture->GetPixelDescriptor(), 256, 256, *texture->front());
        texture      = std::make_shared<Texture>(1, std::make_shared<Image>(cubemap));
        TextureGenerateMipmaps(*texture);
    }
    a_Data.specular.texture = texture;
    if (a_JSON.contains("specularSampler"))
        a_Data.specular.sampler = a_Samplers[a_JSON["specularSampler"]];
    if (a_JSON.contains("irradianceCoefficients")) {
        auto& jCoeffs = a_JSON["irradianceCoefficients"];
        for (uint8_t i = 0; i < 16; i++)
            a_Data.irradianceCoefficients[i] = jCoeffs[i];
    } else
        a_Data.GenerateIrradianceCoeffs();
}

static void ParseLightBase(MSGAssetsContainer& a_Container, const json& a_JSON, PunctualLight& a_Light)
{
    if (a_JSON.contains("color"))
        a_Light.SetColor(a_JSON["color"]);
    if (a_JSON.contains("intensity"))
        a_Light.SetIntensity(a_JSON["intensity"]);
    if (a_JSON.contains("falloff"))
        a_Light.SetFalloff(a_JSON["falloff"]);
    if (a_JSON.contains("priority"))
        a_Light.SetPriority(a_JSON["priority"]);
    if (a_JSON.contains("shadowSettings")) {
        LightShadowSettings shadowSettings;
        auto& jShadowSettings = a_JSON["shadowSettings"];
        if (jShadowSettings.contains("castShadow"))
            shadowSettings.castShadow = jShadowSettings["castShadow"];
        if (jShadowSettings.contains("shadowPrecision"))
            shadowSettings.precision = GetShadowPrecision(jShadowSettings["shadowPrecision"]);
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
        a_Light.SetShadowSettings(shadowSettings);
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
        if (jBufferView.contains("uri")) // this bufferView is sourced from outside
            bufferView = GetFromURI<BufferView>(a_Container, jBufferView);
        else {
            bufferView = std::make_shared<BufferView>();
            bufferView->SetName(jBufferView["name"]);
            bufferView->SetBuffer(buffers[jBufferView["buffer"]]);
            bufferView->SetByteLength(jBufferView["size"]);
            bufferView->SetByteOffset(jBufferView["offset"]);
        }
        MSGCheckErrorFatal(bufferView == nullptr, "Error while parsing buffer view !");
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
        if (jImage.contains("uri"))
            image = GetFromURI<Image>(a_Container, jImage);
        else {
            image = std::make_shared<Image>();
            image->SetName(jImage["name"]);
            image->SetPixelDescriptor(GetPixelFormat(jImage["pixelFormat"]));
            image->SetSize(jImage["size"]);
            image->SetStorage(BufferViewToVector(bufferViews[jImage["bufferView"]]));
        }
        MSGCheckErrorFatal(image == nullptr, "Error while parsing image !");
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
        if (jTexture.contains("uri"))
            texture = GetFromURI<Texture>(a_Container, jTexture);
        else {
            texture = std::make_shared<Texture>();
            texture->SetType(GetTextureType(jTexture["type"]));
            texture->SetName(jTexture["name"]);
            for (auto& jImage : jTexture["images"])
                texture->emplace_back(images[jImage]);
            if (jTexture.contains("pixelFormat"))
                texture->SetPixelDescriptor(GetPixelFormat(jTexture["pixelFormat"]));
            else
                texture->SetPixelDescriptor(texture->front()->GetPixelDescriptor());
            if (jTexture.contains("size"))
                texture->SetSize(jTexture["size"]);
            else
                texture->SetSize(texture->front()->GetSize());
        }
        MSGCheckErrorFatal(texture == nullptr, "Error while parsing texture !");
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
        if (jMaterial.contains("uri"))
            material = GetFromURI<Material>(a_Container, jMaterial);
        else {
            material = std::make_shared<Material>();
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
                    extension.alphaMode = GetAlphaMode(jExtension["alphaMode"]);
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
        }
        MSGCheckErrorFatal(material == nullptr, "Error while parsing material !");
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
        if (jPrimitive.contains("uri"))
            primitive = GetFromURI<MeshPrimitive>(a_Container, jPrimitive);
        else {
            auto& vertexBufferView = bufferViews.at(jPrimitive["vertexBufferView"]);
            primitive              = std::make_shared<MeshPrimitive>();
            primitive->SetName(jPrimitive["name"]);
            primitive->SetVertices(BufferViewToVector<Vertex>(vertexBufferView));
            primitive->SetHasTexCoord({
                bool(jPrimitive["hasTexCoords"][0]),
                bool(jPrimitive["hasTexCoords"][1]),
                bool(jPrimitive["hasTexCoords"][2]),
                bool(jPrimitive["hasTexCoords"][3]),
            });
            if (jPrimitive.contains("indiceBufferView")) {
                auto& indiceBufferView = bufferViews.at(jPrimitive["indiceBufferView"]);
                primitive->SetIndices(BufferViewToVector<uint32_t>(indiceBufferView));
            }
            if (jPrimitive.contains("drawingMode"))
                primitive->SetDrawingMode(GetDrawingMode(jPrimitive["drawingMode"]));
            if (jPrimitive.contains("castShadow"))
                primitive->SetCastShadow(jPrimitive["castShadow"]);
            primitive->ComputeBoundingVolume();
        }
        MSGCheckErrorFatal(primitive == nullptr, "Error while parsing primitive !");
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
    auto type = GetLightType(a_JSON["type"]);
    PunctualLight light;
    if (a_JSON.contains("copyFrom"))
        light = QueryComponent<PunctualLight>(a_Container, a_JSON["copyFrom"]);
    auto& jLightType = a_JSON["type"];
    auto lightType   = GetLightType(jLightType);
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

static auto ParseFogArea(MSGAssetsContainer& a_Container, const json& a_JSON)
{
    FogArea fogArea;
    if (a_JSON.contains("copyFrom"))
        fogArea = QueryComponent<FogArea>(a_Container, a_JSON["copyFrom"]);
    if (a_JSON.contains("scattering"))
        fogArea.SetScattering(a_JSON["scattering"]);
    if (a_JSON.contains("emissive"))
        fogArea.SetEmissive(a_JSON["emissive"]);
    if (a_JSON.contains("extinction"))
        fogArea.SetExtinction(a_JSON["extinction"]);
    if (a_JSON.contains("phaseG"))
        fogArea.SetPhaseG(a_JSON["phaseG"]);
    if (a_JSON.contains("attenuationExponant"))
        fogArea.SetAttenuationExp(a_JSON["attenuationExponant"]);
    if (a_JSON.contains("extinction"))
        fogArea.SetOp(GetFogAreaOp(a_JSON["op"]));
    for (auto& jShapeComb : a_JSON["shapes"]) {
        auto& shapeComb = fogArea.emplace_back();
        auto& jShape    = jShapeComb["shape"];
        if (jShapeComb.contains("op"))
            shapeComb.op = GetShapeCombOp(jShapeComb["op"]);
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
    }
    return fogArea;
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
        auto& jProj   = a_JSON["projection"];
        auto projType = GetProjectionType(jProj["type"]);
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
    for (auto& jScene : a_JSON["scenes"]) {
        std::shared_ptr<Scene> scene;
        if (jScene.contains("uri"))
            scene = GetFromURI<Scene>(a_Container, jScene);
        else
            scene = std::make_shared<Scene>(a_Container.asset->GetECSRegistry());
        if (jScene.contains("name"))
            scene->SetName(jScene["name"]);
        if (jScene.contains("entities")) {
            for (auto& jEntity : jScene["entities"]) {
                auto& entity = a_Container.entities.at(jEntity);
                if (!entity.HasComponent<Parent>())
                    entity.AddComponent<Parent>();
                scene->AddEntity(entity);
            }
        }
        MSGCheckErrorFatal(scene == nullptr, "Error while parsing scene !");
        a_Container.asset->AddObject(scene);
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
