#include <Renderer/OGL/Components/LightData.hpp>
#include <Renderer/OGL/Components/MeshData.hpp>
#include <Renderer/OGL/Components/Transform.hpp>
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

#include <Bindings.glsl>
#include <Camera.glsl>
#include <Material.glsl>
#include <Transform.glsl>

#include <SG/Component/Camera.hpp>
#include <SG/Component/Light/PunctualLight.hpp>
#include <SG/Component/Mesh.hpp>
#include <SG/Core/Buffer/Buffer.hpp>
#include <SG/Core/Buffer/View.hpp>
#include <SG/Core/Image/Image2D.hpp>
#include <SG/Core/Texture/Sampler.hpp>
#include <SG/Core/Texture/Texture.hpp>
#include <SG/Entity/Camera.hpp>
#include <SG/Entity/Node.hpp>
#include <SG/Scene/Scene.hpp>

#include <Tools/BRDFIntegration.hpp>
#include <Tools/LazyConstructor.hpp>

#ifdef _WIN32
#ifdef IN
#undef IN
#endif // IN
#include <GL/wglew.h>
#include <Renderer/OGL/Win32/Context.hpp>
#include <Renderer/OGL/Win32/Error.hpp>
#include <Renderer/OGL/Win32/Window.hpp>
#elif defined __linux__
#include <GL/glew.h>
#include <Renderer/OGL/Unix/Context.hpp>
#endif //_WIN32

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <unordered_set>

