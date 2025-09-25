#pragma once

#include <MSG/Renderer/OGL/Module.hpp>

namespace MSG::Renderer {
class Impl;
class RenderPassInterface;
class RenderSubPassInterface;
template <typename T>
class ModulesLibrary;
struct RendererSettings;
}

namespace MSG::Renderer {
using RenderSubPassesLibrary = ModulesLibrary<RenderSubPassInterface>;
class RenderSubPassInterface : public Module {
public:
    RenderSubPassInterface(const ModuleDependencies& a_Deps = {})
        : Module(a_Deps)
    {
    }
    virtual void UpdateSettings(Renderer::Impl& a_Renderer, const RendererSettings& a_Settings) { };
    virtual void Update(Impl& a_Renderer, RenderPassInterface* a_ParentPass) = 0;
    virtual void Render(Impl& a_Renderer)                                    = 0;
};
}