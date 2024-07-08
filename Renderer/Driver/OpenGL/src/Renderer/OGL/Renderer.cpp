#include <Renderer/OGL/Components/MeshData.hpp>
#include <Renderer/OGL/Components/Transform.hpp>
#include <Renderer/OGL/Primitive.hpp>
#include <Renderer/OGL/RAII/Buffer.hpp>
#include <Renderer/OGL/RAII/DebugGroup.hpp>
#include <Renderer/OGL/RAII/FrameBuffer.hpp>
#include <Renderer/OGL/RAII/Program.hpp>
#include <Renderer/OGL/RAII/ProgramPipeline.hpp>
#include <Renderer/OGL/RAII/VertexArray.hpp>
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
auto CreateForwardFrameBuffer(
    Renderer::Impl& a_Renderer,
    uint32_t a_Width, uint32_t a_Height,
    const glm::vec3& a_ClearColor = { 0, 0, 0 })
{
    auto& context = a_Renderer.context;
    FrameBufferState frameBufferState;
    frameBufferState.frameBuffer = RAII::MakePtr<RAII::FrameBuffer>(context, a_Width, a_Height);
    frameBufferState.colorBuffers.push_back(
        RAII::MakePtr<RAII::Texture2D>(
            context, a_Width, a_Height, 1, GL_RGB8));
    frameBufferState.depthBuffer = RAII::MakePtr<RAII::Texture2D>(
        context, a_Width, a_Height, 1, GL_DEPTH_COMPONENT24);
    frameBufferState.clearColors = { { 0, { a_ClearColor, 1 } } };
    frameBufferState.clearDepth  = 1.f;
    context.PushCmd(
        [frameBufferState = frameBufferState] {
            glNamedFramebufferTexture(
                *frameBufferState.frameBuffer,
                GL_COLOR_ATTACHMENT0, *frameBufferState.colorBuffers.front(), 0);
            glNamedFramebufferTexture(
                *frameBufferState.frameBuffer,
                GL_DEPTH_ATTACHMENT, *frameBufferState.depthBuffer, 0);
            constexpr std::array<GLenum, 1> drawArrays { GL_COLOR_ATTACHMENT0 };
            glNamedFramebufferDrawBuffers(
                *frameBufferState.frameBuffer,
                1, drawArrays.data());
        });
    return frameBufferState;
}

auto CreateForwardLitShader(Renderer::Impl& a_Renderer, uint a_MaterialType)
{
    ShaderState shaderState;
    shaderState.pipeline = RAII::MakePtr<RAII::ProgramPipeline>(a_Renderer.context);
    if (a_MaterialType == MATERIAL_TYPE_SPECULAR_GLOSSINESS)
        shaderState.program = a_Renderer.shaderCompiler.CompileProgram("ForwardLitSpecGloss");
    else if (a_MaterialType == MATERIAL_TYPE_METALLIC_ROUGHNESS)
        shaderState.program = a_Renderer.shaderCompiler.CompileProgram("ForwardLitMetRough");
    shaderState.stages = GL_VERTEX_SHADER | GL_FRAGMENT_SHADER;
    return shaderState;
}

Impl::Impl(const CreateRendererInfo& a_Info)
    : context(a_Info.display, nullptr, 64)
    , version(a_Info.applicationVersion)
    , name(a_Info.name)
    , shaderCompiler(context)
    , forwardFrameBuffer(CreateForwardFrameBuffer(*this, 2048, 2048))
    , forwardLitMetRoughShader(CreateForwardLitShader(*this, MATERIAL_TYPE_METALLIC_ROUGHNESS))
    , forwardLitSpecGlossShader(CreateForwardLitShader(*this, MATERIAL_TYPE_SPECULAR_GLOSSINESS))
    , forwardCameraUBO(UniformBufferT<GLSL::Camera>(context))
{
}

