#pragma once

#include <Renderer/Handles.hpp>

#include <Renderer/OCRA/Shader.hpp>

#include <OCRA/OCRA.hpp>

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
    OCRA::Descriptor::Pool::Handle descriptorPool;
    Shader defaultShader;
    UniformBuffer cameraMatrix { 0, 1, OCRA::ShaderStageFlagBits::AllGraphics, glm::mat4(1) };
    std::map<SG::Primitive*, std::shared_ptr<Primitive>> primitives;
    std::map<SG::Material*, std::shared_ptr<Material>> materials;
};
}
