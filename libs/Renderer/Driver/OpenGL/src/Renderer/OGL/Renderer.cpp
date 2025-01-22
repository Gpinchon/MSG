#include <Renderer/OGL/Components/LightData.hpp>
#include <Renderer/OGL/Components/Mesh.hpp>
#include <Renderer/OGL/Components/MeshSkin.hpp>
#include <Renderer/OGL/Components/Transform.hpp>
#include <Renderer/OGL/Context.hpp>
#include <Renderer/OGL/Material.hpp>
#include <Renderer/OGL/Primitive.hpp>
#include <Renderer/OGL/RAII/Buffer.hpp>
#include <Renderer/OGL/RAII/DebugGroup.hpp>
#include <Renderer/OGL/RAII/FrameBuffer.hpp>
#include <Renderer/OGL/RAII/Program.hpp>
#include <Renderer/OGL/RAII/VertexArray.hpp>
#include <Renderer/OGL/RAII/Wrapper.hpp>
#include <Renderer/OGL/RenderBuffer.hpp>
#include <Renderer/OGL/Renderer.hpp>
#include <Renderer/OGL/RendererPathFwd.hpp>
#include <Renderer/ShaderLibrary.hpp>
#include <Renderer/Structs.hpp>
#ifdef _WIN32
#ifdef IN
#undef IN
#include <Renderer/OGL/Win32/Context.hpp>
#endif // IN
#elif defined(__linux__)
#include <Renderer/OGL/Unix/Context.hpp>
#endif //_WIN32

#include <Buffer.hpp>
#include <Buffer/View.hpp>
#include <Core/Camera.hpp>
#include <Image2D.hpp>
#include <Light/PunctualLight.hpp>
#include <Mesh.hpp>
#include <Mesh/Skin.hpp>
#include <Sampler.hpp>
#include <Texture.hpp>

#include <Entity/Camera.hpp>
#include <Entity/Node.hpp>

#include <Scene.hpp>

#include <Tools/BRDFIntegration.hpp>
#include <Tools/Halton.hpp>
#include <Tools/LazyConstructor.hpp>

#include <Bindings.glsl>
#include <Camera.glsl>
#include <Material.glsl>
#include <Transform.glsl>

#include <cstdlib>
#include <glm/vec2.hpp>
#include <iostream>
#include <stdexcept>
#include <unordered_set>

