#include <MSG/OGLCmdBuffer.hpp>
#include <MSG/Renderer/RenderSubPassInterface.hpp>

namespace Msg::Renderer {
class SubPassOpaqueGeometry : public RenderSubPassInterface {
public:
    SubPassOpaqueGeometry(Renderer::Impl& a_Rdr);
    void Update(Renderer::Impl& a_Renderer, RenderPassInterface* a_ParentPass) override;
    void Render(Impl& a_Renderer) override;

private:
    OGLCmdBuffer _cmdBuffer;
};
}