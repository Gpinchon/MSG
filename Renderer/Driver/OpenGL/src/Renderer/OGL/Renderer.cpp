#include <Renderer/OGL/Components/MeshData.hpp>
#include <Renderer/OGL/Components/Transform.hpp>
#include <Renderer/OGL/Primitive.hpp>
#include <Renderer/OGL/RAII/Buffer.hpp>
#include <Renderer/OGL/RAII/DebugGroup.hpp>
#include <Renderer/OGL/RAII/FrameBuffer.hpp>
#include <Renderer/OGL/RAII/Program.hpp>
#include <Renderer/OGL/RAII/VertexArray.hpp>
#include <Renderer/OGL/RAII/Wrapper.hpp>
#include <Renderer/OGL/RenderBuffer.hpp>
#include <Renderer/OGL/Renderer.hpp>

#include <Renderer/ShaderLibrary.hpp>
#include <Renderer/Structs.hpp>

#include <Camera.glsl>
#include <Transform.glsl>

#include <SG/Component/Camera.hpp>
#include <SG/Component/Mesh.hpp>
#include <SG/Core/Image/Image.hpp>
#include <SG/Core/Texture/Texture.hpp>
#include <SG/Entity/Camera.hpp>
#include <SG/Entity/Node.hpp>
#include <SG/Scene/Scene.hpp>

#include <Tools/LazyConstructor.hpp>

#ifdef _WIN32
#include <GL/wglew.h>
#include <Renderer/OGL/Win32/Context.hpp>
#include <Renderer/OGL/Win32/Error.hpp>
#include <Renderer/OGL/Win32/Window.hpp>
#elif defined __linux__
#include <GL/glew.h>
#include <Renderer/OGL/Unix/Context.hpp>
#endif

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <unordered_set>

