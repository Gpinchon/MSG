#pragma once

#include <variant>

#include <MSG/Cmd/OGLCmdClearTexture.hpp>
#include <MSG/Cmd/OGLCmdCopyBuffer.hpp>
#include <MSG/Cmd/OGLCmdDispatchCompute.hpp>
#include <MSG/Cmd/OGLCmdDraw.hpp>
#include <MSG/Cmd/OGLCmdEndRenderPass.hpp>
#include <MSG/Cmd/OGLCmdGenerateMipmap.hpp>
#include <MSG/Cmd/OGLCmdMemoryBarrier.hpp>
#include <MSG/Cmd/OGLCmdPushCmdBuffer.hpp>
#include <MSG/Cmd/OGLCmdPushPipeline.hpp>
#include <MSG/Cmd/OGLCmdPushRenderPass.hpp>

namespace MSG {
using OGLCmd = std::variant<
    OGLCmdMemoryBarrier,
    OGLCmdClearTexture,
    OGLCmdPushRenderPass,
    OGLCmdEndRenderPass,
    OGLCmdPushCmdBuffer,
    OGLCmdDraw,
    OGLCmdDispatchCompute,
    OGLCmdPushPipeline,
    OGLCmdGenerateMipmap,
    OGLCmdCopyBuffer>;
}