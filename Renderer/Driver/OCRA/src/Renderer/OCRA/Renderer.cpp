#include <Renderer/OCRA/Component/MeshData.hpp>
#include <Renderer/OCRA/Primitive.hpp>
#include <Renderer/OCRA/RenderBuffer.hpp>
#include <Renderer/OCRA/Renderer.hpp>
#include <Renderer/Renderer.hpp>

#include <SG/Component/Mesh.hpp>
#include <SG/Scene/Scene.hpp>

#include <OCRA/OCRA.hpp>
#include <OCRA/ShaderCompiler/Compiler.hpp>

namespace TabGraph::Renderer {
Handle Create(const CreateRendererInfo& a_Info)
{
    OCRA::CreateInstanceInfo info;
    info.name               = a_Info.name;
    info.applicationVersion = info.applicationVersion;
    info.engineVersion      = 100;
    info.engineName         = "TabGraph";
    return Handle(new Impl(info));
}

inline std::vector<OCRA::QueueInfo> GetQueueInfos(const OCRA::PhysicalDevice::Handle& a_PhysicalDevice)
{
    std::vector<OCRA::QueueInfo> queueInfos;
    auto& queueFamilies  = OCRA::PhysicalDevice::GetQueueFamilyProperties(a_PhysicalDevice);
    uint32_t familyIndex = 0;
    for (auto& queueFamily : queueFamilies) {
        OCRA::QueueInfo queueInfo;
        queueInfo.queueCount       = queueFamily.queueCount;
        queueInfo.queueFamilyIndex = familyIndex;
        queueInfo.queuePriorities.resize(queueFamily.queueCount, 1.f);
        queueInfos.push_back(queueInfo);
        ++familyIndex;
    }
    return queueInfos;
}

inline OCRA::Device::Handle CreateDevice(const OCRA::PhysicalDevice::Handle& a_PhysicalDevice)
{
    OCRA::CreateDeviceInfo deviceInfo;
    deviceInfo.queueInfos = GetQueueInfos(a_PhysicalDevice);
    return OCRA::PhysicalDevice::CreateDevice(a_PhysicalDevice, deviceInfo);
}

inline OCRA::Queue::Handle GetQueue(const OCRA::PhysicalDevice::Handle& a_PhysicalDevice, const OCRA::Device::Handle& a_Device)
{
    const auto queueFamily = OCRA::PhysicalDevice::FindQueueFamily(a_PhysicalDevice, OCRA::QueueFlagBits::Graphics);
    return OCRA::Device::GetQueue(a_Device, queueFamily, 0); // Get first available queue
}

void Load(
    const Handle& a_Renderer,
    const SG::Scene& a_Scene)
{
    a_Renderer->Load(a_Scene);
}

void Render(
    const Handle& a_Renderer,
    const SG::Scene& a_Scene,
    const RenderBuffer::Handle& a_Buffer)
{
    a_Renderer->Render(a_Scene, a_Buffer);
}

void Update(const Handle& a_Renderer)
{
    a_Renderer->Update();
}

static inline auto DefaultVertexShader()
{
    Renderer::Shader::Stage shaderStage;
    shaderStage.type       = OCRA::ShaderCompiler::ShaderType::Vertex;
    shaderStage.entryPoint = "main";
    shaderStage.source     = {
        "#version 450                                                   \n"
            "layout(binding = 0) uniform Transforms {                       \n"
            "   mat4 matrix;                                                \n"
            "} in_Transforms;                                               \n"
            "                                                               \n"
            "layout(location = 0) in vec3  in_Position;                     \n"
            "layout(location = 1) in vec3  in_Normal;                       \n"
            "layout(location = 2) in vec4  in_Tangent;                      \n"
            "layout(location = 3) in vec2  in_TexCoord_0;                   \n"
            "layout(location = 4) in vec2  in_TexCoord_1;                   \n"
            "layout(location = 5) in vec2  in_TexCoord_2;                   \n"
            "layout(location = 6) in vec2  in_TexCoord_3;                   \n"
            "layout(location = 7) in vec3  in_Color;                        \n"
            "layout(location = 8) in uvec4 in_Joints;                       \n"
            "layout(location = 9) in vec4  in_Weights;                      \n"
            "                                                               \n"
            "layout(location = 0) out vec3 vert_Color;                      \n"
            "layout(location = 1) out vec2 vert_TexCoord_0;                 \n"
            "                                                               \n"
            "void main() {                                                  \n"
            "   gl_Position = vec4(in_Position, 1.0);\n"
            "   vert_Color = in_Color;                                      \n"
            "   vert_TexCoord_0 = in_TexCoord_0;                            \n"
            "}                                                              \n"
    };
    return shaderStage;
}

static inline auto DefaultFragmentShader()
{
    Renderer::Shader::Stage shaderStage;
    shaderStage.type       = OCRA::ShaderCompiler::ShaderType::Fragment;
    shaderStage.entryPoint = "main";
    shaderStage.source     = {
        "#version 450                                                  \n"
            "layout(location = 0) in vec3 vert_Color;                      \n"
            "layout(location = 1) in vec2 vert_TexCoord;                   \n"
            "                                                              \n"
            "layout(location = 0) out vec3 frag_Color;                     \n"
            "                                                              \n"
            "void main() {                                                 \n"
            "   frag_Color = vert_Color;                                   \n"
            "}                                                             \n"
    };
    return shaderStage;
}

Impl::Impl(const OCRA::CreateInstanceInfo& a_Info)
    : instance(OCRA::CreateInstance(a_Info))
    , physicalDevice(OCRA::Instance::EnumeratePhysicalDevices(instance).front())
    , logicalDevice(CreateDevice(physicalDevice))
    , queue(GetQueue(physicalDevice, logicalDevice))
    , shaderCompiler(OCRA::ShaderCompiler::Create())
{
    OCRA::CreateCommandPoolInfo poolInfo;
    poolInfo.flags            = OCRA::CreateCommandPoolFlagBits::Reset;
    poolInfo.queueFamilyIndex = OCRA::PhysicalDevice::FindQueueFamily(physicalDevice, OCRA::QueueFlagBits::Graphics);
    commandPool               = OCRA::Device::CreateCommandPool(logicalDevice, poolInfo);
    OCRA::AllocateCommandBufferInfo commandBufferInfo;
    commandBufferInfo.count = 1;
    commandBufferInfo.level = OCRA::CommandBufferLevel::Primary;
    commandBuffer           = OCRA::Command::Pool::AllocateCommandBuffer(commandPool, commandBufferInfo).front();
    OCRA::CreateDescriptorPoolInfo descriptorPoolInfo;
    descriptorPoolInfo.maxSets = 4096;
    descriptorPoolInfo.sizes.reserve(1024);
    {
        OCRA::DescriptorPoolSize descriptorPoolSize;
        descriptorPoolSize.type  = OCRA::DescriptorType::UniformBuffer;
        descriptorPoolSize.count = 1024;
        descriptorPoolInfo.sizes.push_back(descriptorPoolSize);
    }
    descriptorPool = OCRA::Device::CreateDescriptorPool(logicalDevice, descriptorPoolInfo);
    defaultShader  = { *this, { DefaultVertexShader(), DefaultFragmentShader() } };
}

Impl::~Impl()
{
    OCRA::Queue::WaitIdle(queue);
    OCRA::Command::Buffer::Reset(commandBuffer);
}

void Impl::Load(const SG::Scene& a_Scene)
{
    auto& registry = a_Scene.GetRegistry();
    auto view      = registry->GetView<SG::Component::Mesh>(ECS::Exclude<Component::MeshData> {});
    view.ForEach<SG::Component::Mesh>([renderer = this, registry](auto entityID, const auto& mesh) {
        registry->AddComponent<Component::MeshData>(entityID, renderer, mesh);
    });
}

void Impl::Render(const SG::Scene& a_Scene, const RenderBuffer::Handle& a_Buffer)
{
    auto& registry = a_Scene.GetRegistry();
    auto view      = registry->GetView<Component::MeshData>();
    OCRA::CommandBufferBeginInfo beginInfo;
    beginInfo.flags = OCRA::CommandBufferUsageFlagBits::OneTimeSubmit;
    OCRA::Command::Buffer::Begin(commandBuffer, beginInfo);
    {
        OCRA::ImageLayoutTransitionInfo renderTargetTransition;
        renderTargetTransition.image            = a_Buffer->image;
        renderTargetTransition.subRange.aspects = OCRA::ImageAspectFlagBits::Color;
        renderTargetTransition.oldLayout        = OCRA::ImageLayout::Undefined;
        renderTargetTransition.newLayout        = OCRA::ImageLayout::ColorAttachmentOptimal;
        OCRA::Command::TransitionImageLayout(commandBuffer, renderTargetTransition);
    }
    {
        OCRA::RenderingInfo renderingInfo;
        OCRA::RenderingAttachmentInfo colorAttachment;
        colorAttachment.clearValue  = OCRA::ColorValue(1.f, 0.f, 0.f, 1.f);
        colorAttachment.imageView   = a_Buffer->imageView;
        colorAttachment.imageLayout = OCRA::ImageLayout::ColorAttachmentOptimal;
        colorAttachment.loadOp      = OCRA::LoadOp::Clear;
        colorAttachment.storeOp     = OCRA::StoreOp::Store;
        renderingInfo.area.offset   = { 0, 0 };
        renderingInfo.area.extent   = { a_Buffer->extent.width, a_Buffer->extent.height };
        renderingInfo.colorAttachments.push_back(colorAttachment);
        renderingInfo.layerCount = 1;
        {
            OCRA::Command::BeginRendering(commandBuffer, renderingInfo);
            view.ForEach<Component::MeshData>(
                [this,
                    &a_Buffer](const Component::MeshData& meshData) {
                    for (auto& pipeline : meshData.graphicsPipelines) {
                        OCRA::Command::BindPipeline(commandBuffer, pipeline);
                        OCRA::Command::BindDescriptorSet(commandBuffer, pipeline, defaultShader.GetDescriptorSet());
                        for (auto& primitive : meshData.primitives) {
                            OCRA::Command::BindVertexBuffers(commandBuffer,
                                0, // first binding
                                { primitive->vertexBuffer.GetBuffer() },
                                { primitive->vertexBuffer.GetOffset() });
                            if (primitive->indexBuffer.GetSize() == 0) {
                                OCRA::Command::Draw(commandBuffer,
                                    primitive->vertexBuffer.GetCount(),
                                    1, 0, 0);
                            } else {
                                OCRA::Command::BindIndexBuffer(commandBuffer,
                                    primitive->indexBuffer.GetBuffer(),
                                    primitive->indexBuffer.GetOffset(),
                                    OCRA::IndexType::Uint32);
                                OCRA::Command::DrawIndexed(commandBuffer,
                                    primitive->indexBuffer.GetCount(),
                                    1, 0, 0, 0);
                            }
                        }
                    }
                });
        }
        OCRA::Command::EndRendering(commandBuffer);
    }
    {
        OCRA::ImageLayoutTransitionInfo renderTargetTransition;
        renderTargetTransition.image            = a_Buffer->image;
        renderTargetTransition.subRange.aspects = OCRA::ImageAspectFlagBits::Color;
        renderTargetTransition.oldLayout        = OCRA::ImageLayout::ColorAttachmentOptimal;
        renderTargetTransition.newLayout        = OCRA::ImageLayout::General;
        OCRA::Command::TransitionImageLayout(
            commandBuffer, renderTargetTransition);
    }
    OCRA::Command::Buffer::End(commandBuffer);
    OCRA::QueueSubmitInfo submitInfo;
    submitInfo.commandBuffers = { commandBuffer };
    OCRA::Queue::Submit(queue, { submitInfo });
    OCRA::Queue::WaitIdle(queue);
}
void Impl::Update()
{
    {
        std::vector<decltype(primitives)::key_type> toDelete;
        for (auto& primitive : primitives) {
            if (primitive.second.use_count() == 1) // nobody else uses that
                toDelete.push_back(primitive.first);
        }
        for (const auto& primitive : toDelete)
            primitives.erase(primitive);
    }
    {
        std::vector<decltype(materials)::key_type> toDelete;
        for (auto& primitive : materials) {
            if (primitive.second.use_count() == 1) // nobody else uses that
                toDelete.push_back(primitive.first);
        }
        for (const auto& primitive : toDelete)
            materials.erase(primitive);
    }
}
}
