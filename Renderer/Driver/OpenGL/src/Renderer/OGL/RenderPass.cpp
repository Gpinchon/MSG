#include <Renderer/OGL/RAII/Buffer.hpp>
#include <Renderer/OGL/RAII/DebugGroup.hpp>
#include <Renderer/OGL/RAII/FrameBuffer.hpp>
#include <Renderer/OGL/RAII/Program.hpp>
#include <Renderer/OGL/RAII/Texture.hpp>
#include <Renderer/OGL/RAII/VertexArray.hpp>
#include <Renderer/OGL/RenderPass.hpp>

#include <GL/glew.h>

namespace TabGraph::Renderer {
bool operator==(const StencilOpState& a_Left, const StencilOpState& a_Right)
{
    return a_Left.failOp != a_Right.failOp
        || a_Left.depthFailOp != a_Right.depthFailOp
        || a_Left.passOp != a_Right.passOp
        || a_Left.compareOp != a_Right.compareOp
        || a_Left.compareMask != a_Right.compareMask
        || a_Left.writeMask != a_Right.writeMask
        || a_Left.reference != a_Right.reference;
}

bool operator!=(const StencilOpState& a_Left, const StencilOpState& a_Right)
{
    return !(a_Left == a_Right);
}

bool operator==(const DepthStencilState& a_Left, const DepthStencilState& a_Right)
{
    return a_Left.enableDepthTest != a_Right.enableDepthTest
        || a_Left.enableDepthWrite != a_Right.enableDepthWrite
        || a_Left.enableDepthBoundsTest != a_Right.enableDepthBoundsTest
        || a_Left.enableStencilTest != a_Right.enableStencilTest
        || a_Left.depthCompareOp != a_Right.depthCompareOp
        || a_Left.depthBounds != a_Right.depthBounds
        || a_Left.front != a_Right.front
        || a_Left.back != a_Right.back;
}

bool operator!=(const DepthStencilState& a_Left, const DepthStencilState& a_Right)
{
    return !(a_Left == a_Right);
}

void ApplyDepthStencilStates(const DepthStencilState& a_DsStates)
{
    auto applyDepthStatesDebugGroup = RAII::DebugGroup("Apply Depth States");
    a_DsStates.enableDepthBoundsTest ? glEnable(GL_DEPTH_BOUNDS_TEST_EXT) : glDisable(GL_DEPTH_BOUNDS_TEST_EXT);
    a_DsStates.enableDepthTest ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    a_DsStates.enableDepthWrite ? glDepthMask(GL_TRUE) : glDepthMask(GL_FALSE);
    a_DsStates.enableStencilTest ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST);
    glDepthFunc(a_DsStates.depthCompareOp);
    glDepthBoundsEXT(a_DsStates.depthBounds[0], a_DsStates.depthBounds[1]);
    // Front
    glStencilOpSeparate(
        GL_FRONT,
        a_DsStates.front.failOp,
        a_DsStates.front.depthFailOp,
        a_DsStates.front.passOp);
    glStencilFuncSeparate(
        GL_FRONT,
        a_DsStates.front.compareOp,
        a_DsStates.front.reference,
        a_DsStates.front.compareMask);
    glStencilMaskSeparate(
        GL_BACK,
        a_DsStates.front.writeMask);
    // Back
    glStencilOpSeparate(
        GL_BACK,
        a_DsStates.back.failOp,
        a_DsStates.back.depthFailOp,
        a_DsStates.back.passOp);
    glStencilFuncSeparate(
        GL_BACK,
        a_DsStates.back.compareOp,
        a_DsStates.back.reference,
        a_DsStates.back.compareMask);
    glStencilMaskSeparate(
        GL_BACK,
        a_DsStates.back.writeMask);
}

struct BufferBinding {
    GLenum target;
    uint index;
};

