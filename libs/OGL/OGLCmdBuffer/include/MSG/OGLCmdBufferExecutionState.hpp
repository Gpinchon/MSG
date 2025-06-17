#pragma once

#include <MSG/OGLPipeline.hpp>
#include <MSG/OGLRenderPass.hpp>

#include <optional>

namespace MSG {
struct OGLCmdBufferExecutionState {
    std::optional<OGLRenderPass> renderPass;
    std::optional<OGLPipeline> pipeline;
};
}