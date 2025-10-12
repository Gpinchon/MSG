#pragma once

#include <MSG/Module.hpp>

namespace Msg {
template <typename T>
class ModulesLibrary;
}

namespace Msg::Renderer {
class Impl;
class RenderPassInterface;
class RenderSubPassInterface;
struct RendererSettings;
}

namespace Msg::Renderer {
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