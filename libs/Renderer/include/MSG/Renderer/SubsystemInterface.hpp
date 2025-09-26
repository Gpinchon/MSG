#pragma once

#include <MSG/Module.hpp>

namespace MSG {
template <typename T>
class ModulesLibrary;
}

namespace MSG::Renderer {
class Impl;
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