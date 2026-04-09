#pragma once

#include <MSG/Renderer/RenderPassInterface.hpp>

#include <MSG/OGLCmdBuffer.hpp>

#include <memory>

namespace Msg::Renderer {
class PassShadowMaps : public RenderPassInterface {
public:
    PassShadowMaps(Renderer::Impl& a_Renderer);
    void Update(Renderer::Impl& a_Renderer, const RenderPassesLibrary& a_RenderPasses) override;
    void Render(Impl& a_Renderer) override;

private:
    bool _render = false;
    OGLCmdBuffer _cmdBuffer;
};
}