namespace TabGraph::Renderer {
Impl::Impl(const CreateRendererInfo& a_Info, const RendererSettings& a_Settings)
#ifdef _WIN32
    : version(a_Info.applicationVersion)
    , name(a_Info.name)
    , shaderCompiler(context)
    , cameraUBO(UniformBufferT<GLSL::Camera>(context))
#elif defined __linux__
    : context(a_Info.display, nullptr, 64)
    , version(a_Info.applicationVersion)
    , name(a_Info.name)
    , shaderCompiler(context)
    , cameraUBO(UniformBufferT<GLSL::Camera>(context))
#endif //_WIN32
{
    shaderCompiler.PrecompileLibrary();
    {
        static SG::Sampler sampler;
        sampler.SetWrapS(SG::Sampler::Wrap::ClampToEdge);
        sampler.SetWrapT(SG::Sampler::Wrap::ClampToEdge);
        sampler.SetWrapR(SG::Sampler::Wrap::ClampToEdge);
        BrdfLutSampler = LoadSampler(&sampler);
    }
    {
        static SG::Sampler sampler;
        sampler.SetMinFilter(SG::Sampler::Filter::LinearMipmapLinear);
        IblSpecSampler = LoadSampler(&sampler);
    }
    glm::uvec3 LUTSize               = { 256, 256, 1 };
    SG::Pixel::Description pixelDesc = SG::Pixel::SizedFormat::Uint8_NormalizedRGBA;
    auto brdfLutImage                = std::make_shared<SG::Image2D>(pixelDesc, LUTSize.x, LUTSize.y, std::make_shared<SG::BufferView>(0, LUTSize.x * LUTSize.y * LUTSize.z * pixelDesc.GetSize()));
    auto brdfLutTexture              = SG::Texture(SG::TextureType::Texture2D, brdfLutImage);
    auto brdfIntegration             = Tools::BRDFIntegration::Generate(256, 256, Tools::BRDFIntegration::Type::Standard);
    for (uint32_t x = 0; x < LUTSize.x; ++x) {
        for (uint32_t y = 0; y < LUTSize.y; ++y) {
            for (uint32_t z = 0; z < LUTSize.z; ++z) {
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
        [path = path]() {
            path->Execute();
        });
    context.ExecuteCmds(context.Busy());
}

void Impl::Update()
{
    // return quietly
    if (activeScene == nullptr || activeRenderBuffer == nullptr)
        return;

    // DO CULLING
    UpdateCamera();
    UpdateTransforms();
    UpdateMeshes();
    lightCuller(activeScene);
    path->Update(*this);

    // UPDATE BUFFERS
    context.PushCmd([uboToUpdate = std::move(uboToUpdate)]() mutable {
        for (auto const& ubo : uboToUpdate)
            ubo();
    });

    // EXECUTE COMMANDS
    context.ExecuteCmds();
}

void Impl::UpdateMeshes()
{
    auto view = activeScene->GetRegistry()->GetView<SG::Component::Mesh>();
    std::unordered_set<std::shared_ptr<SG::Material>> SGMaterials;
    for (const auto& [entityID, sgMesh] : view) {
        for (auto& [primitive, material] : sgMesh.primitives) {
            SGMaterials.insert(material);
        }
    }
    for (auto& SGMaterial : SGMaterials) {
        auto material = materialLoader.Update(*this, SGMaterial.get());
        if (material->needsUpdate)
            uboToUpdate.emplace_back(*material);
    }
}

void Impl::UpdateTransforms()
{
    {
        auto view = activeScene->GetRegistry()->GetView<Component::Transform>(ECS::Exclude<SG::Component::Mesh> {});
        for (const auto& [entityID, rTransform] : view) {
            auto entityRef = activeScene->GetRegistry()->GetEntityRef(entityID);
            GLSL::Transform transform;
            transform.modelMatrix  = SG::Node::GetWorldTransformMatrix(entityRef);
            transform.normalMatrix = glm::inverseTranspose(transform.modelMatrix);
            rTransform.SetData(transform);
            if (rTransform.needsUpdate)
                uboToUpdate.emplace_back(rTransform);
        }
    }
    {
        auto view = activeScene->GetRegistry()->GetView<Component::Transform, SG::Component::Mesh>();
        for (const auto& [entityID, rTransform, sgMesh] : view) {
            auto entityRef = activeScene->GetRegistry()->GetEntityRef(entityID);
            GLSL::Transform transform;
            transform.modelMatrix  = sgMesh.geometryTransform * SG::Node::GetWorldTransformMatrix(entityRef);
            transform.normalMatrix = glm::inverseTranspose(transform.modelMatrix);
            rTransform.SetData(transform);
            if (rTransform.needsUpdate)
                uboToUpdate.emplace_back(rTransform);
        }
    }
}

void Impl::UpdateCamera()
{
    auto currentCamera = activeScene->GetCamera();
    GLSL::Camera cameraUBOData {};
    cameraUBOData.position   = SG::Node::GetWorldPosition(currentCamera);
    cameraUBOData.projection = currentCamera.GetComponent<SG::Component::Camera>().projection.GetMatrix();
    cameraUBOData.view       = glm::inverse(SG::Node::GetWorldTransformMatrix(currentCamera));
    cameraUBO.SetData(cameraUBOData);
    if (cameraUBO.needsUpdate)
        uboToUpdate.emplace_back(cameraUBO);
}

std::shared_ptr<Material> Impl::LoadMaterial(SG::Material* a_Material)
{
    return std::shared_ptr<Material>();
}

void TabGraph::Renderer::Impl::SetActiveRenderBuffer(const RenderBuffer::Handle& a_RenderBuffer)
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

std::shared_ptr<RAII::Texture> Impl::LoadTexture(SG::Texture* a_Texture)
{
    return textureLoader(context, a_Texture);
}

std::shared_ptr<RAII::Sampler> Impl::LoadSampler(SG::Sampler* a_Sampler)
{
    return samplerLoader(context, a_Sampler);
}

void Load(
    const Handle& a_Renderer,
    const SG::Scene& a_Scene)
{
    auto& registry = a_Scene.GetRegistry();
    auto meshView  = registry->GetView<SG::Component::Mesh, SG::Component::Transform>(ECS::Exclude<Component::PrimitiveList, Component::Transform> {});
    auto lightView = registry->GetView<SG::Component::PunctualLight>(ECS::Exclude<Component::LightData> {});
    for (const auto& [entityID, mesh, transform] : meshView) {
        a_Renderer->LoadMesh(registry->GetEntityRef(entityID), mesh, transform);
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

void SetActiveScene(const Handle& a_Renderer, SG::Scene* const a_Scene)
{
    a_Renderer->activeScene = a_Scene;
}

SG::Scene* GetActiveScene(const Handle& a_Renderer)
{
    return a_Renderer->activeScene;
}
}
