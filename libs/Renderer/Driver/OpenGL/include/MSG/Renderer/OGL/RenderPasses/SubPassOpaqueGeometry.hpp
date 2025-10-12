#include <MSG/Renderer/RenderSubPassInterface.hpp>

namespace Msg::Renderer {
class SubPassOpaqueGeometry : public RenderSubPassInterface {
public:
    SubPassOpaqueGeometry();
    void Update(Renderer::Impl& a_Renderer, RenderPassInterface* a_ParentPass) override;
    void Render(Impl& a_Renderer) override;
};
}