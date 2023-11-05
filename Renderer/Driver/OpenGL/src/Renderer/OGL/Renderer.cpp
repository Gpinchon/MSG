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

#include <Renderer/OGL/GLSL/CameraUBO.hpp>
#include <Renderer/OGL/GLSL/TransformUBO.hpp>

#include <Renderer/Structs.hpp>
#include <SG/Component/Camera.hpp>
#include <SG/Component/Mesh.hpp>
#include <SG/Core/Image/Image.hpp>
#include <SG/Core/Texture/Texture.hpp>
#include <SG/Entity/Node.hpp>
#include <SG/Scene/Scene.hpp>
#include <Tools/LazyConstructor.hpp>

#ifdef _WIN32
#include <Renderer/OGL/Win32/Context.hpp>
#include <Renderer/OGL/Win32/Error.hpp>
#include <Renderer/OGL/Win32/Window.hpp>
#endif

#include <GL/wglew.h>

#include <cstdlib>
#include <stdexcept>
#include <unordered_set>

namespace TabGraph::Renderer {
auto s_ForwardVertexCode = "                            \n\
#version 450                                            \n\
out gl_PerVertex                                        \n\
{                                                       \n\
    vec4 gl_Position;                                   \n\
};                                                      \n\
struct TransformUBO {                                   \n\
    //vec3 position;                                      \n\
    //vec3 scale;                                         \n\
    //vec4 rotation;                                      \n\
    mat4 matrix;                                     \n\
};                                                      \n\
layout(binding = 0) uniform CameraBlock {                 \n\
    //TransformUBO transform;                             \n\
    mat4 projection;                                    \n\
    mat4 view;                                          \n\
} u_Camera;                                             \n\
layout (binding = 1) uniform TransformBlock {           \n\
    TransformUBO u_ModelTransform;                      \n\
};                                                      \n\
layout(location = 0) in vec3	in_Position;            \n\
void main() {                                           \n\
    vec4 worldPos = u_ModelTransform.matrix * vec4(in_Position, 1);\n\
    gl_Position = u_Camera.projection * u_Camera.view * worldPos; \n\
}                                                       \n\
";

auto s_ForwardFragmentCode = "              \n\
#include <MaterialUBO.glsl>                 \n\
void main() {                               \n\
    gl_FragColor = vec4(1);                 \n\
}                                           \n\
";

auto CompileForwardShaders(ShaderCompiler& a_ShaderCompiler)
{
    std::vector<RAII::Shader*> forwardShaders;
    forwardShaders.push_back(&a_ShaderCompiler.CompileShader(GL_VERTEX_SHADER, s_ForwardVertexCode));
    forwardShaders.push_back(&a_ShaderCompiler.CompileShader(GL_FRAGMENT_SHADER, s_ForwardFragmentCode));
    return RAII::MakePtr<RAII::Program>(a_ShaderCompiler.context, forwardShaders);
}

auto CreateForwardFrameBuffer(Renderer::Impl& a_Renderer, uint32_t a_Width, uint32_t a_Height)
{
    auto& context = a_Renderer.context;
    FrameBufferState frameBufferState;
    frameBufferState.frameBuffer = RAII::MakePtr<RAII::FrameBuffer>(context, a_Width, a_Height);
    frameBufferState.colorBuffers.push_back(
        RAII::MakePtr<RAII::Texture2D>(
            context, a_Width, a_Height, 1, GL_RGB8));
    frameBufferState.depthBuffer = RAII::MakePtr<RAII::Texture2D>(
        context, a_Width, a_Height, 1, GL_DEPTH_COMPONENT24);
    frameBufferState.clearColors = { { 0, { 1, 0, 0, 1 } } };
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

auto CreateForwardShader(Renderer::Impl& a_Renderer)
{
    ShaderState shaderState;
    shaderState.pipeline = RAII::MakePtr<RAII::ProgramPipeline>(a_Renderer.context);
    shaderState.program  = CompileForwardShaders(a_Renderer.shaderCompiler);
    shaderState.stages   = GL_VERTEX_SHADER | GL_FRAGMENT_SHADER;
    return shaderState;
}

Impl::Impl(const CreateRendererInfo& a_Info)
    : name(a_Info.name)
    , version(a_Info.applicationVersion)
    , shaderCompiler(context)
    , forwardFrameBuffer(CreateForwardFrameBuffer(*this, 2048, 2048))
    , forwardShader(CreateForwardShader(*this))
    , forwardCameraUBO(UniformBufferT<GLSL::CameraUBO>(context))
{
}

void Impl::Render()
{
    // return quietly
    if (activeScene == nullptr || activeRenderBuffer == nullptr) {
        return;
    }
    context.PushCmd(
        [renderPasses          = renderPasses,
            activeRenderBuffer = activeRenderBuffer,
            forwardFrameBuffer = forwardFrameBuffer]() {
            for (auto& renderPass : renderPasses) {
                ExecuteRenderPass(renderPass);
            }
            auto& dstImage             = *activeRenderBuffer;
            auto& srcImage             = forwardFrameBuffer.colorBuffers.front();
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
    virtual void operator()() = 0;
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
    void operator()()
    {
        glNamedBufferSubData(
            *_buffer, _offset,
            _size, _data.get());
    }

private:
    std::shared_ptr<RAII::Buffer> _buffer;
    const uint32_t _size = 0, _offset = 0;
    std::shared_ptr<void> _data;
};

void Impl::Update()
{
    // return quietly
    if (activeScene == nullptr || activeRenderBuffer == nullptr)
        return;
    auto& renderBuffer = *activeRenderBuffer;
    // Update forward pass
    if (forwardFrameBuffer.frameBuffer->width < renderBuffer->width
        || forwardFrameBuffer.frameBuffer->height < renderBuffer->height) {
        // Recreate framebuffer
        auto newWidth      = std::max(forwardFrameBuffer.frameBuffer->width, renderBuffer->width);
        auto newHeight     = std::max(forwardFrameBuffer.frameBuffer->height, renderBuffer->height);
        forwardFrameBuffer = CreateForwardFrameBuffer(*this, newWidth, newHeight);
    }
    RenderPassInfo forwardRenderPass;
    forwardRenderPass.name                        = "Forward";
    forwardRenderPass.UBOs                        = { { 0, forwardCameraUBO.buffer } };
    forwardRenderPass.frameBufferState            = forwardFrameBuffer;
    forwardRenderPass.viewportState.viewport      = { renderBuffer->width, renderBuffer->height };
    forwardRenderPass.viewportState.scissorExtent = forwardRenderPass.viewportState.viewport;
    forwardRenderPass.graphicsPipelines.clear();
    std::vector<UniformBufferUpdate> uboToUpdate;
    std::unordered_set<std::shared_ptr<SG::Material>> SGMaterials;
    auto view = activeScene->GetRegistry()->GetView<Component::PrimitiveList, Component::Transform, SG::Component::Mesh, SG::Component::Transform>();
    for (auto& [entityID, rPrimitives, rTransform, sgMesh, sgTransform] : view) {
        auto entityRef = activeScene->GetRegistry()->GetEntityRef(entityID);
        rTransform.SetData(SG::Node::GetWorldTransformMatrix(entityRef));
        for (auto& primitive : sgMesh.primitives) {
            SGMaterials.insert(primitive.second);
        }
        if (rTransform.needsUpdate)
            uboToUpdate.push_back(rTransform);
        for (auto& primitiveKey : rPrimitives) {
            auto& primitive                  = primitiveKey.first;
            auto& material                   = primitiveKey.second;
            auto& graphicsPipelineInfo       = forwardRenderPass.graphicsPipelines.emplace_back();
            graphicsPipelineInfo.UBOs        = { { 1, rTransform.buffer } };
            graphicsPipelineInfo.shaderState = forwardShader;
            primitive->FillGraphicsPipelineInfo(graphicsPipelineInfo);
        }
    }
    for (auto& SGMaterial : SGMaterials) {
        auto material = materialLoader.Update(*this, SGMaterial.get());
        if (material->needsUpdate)
            uboToUpdate.push_back(*material);
    }
    {
        auto cameraProj = activeScene->GetCamera().template GetComponent<SG::Component::Camera>().projection.GetMatrix();
        auto cameraView = glm::inverse(SG::Node::GetWorldTransformMatrix(activeScene->GetCamera()));
        GLSL::CameraUBO cameraUBOData {};
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
    a_Renderer->activeRenderBuffer = a_RenderBuffer;
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
        primitiveList.push_back({ rPrimitive, rMaterial });
    }
    glm::mat4 transform = a_Mesh.geometryTransform * SG::Node::GetWorldTransformMatrix(a_Entity);
    a_Entity.template AddComponent<Component::Transform>(context, transform);
    a_Entity.template AddComponent<Component::PrimitiveList>(primitiveList);
}

std::shared_ptr<RAII::TextureSampler> Impl::LoadTextureSampler(SG::Texture* a_Texture)
{
    return textureSamplerLoader.Load(context,
        textureLoader(context, a_Texture->GetImage().get()),
        samplerLoader(context, a_Texture->GetSampler().get()));
}

void Load(
    const Handle& a_Renderer,
    const SG::Scene& a_Scene)
{
    auto& registry = a_Scene.GetRegistry();
    auto view      = registry->GetView<SG::Component::Mesh, SG::Component::Transform>(ECS::Exclude<Component::PrimitiveList, Component::Transform> {});
    for (auto& [entityID, mesh, transform] : view) {
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
    for (auto& [entityID, mesh, primList, transform] : view) {
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
