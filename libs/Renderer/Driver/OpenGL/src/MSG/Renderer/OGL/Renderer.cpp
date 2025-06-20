#include <MSG/Renderer/OGL/Components/LightData.hpp>
#include <MSG/Renderer/OGL/Components/Mesh.hpp>
#include <MSG/Renderer/OGL/Components/MeshSkin.hpp>
#include <MSG/Renderer/OGL/Components/Transform.hpp>
#include <MSG/Renderer/OGL/Primitive.hpp>
#include <MSG/Renderer/OGL/RenderBuffer.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/RendererPathDfd.hpp>
#include <MSG/Renderer/OGL/RendererPathFwd.hpp>
#include <MSG/Renderer/ShaderLibrary.hpp>
#include <MSG/Renderer/Structs.hpp>

#include <MSG/OGLVertexArray.hpp>

#include <MSG/Light/PunctualLight.hpp>
#include <MSG/Mesh.hpp>
#include <MSG/Mesh/Skin.hpp>
#include <MSG/Sampler.hpp>
#include <MSG/Scene.hpp>

#include <MSG/Tools/LazyConstructor.hpp>

#include <MSG/Renderer/OGL/Subsystems/CameraSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/FogSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/FrameSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/LightsSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/MaterialSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/MeshSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/SkinSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/TexturingSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/TransformSubsystem.hpp>

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/vec2.hpp>

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <unordered_set>

namespace MSG::Renderer {
static inline auto CreatePresentVAO(OGLContext& a_Context)
{
    OGLVertexAttributeDescription attribDesc {};
    attribDesc.binding           = 0;
    attribDesc.format.normalized = false;
    attribDesc.format.size       = 1;
    attribDesc.format.type       = GL_BYTE;
    OGLVertexBindingDescription bindingDesc {};
    bindingDesc.buffer = std::make_shared<OGLBuffer>(a_Context, 3, nullptr, 0);
    bindingDesc.index  = 0;
    bindingDesc.offset = 0;
    bindingDesc.stride = 1;
    std::vector<OGLVertexAttributeDescription> attribs { attribDesc };
    std::vector<OGLVertexBindingDescription> bindings { bindingDesc };
    return std::make_shared<OGLVertexArray>(a_Context,
        3, attribs, bindings);
}

Impl::Impl(const CreateRendererInfo& a_Info, const RendererSettings& a_Settings)
    : context(CreateHeadlessOGLContext({ .maxPendingTasks = 64 }))
    , version(a_Info.applicationVersion)
    , name(a_Info.name)
    , shaderCompiler(context)
    , presentVAO(CreatePresentVAO(context))
{
    subsystemsLibrary.Add<TransformSubsystem>();
    subsystemsLibrary.Add<MaterialSubsystem>();
    subsystemsLibrary.Add<SkinSubsystem>();
    subsystemsLibrary.Add<FrameSubsystem>(*this);
    subsystemsLibrary.Add<CameraSubsystem>(*this);
    subsystemsLibrary.Add<LightsSubsystem>(*this);
    subsystemsLibrary.Add<FogSubsystem>(*this);
    subsystemsLibrary.Add<MeshSubsystem>(*this);
    subsystemsLibrary.Add<TexturingSubsystem>(*this);
    subsystemsLibrary.Sort();
    // shaderCompiler.PrecompileLibrary();
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
    path->Render(*this);
    frameIndex++;
}

void Impl::Update()
{
    // return quietly
    if (activeScene == nullptr || activeRenderBuffer == nullptr)
        return;
    std::lock_guard lock(activeScene->GetRegistry()->GetLock());
    for (auto& subsystem : subsystemsLibrary.subsystems)
        subsystem->Update(*this, subsystemsLibrary);
    path->Update(*this);
    blurHelpers.Update();
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
    path->UpdateRenderBuffers(*this);
}

void Impl::SetSettings(const RendererSettings& a_Settings)
{
    if (a_Settings.mode == RendererMode::Forward) {
        path = std::make_shared<PathFwd>(*this, a_Settings);
    } else {
        path = std::make_shared<PathDfd>(*this, a_Settings);
    }
    internalResolution = a_Settings.internalResolution;
    enableTAA          = a_Settings.enableTAA;
    shadowQuality      = a_Settings.shadowQuality;
    ssaoQuality        = a_Settings.ssao.quality;
    for (auto& subsystem : subsystemsLibrary.subsystems)
        subsystem->UpdateSettings(*this, a_Settings);
    path->UpdateSettings(*this, a_Settings);
}

void Impl::LoadMesh(
    const ECS::DefaultRegistry::EntityRefType& a_Entity,
    const Mesh& a_Mesh,
    const MSG::Transform& a_Transform)
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
            rLod.emplace_back(rPrimitive, rMaterial);
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
    auto parent     = a_Entity.GetComponent<Parent>().Lock();
    auto& transform = parent.GetComponent<MSG::Transform>().GetWorldTransformMatrix();
    a_Entity.AddComponent<Component::MeshSkin>(context, transform, a_MeshSkin);
}

std::shared_ptr<OGLTexture> Impl::LoadTexture(Texture* a_Texture, const bool& a_Sparse)
{
    return textureLoader(context, a_Texture, a_Sparse);
}

std::shared_ptr<OGLSampler> Impl::LoadSampler(Sampler* a_Sampler)
{
    return samplerLoader(context, a_Sampler);
}

void Load(
    const Handle& a_Renderer,
    const Scene& a_Scene)
{
    auto& registry    = *a_Scene.GetRegistry();
    auto meshView     = registry.GetView<Mesh, MSG::Transform>(ECS::Exclude<Component::Mesh, Component::Transform> {});
    auto meshSkinView = registry.GetView<MeshSkin>(ECS::Exclude<Component::MeshSkin> {});
    auto lightView    = registry.GetView<PunctualLight>(ECS::Exclude<Component::LightData> {});
    for (const auto& [entityID, mesh, transform] : meshView) {
        a_Renderer->LoadMesh(registry.GetEntityRef(entityID), mesh, transform);
    }
    for (const auto& [entityID, sgMeshSkin] : meshSkinView) {
        a_Renderer->LoadMeshSkin(registry.GetEntityRef(entityID), sgMeshSkin);
    }
    for (const auto& [entityID, light] : lightView) {
        registry.AddComponent<Component::LightData>(entityID, *a_Renderer, registry, entityID);
    }
}

void Load(
    const Handle& a_Renderer,
    const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
    if (a_Entity.template HasComponent<Mesh>() && a_Entity.template HasComponent<MSG::Transform>()) {
        const auto& mesh      = a_Entity.template GetComponent<Mesh>();
        const auto& transform = a_Entity.template GetComponent<MSG::Transform>();
        a_Renderer->LoadMesh(a_Entity, mesh, transform);
    }
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

void SetSettings(const Handle& a_Renderer, const RendererSettings& a_Settings)
{
    a_Renderer->SetSettings(a_Settings);
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
