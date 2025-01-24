#pragma once

#include <MSG/Renderer/OGL/RenderPassInfo.hpp>

namespace MSG {
class OGLContext;
}

namespace MSG::Renderer {

class RenderPass {
public:
    RenderPass(const RenderPassInfo& a_Info);
    void Execute() const;
    const RenderPassInfo info;
};
}
