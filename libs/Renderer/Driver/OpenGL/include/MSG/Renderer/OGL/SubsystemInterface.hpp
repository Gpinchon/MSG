#pragma once

#include <MSG/Renderer/OGL/Module.hpp>

namespace MSG::Renderer {
class Impl;
template <typename T>
class ModulesLibrary;
class SubsystemInterface;
struct RendererSettings;
}

namespace MSG::Renderer {
using SubsystemsLibrary = ModulesLibrary<SubsystemInterface>;
class SubsystemInterface : public Module {
public:
    SubsystemInterface(const ModuleDependencies& a_Deps = {})
        : Module(a_Deps)
    {
    }
    virtual void Update(Renderer::Impl& a_Renderer, const SubsystemsLibrary& a_Subsystems) = 0;
    virtual void UpdateSettings(Renderer::Impl& a_Renderer, const RendererSettings& a_Settings) { };
};
}