#pragma once

#include <Renderer/Handles.hpp>

#include <OCRA/OCRA.hpp>
#include <OCRA/ShaderCompiler/Compiler.hpp>

#include <string>
#include <vector>

namespace TabGraph::Renderer {
class Shader {
public:
    struct Stage {
        OCRA::ShaderCompiler::ShaderType type;
        std::string entryPoint;
        std::string source;
    };
    Shader() = default;
    Shader(const Renderer::Impl& a_Renderer, const std::vector<Stage>& a_Stages);
    auto& GetUniformBufferBinding(const std::string& a_Name) const
    {
        return _GetUniformBuffer(a_Name).binding;
    }
    void UpdateUniformBuffer(
        const std::string& a_Name,
        const OCRA::Buffer::Handle a_Buffer,
        const size_t a_Offset, const size_t a_Range);
    auto& GetBindings() const
    {
        return _bindings;
    }
    auto& GetStages() const
    {
        return _stages;
    }
    auto& GetDescriptorSet() const
    {
        return _descriptorSet;
    }

private:
    OCRA::DescriptorSetBinding& _PushUniformBuffer(const std::string& a_Name, const OCRA::DescriptorSetBinding& a_Binding = {})
    {
        _uniformBuffers[a_Name] = _bindings.size();
        return _bindings.emplace_back(a_Binding);
    }
    OCRA::DescriptorSetBinding& _GetUniformBuffer(const std::string& a_Name)
    {
        return _bindings.at(_uniformBuffers.at(a_Name));
    }
    const OCRA::DescriptorSetBinding& _GetUniformBuffer(const std::string& a_Name) const
    {
        return _bindings.at(_uniformBuffers.at(a_Name));
    }
    std::vector<OCRA::PipelineShaderStage> _stages;
    std::vector<OCRA::DescriptorSetBinding> _bindings;
    std::unordered_map<std::string, uint32_t> _uniformBuffers;
    OCRA::Descriptor::Set::Handle _descriptorSet;
};
}
