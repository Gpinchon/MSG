#pragma once

#include <MSG/Renderer/OGL/RenderSubPassInterface.hpp>

#include <memory>

namespace MSG {
class OGLProgram;
}

namespace MSG::Renderer {
class DfdSubPassSkybox : public RenderSubPassInterface {
public:
    DfdSubPassSkybox(Renderer::Impl& a_Renderer);
    void Update(Renderer::Impl& a_Renderer, RenderPassInterface* a_ParentPass) override;
    void Render(Impl& a_Renderer) override;
    std::shared_ptr<OGLProgram> shader;
};
}