void Impl::Render()
{
    // return quietly
    if (activeScene == nullptr || activeRenderBuffer == nullptr) {
        return;
    }
    context.PushCmd(
        [renderPasses = renderPasses,
            dstImage  = *activeRenderBuffer,
            srcImage  = forwardFrameBuffer.colorBuffers.front()]() {
            for (auto& renderPass : renderPasses) {
                ExecuteRenderPass(renderPass);
            }
            auto renderSceneDebugGroup = RAII::DebugGroup("Copy result to buffer");
            glCopyImageSubData(
                *srcImage, GL_TEXTURE_2D, 0,
                0, 0, 0,
                *dstImage, GL_TEXTURE_2D, 0,
                0, 0, 0,
                dstImage->width, dstImage->height, 1);
        });
    context.ExecuteCmds(context.Busy());
}

struct UniformBufferUpdateI {
    virtual ~UniformBufferUpdateI() = default;
    virtual void operator()()       = 0;
};

struct UniformBufferUpdate {
    template <typename T>
    UniformBufferUpdate(UniformBufferT<T>& a_UniformBuffer)
        : _buffer(a_UniformBuffer.buffer)
        , _size(sizeof(T))
        , _offset(a_UniformBuffer.offset)
        , _data(std::make_shared<T>(a_UniformBuffer.GetData()))

    {
        a_UniformBuffer.needsUpdate = false;
    }
    void operator()() const
    {
        glNamedBufferSubData(
            *_buffer, _offset,
            _size, _data.get());
    }

private:
    std::shared_ptr<RAII::Buffer> _buffer;
    const uint32_t _size   = 0;
    const uint32_t _offset = 0;
    std::shared_ptr<void> _data;
};

void Impl::Update()
{
    // return quietly
    if (activeScene == nullptr || activeRenderBuffer == nullptr)
        return;
    lightCuller(activeScene);
    auto& renderBuffer = *activeRenderBuffer;
    // Update forward pass
    RenderPassInfo forwardRenderPass;
    forwardRenderPass.name                        = "Forward";
    forwardRenderPass.frameBufferState            = forwardFrameBuffer;
    forwardRenderPass.viewportState.viewport      = { renderBuffer->width, renderBuffer->height };
    forwardRenderPass.viewportState.scissorExtent = forwardRenderPass.viewportState.viewport;
    forwardRenderPass.buffers                     = {
        { GL_UNIFORM_BUFFER, UBO_CAMERA, forwardCameraUBO.buffer, 0, forwardCameraUBO.buffer->size },
        { GL_SHADER_STORAGE_BUFFER, SSBO_VTFS_LIGHTS, lightCuller.GPUlightsBuffer, sizeof(int) * 4, lightCuller.GPUlightsBuffer->size },
        { GL_SHADER_STORAGE_BUFFER, SSBO_VTFS_CLUSTERS, lightCuller.GPUclusters, 0, lightCuller.GPUclusters->size }
    };
    forwardRenderPass.graphicsPipelines.clear();
    std::vector<UniformBufferUpdate> uboToUpdate;
    std::unordered_set<std::shared_ptr<SG::Material>> SGMaterials;
    auto view = activeScene->GetRegistry()->GetView<Component::PrimitiveList, Component::Transform, SG::Component::Mesh, SG::Component::Transform>();
    for (const auto& [entityID, rPrimitives, rTransform, sgMesh, sgTransform] : view) {
        auto entityRef = activeScene->GetRegistry()->GetEntityRef(entityID);
        GLSL::Transform transform;
        transform.modelMatrix  = sgMesh.geometryTransform * SG::Node::GetWorldTransformMatrix(entityRef);
        transform.normalMatrix = glm::inverseTranspose(transform.modelMatrix);
        rTransform.SetData(transform);
        for (auto& primitive : sgMesh.primitives) {
            SGMaterials.insert(primitive.second);
        }
        if (rTransform.needsUpdate)
            uboToUpdate.push_back(rTransform);
        for (auto& primitiveKey : rPrimitives) {
            auto& primitive            = primitiveKey.first;
            auto& material             = primitiveKey.second;
            auto& graphicsPipelineInfo = forwardRenderPass.graphicsPipelines.emplace_back();
            if (material->GetData().base.type == MATERIAL_TYPE_METALLIC_ROUGHNESS)
                graphicsPipelineInfo.shaderState = forwardLitMetRoughShader;
            else if (material->GetData().base.type == MATERIAL_TYPE_SPECULAR_GLOSSINESS)
                graphicsPipelineInfo.shaderState = forwardLitSpecGlossShader;
            graphicsPipelineInfo.buffers = {
                { GL_UNIFORM_BUFFER, UBO_TRANSFORM, rTransform.buffer, 0, rTransform.buffer->size },
                { GL_UNIFORM_BUFFER, UBO_MATERIAL, material->buffer, 0, material->buffer->size },
            };
            for (uint i = 0; i < material->textureSamplers.size(); ++i) {
                auto& textureSampler = material->textureSamplers.at(i);
                graphicsPipelineInfo.textures.push_back({ i, textureSampler.texture, textureSampler.sampler });
            }
            primitive->FillGraphicsPipelineInfo(graphicsPipelineInfo);
        }
    }
    for (auto& SGMaterial : SGMaterials) {
        auto material = materialLoader.Update(*this, SGMaterial.get());
        if (material->needsUpdate)
            uboToUpdate.push_back(*material);
    }
    {
        GLSL::Camera cameraUBOData {};
        cameraUBOData.projection = activeScene->GetCamera().template GetComponent<SG::Component::Camera>().projection.GetMatrix();
        cameraUBOData.view       = glm::inverse(SG::Node::GetWorldTransformMatrix(activeScene->GetCamera()));
        forwardCameraUBO.SetData(cameraUBOData);
        if (forwardCameraUBO.needsUpdate)
            uboToUpdate.push_back(forwardCameraUBO);
    }
    context.PushCmd([uboToUpdate = std::move(uboToUpdate)]() mutable {
        for (auto& ubo : uboToUpdate)
            ubo();
    });
    renderPasses.clear();
    renderPasses.push_back(forwardRenderPass);
    context.ExecuteCmds();
}

