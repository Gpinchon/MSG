#pragma once

#include <MSG/Renderer/OGL/RenderPassInfo.hpp>

namespace MSG::Renderer {
class Context;
}

namespace MSG::Renderer {

class RenderPass {
public:
    RenderPass(const RenderPassInfo& a_Info);
    void Execute() const;
    const RenderPassInfo info;
};
}
