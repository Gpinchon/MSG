#pragma once

#include <MSG/OGLPipeline.hpp>
#include <MSG/OGLRenderPass.hpp>

#include <optional>

namespace Msg {
struct OGLCmdBufferExecutionState {
    std::shared_ptr<OGLRenderPass> renderPass;
    std::shared_ptr<OGLPipeline> pipeline;
};
}
