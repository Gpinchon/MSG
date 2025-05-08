#pragma once

#include <typeindex>
#include <vector>

namespace MSG::Renderer {
class SubsystemLibrary;
struct RendererSettings;
class Impl;
}

namespace MSG::Renderer {
using SubsystemDependencies = std::vector<std::type_index>;
class SubsystemInterface {
public:
    SubsystemInterface(const SubsystemDependencies& a_Deps = {})
        : dependencies(a_Deps)
    {
    }
    virtual void Update(Renderer::Impl& a_Renderer, const SubsystemLibrary& a_Subsystems) = 0;
    virtual void UpdateSettings(Renderer::Impl& a_Renderer, const RendererSettings& a_Settings) { };
    const SubsystemDependencies dependencies;
};
}