namespace TabGraph::Renderer {
void UniformBufferUpdate::operator()() const
{
    glNamedBufferSubData(
        *_buffer, _offset,
        _size, _data.get());
}

auto CreatePresentVAO(Context& a_Context)
{
    VertexAttributeDescription attribDesc {};
    attribDesc.binding           = 0;
    attribDesc.format.normalized = false;
    attribDesc.format.size       = 1;
    attribDesc.format.type       = GL_BYTE;
    VertexBindingDescription bindingDesc {};
    bindingDesc.buffer = RAII::MakePtr<RAII::Buffer>(a_Context, 3, nullptr, 0);
    bindingDesc.index  = 0;
    bindingDesc.offset = 0;
    bindingDesc.stride = 1;
    std::vector<VertexAttributeDescription> attribs { attribDesc };
    std::vector<VertexBindingDescription> bindings { bindingDesc };
    return RAII::MakePtr<RAII::VertexArray>(a_Context,
        3, attribs, bindings);
}

auto CreatePresentShader(Renderer::Impl& a_Renderer)
{
    ShaderState shaderState;
    shaderState.program = a_Renderer.shaderCompiler.CompileProgram("Present");
    return shaderState;
}

auto CreatePresentRenderPass(
    Renderer::Impl& a_Renderer,
    const RenderBuffer::Handle& a_RenderBuffer,
    const std::shared_ptr<RAII::FrameBuffer>& a_FwdFB)
{
    auto& renderBuffer = *a_RenderBuffer;
    auto& context      = a_Renderer.context;
    RenderPassInfo info;
    info.name                   = "Present";
    info.viewportState.viewport = { renderBuffer->width, renderBuffer->height };
    info.frameBufferState       = {
              .framebuffer = RAII::MakePtr<RAII::FrameBuffer>(context, RAII::FrameBufferCreateInfo { .defaultSize = { info.viewportState.viewport, 1 } })
    };
    info.bindings.images = {
        ImageBindingInfo {
            .bindingIndex = 0,
            .texture      = a_FwdFB->info.colorBuffers[OUTPUT_FRAG_FINAL].texture,
            .access       = GL_READ_ONLY,
            .format       = GL_RGBA16F },
        ImageBindingInfo {
            .bindingIndex = 1,
            .texture      = renderBuffer,
            .access       = GL_WRITE_ONLY,
            .format       = GL_RGBA8 }
    };
    info.graphicsPipelines = {
        GraphicsPipelineInfo {
            .depthStencilState  = { .enableDepthTest = false },
            .shaderState        = CreatePresentShader(a_Renderer),
            .inputAssemblyState = { .primitiveTopology = GL_TRIANGLES },
            .rasterizationState = { .cullMode = GL_NONE },
            .vertexInputState   = { .vertexCount = 3, .vertexArray = CreatePresentVAO(context) } }
    };
    return info;
}

/**
 * Deferred Render Target :
 * RT0 : BRDF CDiff/BRDF Alpha (R), BRDF F0/AO (G) GL_RG32UI
 * RT2 : World Normal (RGB)                        GL_RGB16_SNORM
 * RT3 : Velocity (RG)                             GL_RG16F
 * RT4 : Color (Unlit/Emissive/Final Color)        GL_RGBA16F
 * Depth                                           GL_DEPTH_COMPONENT24
 */
auto CreateFwdFB(
    Context& a_Context,
    const glm::uvec2& a_Size)
{
    RAII::FrameBufferCreateInfo info;
    info.defaultSize = { a_Size, 1 };
    info.colorBuffers.resize(OUTPUT_FRAG_COUNT);
    info.colorBuffers[OUTPUT_FRAG_MATERIAL].attachment = GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_MATERIAL;
    info.colorBuffers[OUTPUT_FRAG_NORMAL].attachment   = GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_NORMAL;
    info.colorBuffers[OUTPUT_FRAG_VELOCITY].attachment = GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_VELOCITY;
    info.colorBuffers[OUTPUT_FRAG_FINAL].attachment    = GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_FINAL;
    info.colorBuffers[OUTPUT_FRAG_MATERIAL].texture    = RAII::MakePtr<RAII::Texture2D>(a_Context, a_Size.x, a_Size.y, 1, GL_RG32UI);
    info.colorBuffers[OUTPUT_FRAG_NORMAL].texture      = RAII::MakePtr<RAII::Texture2D>(a_Context, a_Size.x, a_Size.y, 1, GL_RGB16_SNORM);
    info.colorBuffers[OUTPUT_FRAG_VELOCITY].texture    = RAII::MakePtr<RAII::Texture2D>(a_Context, a_Size.x, a_Size.y, 1, GL_RG16F);
    info.colorBuffers[OUTPUT_FRAG_FINAL].texture       = RAII::MakePtr<RAII::Texture2D>(a_Context, a_Size.x, a_Size.y, 1, GL_RGBA16F);
    info.depthBuffer                                   = RAII::MakePtr<RAII::Texture2D>(a_Context, a_Size.x, a_Size.y, 1, GL_DEPTH_COMPONENT24);
    return RAII::MakePtr<RAII::FrameBuffer>(a_Context, info);
}

auto CreateFwdShader(Renderer::Impl& a_Renderer, const RendererMode& a_Mode, uint a_MaterialType)
{
    ShaderState shaderState;
    if (a_MaterialType == MATERIAL_TYPE_SPECULAR_GLOSSINESS)
        shaderState.program = a_Mode == RendererMode::Forward ? a_Renderer.shaderCompiler.CompileProgram("ForwardLitSpecGloss") : a_Renderer.shaderCompiler.CompileProgram("ForwardDeferredSpecGloss");
    else if (a_MaterialType == MATERIAL_TYPE_METALLIC_ROUGHNESS)
        shaderState.program = a_Mode == RendererMode::Forward ? a_Renderer.shaderCompiler.CompileProgram("ForwardLitMetRough") : a_Renderer.shaderCompiler.CompileProgram("ForwardDeferredMetRough");
    return shaderState;
}

auto CreateFwdRenderPass(
    const glm::uvec2& a_Viewport,
    const std::shared_ptr<RAII::FrameBuffer>& a_FB,
    const glm::vec3& a_ClearColor = { 0, 0, 0 })
{
    RenderPassInfo info;
    info.name                         = "Forward";
    info.viewportState.viewport       = a_Viewport;
    info.frameBufferState.framebuffer = a_FB;
    info.frameBufferState.clear.colors.resize(OUTPUT_FRAG_COUNT);
    info.frameBufferState.clear.colors[OUTPUT_FRAG_MATERIAL] = { OUTPUT_FRAG_MATERIAL, { 0u, 0u } };
    info.frameBufferState.clear.colors[OUTPUT_FRAG_NORMAL]   = { OUTPUT_FRAG_NORMAL, { 0.f, 0.f, 0.f } };
    info.frameBufferState.clear.colors[OUTPUT_FRAG_VELOCITY] = { OUTPUT_FRAG_VELOCITY, { 0.f, 0.f } };
    info.frameBufferState.clear.colors[OUTPUT_FRAG_FINAL]    = { OUTPUT_FRAG_FINAL, { a_ClearColor.r, a_ClearColor.g, a_ClearColor.b } };
    info.frameBufferState.clear.depth                        = 1.f;
    info.frameBufferState.drawBuffers                        = {
        GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_MATERIAL,
        GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_NORMAL,
        GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_VELOCITY,
        GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_FINAL
    };
    return info;
}

Impl::Impl(const CreateRendererInfo& a_Info)
    : context(a_Info.display, nullptr, 64)
    , version(a_Info.applicationVersion)
    , name(a_Info.name)
    , shaderCompiler(context)
    , fwdMetRoughShader(CreateFwdShader(*this, a_Info.mode, MATERIAL_TYPE_METALLIC_ROUGHNESS))
    , fwdSpecGlossShader(CreateFwdShader(*this, a_Info.mode, MATERIAL_TYPE_SPECULAR_GLOSSINESS))
    , fwdCameraUBO(UniformBufferT<GLSL::Camera>(context))
    , fwdFB(CreateFwdFB(context, { 2048, 2048 }))
    , fwdRenderPass(CreateRenderPass(CreateFwdRenderPass({ 2048, 2048 }, fwdFB)))
{
    shaderCompiler.PrecompileLibrary();
}

void Impl::Render()
{
    // return quietly
    if (activeScene == nullptr || activeRenderBuffer == nullptr) {
        return;
    }
    context.PushCmd(
        [fwdRenderPass        = fwdRenderPass,
            presentRenderPass = presentRenderPass,
            dstImage          = *activeRenderBuffer]() {
            fwdRenderPass->Execute();
            presentRenderPass->Execute();
        });
    context.ExecuteCmds(context.Busy());
}

void Impl::Update()
{
    // return quietly
    if (activeScene == nullptr || activeRenderBuffer == nullptr)
        return;

    // DO CULLING
    lightCuller(activeScene);
    UpdateCamera();
    UpdateForwardPass();

    // UPDATE BUFFERS
    context.PushCmd([uboToUpdate = std::move(uboToUpdate)]() mutable {
        for (auto const& ubo : uboToUpdate)
            ubo();
    });

    // EXECUTE COMMANDS
    context.ExecuteCmds();
}

void TabGraph::Renderer::Impl::UpdateCamera()
{
    GLSL::Camera cameraUBOData {};
    cameraUBOData.projection = activeScene->GetCamera().template GetComponent<SG::Component::Camera>().projection.GetMatrix();
    cameraUBOData.view       = glm::inverse(SG::Node::GetWorldTransformMatrix(activeScene->GetCamera()));
    fwdCameraUBO.SetData(cameraUBOData);
    if (fwdCameraUBO.needsUpdate)
        uboToUpdate.push_back(fwdCameraUBO);
}

void Impl::UpdateForwardPass()
{
    auto& renderBuffer                                              = *activeRenderBuffer;
    RenderPassInfo passInfo                                         = fwdRenderPass->info;
    passInfo.frameBufferState.framebuffer                           = fwdFB;
    passInfo.frameBufferState.clear.colors[OUTPUT_FRAG_FINAL].color = {
        activeScene->GetBackgroundColor().r, activeScene->GetBackgroundColor().g, activeScene->GetBackgroundColor().b
    };
    passInfo.viewportState.viewport      = { renderBuffer->width, renderBuffer->height };
    passInfo.viewportState.scissorExtent = passInfo.viewportState.viewport;
    passInfo.bindings.buffers            = {
        { GL_UNIFORM_BUFFER, UBO_CAMERA, fwdCameraUBO.buffer, 0, fwdCameraUBO.buffer->size },
        { GL_SHADER_STORAGE_BUFFER, SSBO_VTFS_LIGHTS, lightCuller.GPUlightsBuffer, offsetof(GLSL::VTFSLightsBuffer, lights), lightCuller.GPUlightsBuffer->size },
        { GL_SHADER_STORAGE_BUFFER, SSBO_VTFS_CLUSTERS, lightCuller.GPUclusters, 0, lightCuller.GPUclusters->size }
    };
    passInfo.graphicsPipelines.clear();
    std::unordered_set<std::shared_ptr<SG::Material>> SGMaterials;
    auto view = activeScene->GetRegistry()->GetView<Component::PrimitiveList, Component::Transform, SG::Component::Mesh, SG::Component::Transform>();
    for (const auto& [entityID, rPrimitives, rTransform, sgMesh, sgTransform] : view) {
        auto entityRef = activeScene->GetRegistry()->GetEntityRef(entityID);
        GLSL::Transform transform;
        transform.modelMatrix  = sgMesh.geometryTransform * SG::Node::GetWorldTransformMatrix(entityRef);
        transform.normalMatrix = glm::inverseTranspose(transform.modelMatrix);
        rTransform.SetData(transform);
        for (auto& material : sgMesh.GetMaterials()) {
            SGMaterials.insert(material);
        }
        if (rTransform.needsUpdate)
            uboToUpdate.push_back(rTransform);
        for (auto& primitiveKey : rPrimitives) {
            auto& primitive            = primitiveKey.first;
            auto& material             = primitiveKey.second;
            auto& graphicsPipelineInfo = passInfo.graphicsPipelines.emplace_back();
            if (material->type == MATERIAL_TYPE_METALLIC_ROUGHNESS)
                graphicsPipelineInfo.shaderState = fwdMetRoughShader;
            else if (material->type == MATERIAL_TYPE_SPECULAR_GLOSSINESS)
                graphicsPipelineInfo.shaderState = fwdSpecGlossShader;
            graphicsPipelineInfo.bindings.buffers = {
                { GL_UNIFORM_BUFFER, UBO_TRANSFORM, rTransform.buffer, 0, rTransform.buffer->size },
                { GL_UNIFORM_BUFFER, UBO_MATERIAL, material->buffer, 0, material->buffer->size },
            };
            for (uint i = 0; i < material->textureSamplers.size(); ++i) {
                auto& textureSampler = material->textureSamplers.at(i);
                if (textureSampler.texture != nullptr)
                    graphicsPipelineInfo.bindings.textures.push_back({ i, textureSampler.texture->target, textureSampler.texture, textureSampler.sampler });
                else
                    graphicsPipelineInfo.bindings.textures.push_back({ i, GL_TEXTURE_2D, textureSampler.texture, textureSampler.sampler });
            }
            primitive->FillGraphicsPipelineInfo(graphicsPipelineInfo);
        }
    }
    for (auto& SGMaterial : SGMaterials) {
        auto material = materialLoader.Update(*this, SGMaterial.get());
        if (material->needsUpdate)
            uboToUpdate.push_back(*material);
    }
    fwdRenderPass = CreateRenderPass(passInfo);
}

void TabGraph::Renderer::Impl::UpdatePresentPass()
{
    presentRenderPass = CreateRenderPass(CreatePresentRenderPass(
        *this, activeRenderBuffer, fwdFB));
}

std::shared_ptr<Material> Impl::LoadMaterial(SG::Material* a_Material)
{
    return std::shared_ptr<Material>();
}

std::shared_ptr<RenderPass> Impl::CreateRenderPass(const RenderPassInfo& a_Info)
{
    return std::shared_ptr<RenderPass>(
        new (renderPassMemoryPool.allocate()) RenderPass(a_Info),
        renderPassMemoryPool.deleter());
}

Handle Create(const CreateRendererInfo& a_Info)
{
    return Handle(new Impl(a_Info));
}

void SetActiveRenderBuffer(const Handle& a_Renderer, const RenderBuffer::Handle& a_RenderBuffer)
{
    a_Renderer->SetActiveRenderBuffer(a_RenderBuffer);
}

RenderBuffer::Handle GetActiveRenderBuffer(const Handle& a_Renderer)
{
    return a_Renderer->activeRenderBuffer;
}

void SetActiveScene(const Handle& a_Renderer, SG::Scene* const a_Scene)
{
    a_Renderer->activeScene = a_Scene;
}

SG::Scene* GetActiveScene(const Handle& a_Renderer)
{
    return a_Renderer->activeScene;
}

void TabGraph::Renderer::Impl::SetActiveRenderBuffer(const RenderBuffer::Handle& a_RenderBuffer)
{
    if (a_RenderBuffer == activeRenderBuffer)
        return;
    activeRenderBuffer = a_RenderBuffer;
    if (activeRenderBuffer == nullptr)
        return;
    auto renderBuffer = *activeRenderBuffer;
    auto fwdFBSize    = fwdRenderPass->info.frameBufferState.framebuffer->info.defaultSize;
    if (fwdFBSize.x < renderBuffer->width || fwdFBSize.y < renderBuffer->height) {
        glm::vec3 bgColor(0);
        if (activeScene != nullptr)
            bgColor = activeScene->GetBackgroundColor();
        fwdFB = CreateFwdFB(context, fwdFBSize * 2u);
    }
    UpdatePresentPass();
}

void Impl::LoadMesh(
    const ECS::DefaultRegistry::EntityRefType& a_Entity,
    const SG::Component::Mesh& a_Mesh,
    const SG::Component::Transform& a_Transform)
{
    Component::PrimitiveList primitiveList;
    for (auto& primitiveMaterial : a_Mesh.primitives) {
        auto& primitive  = primitiveMaterial.first;
        auto& material   = primitiveMaterial.second;
        auto& rPrimitive = primitiveCache.GetOrCreate(primitive.get(),
            Tools::LazyConstructor(
                [this, &primitive]() {
                    return std::make_shared<Primitive>(context, *primitive);
                }));
        auto rMaterial   = materialLoader.Load(*this, material.get());
        primitiveList.push_back(Component::PrimitiveKey { rPrimitive, rMaterial });
    }
    GLSL::Transform transform;
    transform.modelMatrix  = a_Mesh.geometryTransform * SG::Node::GetWorldTransformMatrix(a_Entity);
    transform.normalMatrix = glm::inverseTranspose(glm::mat3(transform.modelMatrix));
    a_Entity.AddComponent<Component::Transform>(context, transform);
    a_Entity.AddComponent<Component::PrimitiveList>(primitiveList);
}

std::shared_ptr<RAII::Texture> Impl::LoadTexture(SG::Image* a_Image)
{
    return textureLoader(context, a_Image);
}

std::shared_ptr<RAII::Sampler> Impl::LoadSampler(SG::TextureSampler* a_Sampler)
{
    return samplerLoader(context, a_Sampler);
}

void Load(
    const Handle& a_Renderer,
    const SG::Scene& a_Scene)
{
    auto& registry = a_Scene.GetRegistry();
    auto view      = registry->GetView<SG::Component::Mesh, SG::Component::Transform>(ECS::Exclude<Component::PrimitiveList, Component::Transform> {});
    for (const auto& [entityID, mesh, transform] : view) {
        a_Renderer->LoadMesh(registry->GetEntityRef(entityID), mesh, transform);
    }
    a_Renderer->context.ExecuteCmds();
}

void Load(
    const Handle& a_Renderer,
    const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
    if (a_Entity.template HasComponent<SG::Component::Mesh>() && a_Entity.template HasComponent<SG::Component::Transform>()) {
        const auto& mesh      = a_Entity.template GetComponent<SG::Component::Mesh>();
        const auto& transform = a_Entity.template GetComponent<SG::Component::Transform>();
        a_Renderer->LoadMesh(a_Entity, mesh, transform);
    }
    a_Renderer->context.ExecuteCmds();
}

void Unload(
    const Handle& a_Renderer,
    const SG::Scene& a_Scene)
{
    auto& renderer = *a_Renderer;
    // wait for rendering to be done
    auto& registry = a_Scene.GetRegistry();
    auto view      = registry->GetView<SG::Component::Mesh, Component::PrimitiveList, Component::Transform>();
    for (const auto& [entityID, mesh, primList, transform] : view) {
        registry->RemoveComponent<Component::PrimitiveList>(entityID);
        for (auto& [primitive, material] : mesh.primitives) {
            if (renderer.primitiveCache.at(primitive.get()).use_count() == 1)
                renderer.primitiveCache.erase(primitive.get());
        }
    }
}

void Unload(
    const Handle& a_Renderer,
    const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
    auto& renderer = *a_Renderer;
    if (a_Entity.template HasComponent<Component::PrimitiveList>())
        a_Entity.RemoveComponent<Component::PrimitiveList>();
    if (a_Entity.template HasComponent<Component::Transform>())
        a_Entity.RemoveComponent<Component::Transform>();
    if (a_Entity.template HasComponent<SG::Component::Mesh>()) {
        auto& mesh = a_Entity.template GetComponent<SG::Component::Mesh>();
        for (auto& [primitive, material] : mesh.primitives) {
            if (renderer.primitiveCache.at(primitive.get()).use_count() == 1)
                renderer.primitiveCache.erase(primitive.get());
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
}
