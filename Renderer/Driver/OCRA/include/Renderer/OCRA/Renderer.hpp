#pragma once

#include <Renderer/Handles.hpp>
#include <Renderer/OCRA/Renderer.hpp>

#include <OCRA/OCRA.hpp>
#include <OCRA/ShaderCompiler/Compiler.hpp>

#include <glm/glm.hpp>
#include <map>

namespace TabGraph::SG {
class Primitive;
class Material;
class Scene;
}

namespace TabGraph::Renderer {
struct Primitive;
struct Material;
struct Uniform {
    Uniform(
        const size_t& a_Binding,
        const OCRA::DescriptorType& a_Type,
        const size_t& a_Count,
        const OCRA::ShaderStageFlags& a_Stages)
    {
        binding.binding    = a_Binding;
        binding.type       = a_Type;
        binding.count      = a_Count;
        binding.stageFlags = a_Stages;
    }
    OCRA::DescriptorSetBinding binding;
    OCRA::DescriptorSetWrite descriptorSetWrite;
};
struct UniformBuffer : Uniform {
    template <typename T>
    UniformBuffer(
        const size_t& a_Binding,
        const size_t& a_Count,
        const OCRA::ShaderStageFlags& a_Stages,
        const T& a_DefaultValue)
        : Uniform(a_Binding, OCRA::DescriptorType::UniformBuffer, a_Count, a_Stages)
        , data((std::byte*)&a_DefaultValue, (std::byte*)&a_DefaultValue + sizeof(T))
    {
    }
    std::vector<std::byte> data;
};

struct Shader {
    struct Stage {
        OCRA::ShaderCompiler::ShaderType type;
        std::string entryPoint;
        std::string source;
    };
    Shader(const Renderer::Impl& a_Renderer, const std::vector<Stage>& a_Stages)
    {
        const auto& compiler = a_Renderer.shaderCompiler;
        OCRA::ShaderCompiler::ShaderInfo shaderInfo;
        if (OCRA_API_IMPL == OCRA_API_Vulkan)
            shaderInfo.targetAPI = OCRA::ShaderCompiler::TargetAPI::Vulkan;
        else if (OCRA_API_IMPL == OCRA_API_OpenGL)
            shaderInfo.targetAPI = OCRA::ShaderCompiler::TargetAPI::OpenGL;
        else if (OCRA_API_IMPL == OCRA_API_DirectX)
            shaderInfo.targetAPI = OCRA::ShaderCompiler::TargetAPI::DirectX;
        for (auto& stage : a_Stages) {
            shaderInfo.type       = stage.type;
            shaderInfo.entryPoint = stage.entryPoint;
            shaderInfo.source     = stage.source;
            OCRA::PipelineShaderStage shaderStageInfo;
            shaderStageInfo.entryPoint = shaderInfo.entryPoint;
            shaderStageInfo.stage      = OCRA::ShaderStageFlagBits::Vertex;
            shaderStageInfo.module     = CreateShaderModule(a_Renderer.logicalDevice, { OCRA::ShaderCompiler::Compile(compiler, shaderInfo).SPIRVBinary });
            stages.push_back(shaderStageInfo);
        }
    }
    std::vector<OCRA::PipelineShaderStage> stages;
};

struct Impl {
    Impl(const OCRA::CreateInstanceInfo& a_Info);
    ~Impl();
    void Load(const SG::Scene& a_Scene);
    void Render(
        const SG::Scene& a_Scene,
        const RenderBuffer::Handle& a_Buffer);
    void Update();
    OCRA::Instance::Handle instance;
    OCRA::PhysicalDevice::Handle physicalDevice;
    OCRA::Device::Handle logicalDevice;
    OCRA::Command::Pool::Handle commandPool;
    OCRA::Command::Buffer::Handle commandBuffer;
    OCRA::Queue::Handle queue;
    OCRA::ShaderCompiler::Handle shaderCompiler;
    Shader defaultShader;
    UniformBuffer cameraMatrix { 0, 1, OCRA::ShaderStageFlagBits::AllGraphics, glm::mat4(1) };
    std::map<SG::Primitive*, std::shared_ptr<Primitive>> primitives;
    std::map<SG::Material*, std::shared_ptr<Material>> materials;
};
}
