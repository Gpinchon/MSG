#include <MSG/Renderer/OGL/RenderSubPassInterface.hpp>

namespace MSG::Renderer {
class DfdSubPassOpaqueGeometry : public RenderSubPassInterface {
public:
    DfdSubPassOpaqueGeometry();
    void Update(Renderer::Impl& a_Renderer, RenderPassInterface* a_ParentPass) override;
    void Render(Impl& a_Renderer) override;
};
}