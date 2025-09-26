#pragma once

#include <MSG/Renderer/RenderSubPassInterface.hpp>

#include <MSG/Module.hpp>
#include <MSG/ModulesLibrary.hpp>

namespace MSG::Renderer {
class Impl;
class RenderPassInterface;
struct RendererSettings;
}

namespace MSG::Renderer {
using RenderPassesLibrary = ModulesLibrary<RenderPassInterface>;
class RenderPassInterface : public Module, public RenderSubPassesLibrary {
public:
    RenderPassInterface(const ModuleDependencies& a_Deps = {})
        : Module(a_Deps)
    {
    }
    virtual void UpdateSettings(Renderer::Impl& a_Renderer, const RendererSettings& a_Settings)
    {
        for (auto& subPass : modules)
            subPass->UpdateSettings(a_Renderer, a_Settings);
    };
    virtual void Update(Impl& a_Renderer, const RenderPassesLibrary& a_RenderPasses)
    {
        for (auto& subPass : modules)
            subPass->Update(a_Renderer, this);
    };
    virtual void Render(Impl& a_Renderer) = 0;
};
}