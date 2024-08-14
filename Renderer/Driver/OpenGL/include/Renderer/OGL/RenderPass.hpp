#pragma once

#include <Renderer/OGL/RenderPassInfo.hpp>

namespace TabGraph::Renderer {
class Context;
}

namespace TabGraph::Renderer {

struct RenderPass {
    RenderPass(const RenderPassInfo& a_Info);
    void Execute() const;
    const RenderPassInfo info;
};
}
