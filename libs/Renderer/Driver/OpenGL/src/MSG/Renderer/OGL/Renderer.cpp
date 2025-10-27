#include <MSG/Renderer/OGL/Components/LightData.hpp>
#include <MSG/Renderer/OGL/Components/Mesh.hpp>
#include <MSG/Renderer/OGL/Components/MeshSkin.hpp>
#include <MSG/Renderer/OGL/Primitive.hpp>
#include <MSG/Renderer/OGL/RenderBuffer.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/ShaderLibrary.hpp>
#include <MSG/Renderer/Structs.hpp>

#include <MSG/OGLVertexArray.hpp>

#include <MSG/Light/PunctualLight.hpp>
#include <MSG/MaterialSet.hpp>
#include <MSG/Mesh.hpp>
#include <MSG/Mesh/Skin.hpp>
#include <MSG/Sampler.hpp>
#include <MSG/Scene.hpp>

#include <MSG/Tools/LazyConstructor.hpp>

// RenderPasses
#include <MSG/Renderer/OGL/RenderPasses/PassBlendedGeometry.hpp>
#include <MSG/Renderer/OGL/RenderPasses/PassLight.hpp>
#include <MSG/Renderer/OGL/RenderPasses/PassOpaqueGeometry.hpp>
#include <MSG/Renderer/OGL/RenderPasses/PassPostTreatment.hpp>
#include <MSG/Renderer/OGL/RenderPasses/PassPresent.hpp>
#include <MSG/Renderer/OGL/RenderPasses/PassTAA.hpp>

// Subsystems
#include <MSG/Renderer/OGL/Subsystems/CameraSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/FogSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/FrameSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/LightsSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/MaterialSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/MeshSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/SkinSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/TexturingSubsystem.hpp>

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/vec2.hpp>

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <unordered_set>