namespace MSG::Renderer {
Impl::Impl(const CreateRendererInfo& a_Info, const RendererSettings& a_Settings)
    : context(ContextT<Platform::HeadlessContext>({ .maxPendingTasks = 64 }))
    , version(a_Info.applicationVersion)
    , name(a_Info.name)
    , shaderCompiler(context)
    , cameraUBO(UniformBufferT<GLSL::CameraUBO>(context))
{
    shaderCompiler.PrecompileLibrary();
    {
        static Sampler sampler;
        sampler.SetWrapS(Sampler::Wrap::ClampToEdge);
        sampler.SetWrapT(Sampler::Wrap::ClampToEdge);
        sampler.SetWrapR(Sampler::Wrap::ClampToEdge);
        BrdfLutSampler = LoadSampler(&sampler);
    }
    {
        static Sampler sampler;
        sampler.SetMinFilter(Sampler::Filter::LinearMipmapLinear);
        IblSpecSampler = LoadSampler(&sampler);
    }
    glm::uvec3 LUTSize        = { 256, 256, 1 };
    PixelDescriptor pixelDesc = PixelSizedFormat::Uint8_NormalizedRGBA;
    auto brdfLutImage         = std::make_shared<Image2D>(pixelDesc, LUTSize.x, LUTSize.y, std::make_shared<BufferView>(0, LUTSize.x * LUTSize.y * LUTSize.z * pixelDesc.GetPixelSize()));
    auto brdfLutTexture       = Texture(TextureType::Texture2D, brdfLutImage);
    auto brdfIntegration      = Tools::BRDFIntegration::Generate(256, 256, Tools::BRDFIntegration::Type::Standard);
    for (uint32_t z = 0; z < LUTSize.z; ++z) {
        for (uint32_t y = 0; y < LUTSize.y; ++y) {
            for (uint32_t x = 0; x < LUTSize.x; ++x) {
                brdfLutImage->Store({ x, y, z }, { brdfIntegration[x][y], 0, 1 });
            }
        }
    }
    BrdfLut = LoadTexture(&brdfLutTexture);
    SetSettings(a_Settings);
    context.PushCmd([] {
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    });
}

void Impl::Render()
{
    // return quietly
    if (activeScene == nullptr || activeRenderBuffer == nullptr) {
        return;
    }
    context.PushCmd(
        [renderPasses = path->renderPasses]() {
            for (auto& pass : renderPasses)
                pass->Execute();
        });
    context.ExecuteCmds(context.Busy());
}

void Impl::Update()
{
    // return quietly
    if (activeScene == nullptr || activeRenderBuffer == nullptr)
        return;

    UpdateCamera();
    UpdateTransforms();
    UpdateSkins();
    UpdateMeshes();
    lightCuller(activeScene); // DO CULLING
    path->Update(*this);

    // UPDATE BUFFERS
    context.PushCmd([uboToUpdate = std::move(uboToUpdate)]() mutable {
        for (auto const& ubo : uboToUpdate)
            ubo();
    });

    // EXECUTE COMMANDS
    context.ExecuteCmds();
    frameIndex++;
}

void Impl::UpdateMeshes()
{
    std::unordered_set<std::shared_ptr<MSG::Material>> SGMaterials;
    for (auto& entity : activeScene->GetVisibleEntities().meshes) {
        auto& sgMesh = entity.GetComponent<Mesh>();
        for (auto& [primitive, material] : sgMesh.at(entity.lod))
            SGMaterials.insert(material);
    }
    for (auto& SGMaterial : SGMaterials) {
        auto material = materialLoader.Update(*this, SGMaterial.get());
        if (material->needsUpdate)
            uboToUpdate.emplace_back(*material);
    }
}

void Impl::UpdateTransforms()
{
    // Only get the ones with Mesh since the others won't be displayed
    for (auto& entity : activeScene->GetVisibleEntities().meshes) {
        if (!entity.HasComponent<Component::Transform>())
            continue;
        auto& sgMesh                      = entity.GetComponent<Mesh>();
        auto& sgTransform                 = entity.GetComponent<MSG::Core::Transform>().GetWorldTransformMatrix();
        auto& rTransform                  = entity.GetComponent<Component::Transform>();
        GLSL::TransformUBO transformUBO   = rTransform.GetData();
        transformUBO.previous             = transformUBO.current;
        transformUBO.current.modelMatrix  = sgMesh.geometryTransform * sgTransform;
        transformUBO.current.normalMatrix = glm::inverseTranspose(transformUBO.current.modelMatrix);
        rTransform.SetData(transformUBO);
        if (rTransform.needsUpdate)
            uboToUpdate.emplace_back(rTransform);
    }
}

void Impl::UpdateSkins()
{
    for (auto& entity : activeScene->GetVisibleEntities().meshes) {
        if (!entity.HasComponent<Component::MeshSkin>())
            continue;
        auto& sgTransform = entity.GetComponent<MSG::Core::Transform>().GetWorldTransformMatrix();
        auto& sgMeshSkin  = entity.GetComponent<MeshSkin>();
        auto& rMeshSkin   = entity.GetComponent<Component::MeshSkin>();
        rMeshSkin.Update(context, sgTransform, sgMeshSkin);
    }
}

template <unsigned Size>
glm::vec2 Halton23(const unsigned& a_Index)
{
    constexpr auto halton2 = Tools::Halton<2>::Sequence<Size>();
    constexpr auto halton3 = Tools::Halton<3>::Sequence<Size>();
    const auto rIndex      = a_Index % Size;
    return { halton2[rIndex], halton3[rIndex] };
}

static inline auto ApplyTemporalJitter(glm::mat4 a_ProjMat, const uint8_t& a_FrameIndex)
{
    // the jitter amount should go bellow the threshold of velocity buffer
    constexpr float f16lowest = 0.0009765625; // 1/1024
    auto halton               = (Halton23<256>(a_FrameIndex) * 0.5f + 0.5f) * f16lowest;
    a_ProjMat[2][0] += halton.x;
    a_ProjMat[2][1] += halton.y;
    return a_ProjMat;
}

void Impl::UpdateCamera()
{
    auto& currentCamera              = activeScene->GetCamera();
    GLSL::CameraUBO cameraUBOData    = cameraUBO.GetData();
    cameraUBOData.previous           = cameraUBOData.current;
    cameraUBOData.current.position   = currentCamera.GetComponent<MSG::Core::Transform>().GetWorldPosition();
    cameraUBOData.current.projection = currentCamera.GetComponent<Core::Camera>().projection.GetMatrix();
    if (enableTAA)
        cameraUBOData.current.projection = ApplyTemporalJitter(cameraUBOData.current.projection, uint8_t(frameIndex));
    cameraUBOData.current.view = glm::inverse(currentCamera.GetComponent<MSG::Core::Transform>().GetWorldTransformMatrix());
    cameraUBO.SetData(cameraUBOData);
    if (cameraUBO.needsUpdate)
        uboToUpdate.emplace_back(cameraUBO);
}

std::shared_ptr<Material> Impl::LoadMaterial(MSG::Material* a_Material)
{
    return materialLoader.Load(*this, a_Material);
}

void Impl::SetActiveRenderBuffer(const RenderBuffer::Handle& a_RenderBuffer)
{
    if (a_RenderBuffer == activeRenderBuffer)
        return;
    activeRenderBuffer = a_RenderBuffer;
}

void Impl::SetSettings(const RendererSettings& a_Settings)
{
    if (a_Settings.mode == RendererMode::Forward) {
        path = std::make_shared<PathFwd>(*this, a_Settings);
    } else {
        std::cerr << "Render path not implemented yet !\n";
    }
    enableTAA = a_Settings.enableTAA;
}

void Impl::LoadMesh(
    const ECS::DefaultRegistry::EntityRefType& a_Entity,
    const Mesh& a_Mesh,
    const MSG::Core::Transform& a_Transform)
{
    Component::Mesh meshData;
    for (auto& sgLod : a_Mesh) {
        Component::MeshLod rLod;
        for (auto& [primitive, material] : sgLod) {
            auto& rPrimitive = primitiveCache.GetOrCreate(primitive.get(),
                Tools::LazyConstructor(
                    [this, &primitive]() {
                        return std::make_shared<Primitive>(context, *primitive);
                    }));
            auto rMaterial   = LoadMaterial(material.get());
            rLod.push_back(Component::PrimitiveKey { rPrimitive, rMaterial });
        }
        meshData.push_back(rLod);
    }

    auto& transformMatrix          = a_Transform.GetWorldTransformMatrix();
    GLSL::TransformUBO transform   = {};
    transform.current.modelMatrix  = a_Mesh.geometryTransform * transformMatrix;
    transform.current.normalMatrix = glm::inverseTranspose(glm::mat3(transform.current.modelMatrix));
    transform.previous             = transform.current;
    a_Entity.AddComponent<Component::Transform>(context, transform);
    a_Entity.AddComponent<Component::Mesh>(meshData);
}

void Impl::LoadMeshSkin(
    const ECS::DefaultRegistry::EntityRefType& a_Entity,
    const MeshSkin& a_MeshSkin)
{
    auto registry   = a_Entity.GetRegistry();
    auto parent     = registry->GetEntityRef(a_Entity.GetComponent<Core::Parent>());
    auto& transform = parent.GetComponent<MSG::Core::Transform>().GetWorldTransformMatrix();
    a_Entity.AddComponent<Component::MeshSkin>(context, transform, a_MeshSkin);
}

std::shared_ptr<RAII::Texture> Impl::LoadTexture(Texture* a_Texture)
{
    return textureLoader(context, a_Texture);
}

std::shared_ptr<RAII::Sampler> Impl::LoadSampler(Sampler* a_Sampler)
{
    return samplerLoader(context, a_Sampler);
}

void Load(
    const Handle& a_Renderer,
    const Scene& a_Scene)
{
    auto& registry    = a_Scene.GetRegistry();
    auto meshView     = registry->GetView<Mesh, MSG::Core::Transform>(ECS::Exclude<Component::Mesh, Component::Transform> {});
    auto meshSkinView = registry->GetView<MeshSkin>(ECS::Exclude<Component::MeshSkin> {});
    auto lightView    = registry->GetView<PunctualLight>(ECS::Exclude<Component::LightData> {});
    for (const auto& [entityID, mesh, transform] : meshView) {
        a_Renderer->LoadMesh(registry->GetEntityRef(entityID), mesh, transform);
    }
    for (const auto& [entityID, sgMeshSkin] : meshSkinView) {
        a_Renderer->LoadMeshSkin(registry->GetEntityRef(entityID), sgMeshSkin);
    }
    for (const auto& [entityID, light] : lightView) {
        auto entity = registry->GetEntityRef(entityID);
        registry->AddComponent<Component::LightData>(entityID, *a_Renderer, light, entity);
    }
    a_Renderer->context.ExecuteCmds();
}

void Load(
    const Handle& a_Renderer,
    const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
    if (a_Entity.template HasComponent<Mesh>() && a_Entity.template HasComponent<MSG::Core::Transform>()) {
        const auto& mesh      = a_Entity.template GetComponent<Mesh>();
        const auto& transform = a_Entity.template GetComponent<MSG::Core::Transform>();
        a_Renderer->LoadMesh(a_Entity, mesh, transform);
    }
    a_Renderer->context.ExecuteCmds();
}

void Unload(
    const Handle& a_Renderer,
    const Scene& a_Scene)
{
    // TODO implement this
    auto& renderer = *a_Renderer;
    // wait for rendering to be done
    auto& registry = a_Scene.GetRegistry();
    auto view      = registry->GetView<Mesh, Component::Mesh, Component::Transform>();
    for (const auto& [entityID, mesh, primList, transform] : view) {
        registry->RemoveComponent<Component::Mesh>(entityID);
        for (auto& sgLod : mesh) {
            for (auto& [primitive, material] : sgLod) {
                auto primitivePtr = primitive.get();
                auto primitiveItr = renderer.primitiveCache.find(primitivePtr);
                if (primitiveItr == renderer.primitiveCache.end())
                    continue;
                if (primitiveItr->second.use_count() == 1)
                    renderer.primitiveCache.erase(primitiveItr);
            }
        }
    }
}

void Unload(
    const Handle& a_Renderer,
    const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
    auto& renderer = *a_Renderer;
    if (a_Entity.template HasComponent<Component::Mesh>())
        a_Entity.RemoveComponent<Component::Mesh>();
    if (a_Entity.template HasComponent<Component::Transform>())
        a_Entity.RemoveComponent<Component::Transform>();
    if (a_Entity.template HasComponent<Mesh>()) {
        auto& mesh = a_Entity.template GetComponent<Mesh>();
        for (auto& sgLod : mesh) {
            for (auto& [primitive, material] : sgLod) {
                if (renderer.primitiveCache.at(primitive.get()).use_count() == 1)
                    renderer.primitiveCache.erase(primitive.get());
            }
        }
    }
}

void Render(
    const Handle& a_Renderer)
{
    a_Renderer->Render();
}

void Update(const Handle& a_Renderer)
{
    a_Renderer->Update();
}

Handle Create(const CreateRendererInfo& a_Info, const RendererSettings& a_Settings)
{
    return Handle(new Impl(a_Info, a_Settings));
}

void SetActiveRenderBuffer(const Handle& a_Renderer, const RenderBuffer::Handle& a_RenderBuffer)
{
    a_Renderer->SetActiveRenderBuffer(a_RenderBuffer);
}

RenderBuffer::Handle GetActiveRenderBuffer(const Handle& a_Renderer)
{
    return a_Renderer->activeRenderBuffer;
}

void SetActiveScene(const Handle& a_Renderer, Scene* const a_Scene)
{
    a_Renderer->activeScene = a_Scene;
}

Scene* GetActiveScene(const Handle& a_Renderer)
{
    return a_Renderer->activeScene;
}
}
