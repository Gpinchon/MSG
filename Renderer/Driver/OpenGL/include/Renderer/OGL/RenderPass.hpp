#pragma once

#include <Renderer/OGL/RAII/Wrapper.hpp>
#include <Renderer/OGL/RenderPassInfo.hpp>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <optional>

namespace TabGraph::Renderer {
struct Context;
}

namespace TabGraph::Renderer {

struct RenderPass {
    RenderPass(const RenderPassInfo& a_Info);
    void Execute() const;
    const RenderPassInfo info;
};
}