namespace Msg::Renderer {
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

OGLContext CreateOGLContext(const CreateRendererInfo& a_Info)
{
    if (a_Info.context != nullptr) {
        return std::move(OGLContext(OGLContextCreateInfo { .maxPendingTasks = 64 }, a_Info.context));
    } else {
        return CreateHeadlessOGLContext({ .maxPendingTasks = 64 });
    }
}

Impl::Impl(const CreateRendererInfo& a_Info, const RendererSettings& a_Settings)
    : context(CreateOGLContext(a_Info))
    , renderCmdBuffer(context, OGLCmdBufferType::OneShot)
    , renderFence(true)
    , version(a_Info.applicationVersion)
    , name(a_Info.name)
    , shaderCompiler(context)
    , presentVAO(CreatePresentVAO(context))
{
    subsystemsLibrary.Add<MaterialSubsystem>();
    subsystemsLibrary.Add<SkinSubsystem>();
    subsystemsLibrary.Add<FrameSubsystem>(*this);
    subsystemsLibrary.Add<CameraSubsystem>(*this);
    subsystemsLibrary.Add<LightsSubsystem>(*this);
    subsystemsLibrary.Add<FogSubsystem>(*this);
    subsystemsLibrary.Add<MeshSubsystem>(*this);
    subsystemsLibrary.Add<TexturingSubsystem>(*this);
    subsystemsLibrary.Sort();
    renderPassesLibrary.Add<PassOpaqueGeometry>(*this);
    renderPassesLibrary.Add<PassLight>(*this);
    renderPassesLibrary.Add<PassPostTreatment>(*this);
    renderPassesLibrary.Add<PassBlendedGeometry>(*this);
    renderPassesLibrary.Add<PassTAA>(*this);
    renderPassesLibrary.Add<PassPresent>(*this);
    renderPassesLibrary.Sort();
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
    renderFence.Wait();
    renderFence.Reset();
    renderCmdBuffer.Reset();
    renderCmdBuffer.Begin();
    for (auto& renderPass : renderPassesLibrary.modules)
        renderPass->Render(*this);
    renderCmdBuffer.End();
    renderCmdBuffer.Execute(&renderFence);
    frameIndex++;
}

void Impl::Update()
{
    // return quietly
    if (activeScene == nullptr || activeRenderBuffer == nullptr)
        return;
    std::lock_guard lock(activeScene->GetRegistry()->GetLock());
    for (auto& subsystem : subsystemsLibrary.modules)
        subsystem->Update(*this, subsystemsLibrary);
    for (auto& renderPass : renderPassesLibrary.modules)
        renderPass->Update(*this, renderPassesLibrary);
    blurHelpers.Update();
}

void Impl::SetActiveRenderBuffer(const RenderBuffer::Handle& a_RenderBuffer)
{
    if (a_RenderBuffer == activeRenderBuffer)
        return;
    activeRenderBuffer = a_RenderBuffer;
}

void Impl::SetSettings(const RendererSettings& a_Settings)
{
    // a_Settings.mode is ignored
    settings = a_Settings;
    for (auto& subsystem : subsystemsLibrary.modules)
        subsystem->UpdateSettings(*this, a_Settings);
    for (auto& renderPass : renderPassesLibrary.modules)
        renderPass->UpdateSettings(*this, a_Settings);
}

std::shared_ptr<OGLTexture> Impl::LoadTexture(Texture* a_Texture, const bool& a_Sparse)
{
    return textureLoader(context, a_Texture, a_Sparse);
}

std::shared_ptr<OGLSampler> Impl::LoadSampler(Sampler* a_Sampler)
{
    return samplerLoader(context, a_Sampler);
}

void LoadHierarchy(const Handle& a_Renderer, const Scene& a_Scene, const SceneHierarchyNode& a_FromNode)
{
    auto& registry = *a_Scene.GetRegistry();
    Renderer::Load(a_Renderer, registry.GetEntityRef(a_FromNode.entity));
    for (auto& child : a_FromNode.children)
        LoadHierarchy(a_Renderer, a_Scene, *child);
}

void Load(
    const Handle& a_Renderer,
    const Scene& a_Scene)
{
    Renderer::WaitGPU(a_Renderer);
    LoadHierarchy(a_Renderer, a_Scene, a_Scene.GetHierarchy());
}

void Load(
    const Handle& a_Renderer,
    const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
    for (auto& subsystem : a_Renderer->subsystemsLibrary.modules)
        subsystem->Load(*a_Renderer, a_Entity);
}

void UnloadHierarchy(const Handle& a_Renderer, const Scene& a_Scene, const SceneHierarchyNode& a_FromNode)
{
    auto& registry = *a_Scene.GetRegistry();
    Renderer::Unload(a_Renderer, registry.GetEntityRef(a_FromNode.entity));
    for (auto& child : a_FromNode.children)
        UnloadHierarchy(a_Renderer, a_Scene, *child);
}

void Unload(
    const Handle& a_Renderer,
    const Scene& a_Scene)
{
    Renderer::WaitGPU(a_Renderer);
    UnloadHierarchy(a_Renderer, a_Scene, a_Scene.GetHierarchy());
}

void Unload(
    const Handle& a_Renderer,
    const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
    for (auto& subsystem : a_Renderer->subsystemsLibrary.modules)
        subsystem->Unload(*a_Renderer, a_Entity);
}

void Render(
    const Handle& a_Renderer)
{
    a_Renderer->Render();
}

void Msg::Renderer::WaitGPU(const Handle& a_Renderer)
{
    a_Renderer->context.WaitGPU();
}

void Update(const Handle& a_Renderer)
{
    a_Renderer->Update();
}

void SetSettings(const Handle& a_Renderer, const RendererSettings& a_Settings)
{
    a_Renderer->SetSettings(a_Settings);
}

ModulesLibrary<RenderPassInterface>& Msg::Renderer::GetRenderPassesLibrary(const Handle& a_Renderer)
{
    return a_Renderer->renderPassesLibrary;
}

ModulesLibrary<SubsystemInterface>& GetSubsystemsLibrary(const Handle& a_Renderer)
{
    return a_Renderer->subsystemsLibrary;
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