std::shared_ptr<Material> Impl::LoadMaterial(SG::Material* a_Material)
{
    return std::shared_ptr<Material>();
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
    if (a_Scene != nullptr) {
        a_Renderer->forwardFrameBuffer.clearColors.front().color = { a_Scene->GetBackgroundColor(), 1 };
    }
}

SG::Scene* GetActiveScene(const Handle& a_Renderer)
{
    return a_Renderer->activeScene;
}

void TabGraph::Renderer::Impl::SetActiveRenderBuffer(const RenderBuffer::Handle& a_RenderBuffer)
{
    activeRenderBuffer = a_RenderBuffer;
    if (activeRenderBuffer == nullptr)
        return;
    auto renderBuffer = *activeRenderBuffer;
    if (forwardFrameBuffer.frameBuffer->width < renderBuffer->width
        || forwardFrameBuffer.frameBuffer->height < renderBuffer->height) {
        // Recreate framebuffer
        auto newWidth      = std::max(forwardFrameBuffer.frameBuffer->width, renderBuffer->width);
        auto newHeight     = std::max(forwardFrameBuffer.frameBuffer->height, renderBuffer->height);
        forwardFrameBuffer = CreateForwardFrameBuffer(*this, newWidth, newHeight, forwardFrameBuffer.clearColors.front().color);
    }
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
    a_Entity.template AddComponent<Component::Transform>(context, transform);
    a_Entity.template AddComponent<Component::PrimitiveList>(primitiveList);
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
        auto& mesh      = a_Entity.template GetComponent<SG::Component::Mesh>();
        auto& transform = a_Entity.template GetComponent<SG::Component::Transform>();
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