void ExecuteRenderPass(const RenderPassInfo& a_Pass)
{
    auto& pass          = a_Pass;
    auto passDebugGroup = RAII::DebugGroup("Execute Pass : " + pass.name);
    std::vector<BufferBinding> globalBuffersToUnbind;
    auto& fbState           = pass.frameBufferState;
    auto& graphicsPipelines = pass.graphicsPipelines;
    {
        auto clearFBDebugGroup = RAII::DebugGroup("Clear Framebuffer");
        for (auto& clearColor : fbState.clearColors) {
            glClearTexSubImage(
                *fbState.colorBuffers.at(clearColor.index),
                0, 0, 0, 0,
                pass.viewportState.viewport.x, pass.viewportState.viewport.y, 1,
                GL_RGBA, GL_FLOAT, &clearColor.color[0]);
        }
        if (fbState.clearDepth.has_value()) {
            glClearTexSubImage(
                *fbState.depthBuffer,
                0, 0, 0, 0,
                pass.viewportState.viewport.x, pass.viewportState.viewport.y, 1,
                GL_DEPTH_COMPONENT, GL_FLOAT, &fbState.clearDepth.value());
        }
        if (fbState.clearStencil.has_value()) {
            glClearTexSubImage(
                *fbState.depthBuffer,
                0, 0, 0, 0,
                pass.viewportState.viewport.x, pass.viewportState.viewport.y, 1,
                GL_STENCIL_INDEX, GL_INT, &fbState.clearStencil.value());
        }
    }

    {
        auto applyStatesDebugGroup = RAII::DebugGroup("Apply States");
        glViewport(0, 0, pass.viewportState.viewport.x, pass.viewportState.viewport.y);
    }
    {
        auto bindUBOsDebugGroup = RAII::DebugGroup("Bind buffers");
        globalBuffersToUnbind.reserve(1024);
        for (auto& info : pass.buffers) {
            glBindBufferRange(info.target, info.index, *info.buffer, info.offset, info.size);
            globalBuffersToUnbind.push_back({ GL_UNIFORM_BUFFER, info.index });
        }
    }
    {
        auto renderGeometryDebugGroup = RAII::DebugGroup("Render Geometry");
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, *pass.frameBufferState.frameBuffer);
        for (uint32_t index = 0; index < graphicsPipelines.size(); ++index) {
            auto& graphicsPipelineInfo = graphicsPipelines.at(index);
            auto lastPipeline          = index > 0 ? &graphicsPipelines.at(index - 1) : nullptr;
            if (lastPipeline == nullptr || (lastPipeline != nullptr && graphicsPipelineInfo.depthStencilState != lastPipeline->depthStencilState))
                ApplyDepthStencilStates(graphicsPipelineInfo.depthStencilState);
            for (auto& info : graphicsPipelineInfo.buffers) {
                glBindBufferRange(info.target, info.index, *info.buffer, info.offset, info.size);
            }
            if (lastPipeline == nullptr
                || lastPipeline->vertexInputState.vertexArray != graphicsPipelineInfo.vertexInputState.vertexArray) {
                glBindVertexArray(*graphicsPipelineInfo.vertexInputState.vertexArray);
            }
            if (lastPipeline == nullptr
                || lastPipeline->shaderState.program != graphicsPipelineInfo.shaderState.program) {
                glUseProgram(*graphicsPipelineInfo.shaderState.program);
            }
            if (graphicsPipelineInfo.vertexInputState.vertexArray->indexed) {
                glDrawElements(
                    graphicsPipelineInfo.rasterizationState.drawingMode,
                    graphicsPipelineInfo.vertexInputState.vertexArray->indexCount,
                    graphicsPipelineInfo.vertexInputState.vertexArray->indexDesc.type,
                    nullptr);
            } else {
                glDrawArrays(
                    graphicsPipelineInfo.rasterizationState.drawingMode,
                    0, graphicsPipelineInfo.vertexInputState.vertexArray->vertexCount);
            }
        }
    }
    {
        auto clearStatesDebugGroup = RAII::DebugGroup("Clear states");
        glUseProgram(0);
        glBindVertexArray(0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        for (const auto& i : globalBuffersToUnbind)
            glBindBufferBase(i.target, i.index, 0);
        static DepthStencilState defaultDSState {};
        if (graphicsPipelines.back().depthStencilState != defaultDSState)
            ApplyDepthStencilStates(defaultDSState);
    }
}
}