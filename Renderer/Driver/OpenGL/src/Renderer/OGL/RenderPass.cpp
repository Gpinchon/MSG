#include <Renderer/OGL/RAII/Buffer.hpp>
#include <Renderer/OGL/RAII/DebugGroup.hpp>
#include <Renderer/OGL/RAII/FrameBuffer.hpp>
#include <Renderer/OGL/RAII/Program.hpp>
#include <Renderer/OGL/RAII/Sampler.hpp>
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

bool operator==(const RasterizationState& a_Left, const RasterizationState& a_Right)
{
    return a_Left.rasterizerDiscardEnable != a_Right.rasterizerDiscardEnable
        || a_Left.depthClampEnable != a_Right.depthClampEnable
        || a_Left.depthBiasEnable != a_Right.depthBiasEnable
        || a_Left.depthBiasConstantFactor != a_Right.depthBiasConstantFactor
        || a_Left.depthBiasSlopeFactor != a_Right.depthBiasSlopeFactor
        || a_Left.depthBiasClamp != a_Right.depthBiasClamp
        || a_Left.lineWidth != a_Right.lineWidth
        || a_Left.polygonOffsetMode != a_Right.polygonOffsetMode
        || a_Left.polygonMode != a_Right.polygonMode
        || a_Left.cullMode != a_Right.cullMode
        || a_Left.frontFace != a_Right.frontFace;
}

bool operator!=(const RasterizationState& a_Left, const RasterizationState& a_Right)
{
    return !(a_Left == a_Right);
}

void ApplyDepthStencilState(const DepthStencilState& a_DsStates)
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

void ApplyRasterizationState(const RasterizationState& a_RasterizationState)
{
    a_RasterizationState.rasterizerDiscardEnable ? glEnable(GL_RASTERIZER_DISCARD) : glDisable(GL_RASTERIZER_DISCARD);
    a_RasterizationState.depthClampEnable ? glEnable(GL_DEPTH_CLAMP) : glDisable(GL_DEPTH_CLAMP);
    a_RasterizationState.depthBiasEnable ? glEnable(a_RasterizationState.polygonOffsetMode) : glDisable(a_RasterizationState.polygonOffsetMode);
    glPolygonMode(
        GL_FRONT_AND_BACK,
        a_RasterizationState.polygonMode);
    glPolygonOffsetClamp(
        a_RasterizationState.depthBiasConstantFactor,
        a_RasterizationState.depthBiasSlopeFactor,
        a_RasterizationState.depthBiasClamp);
    glLineWidth(a_RasterizationState.lineWidth);
    glFrontFace(a_RasterizationState.frontFace);
    if (a_RasterizationState.cullMode != GL_NONE) {
        glEnable(GL_CULL_FACE);
        glCullFace(a_RasterizationState.cullMode);
    } else
        glDisable(GL_CULL_FACE);
}

static inline auto GetClearColorType(const GLenum& a_SizedFormat)
{
    int type;
    glGetInternalformativ(GL_TEXTURE_2D, a_SizedFormat, GL_READ_PIXELS_TYPE, 1, &type);
    return type;
}

static inline auto GetClearColorFormat(const GLenum& a_SizedFormat)
{
    int format;
    glGetInternalformativ(GL_TEXTURE_2D, a_SizedFormat, GL_READ_PIXELS_FORMAT, 1, &format);
    return format;
}

void ApplyFBState(const FrameBufferState& a_FBState, const glm::uvec2& a_Viewport)
{
    auto clearFBDebugGroup = RAII::DebugGroup(__func__);
    if (a_FBState.framebuffer == nullptr) {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        return;
    }
    auto& fbInfo = a_FBState.framebuffer->info;
    for (auto& clearColor : a_FBState.clear.colors) {
        auto& colorBuffer     = fbInfo.colorBuffers.at(clearColor.index).texture;
        auto clearColorFormat = GetClearColorFormat(colorBuffer->sizedFormat);
        auto clearColorType   = GetClearColorType(colorBuffer->sizedFormat);
        glClearTexSubImage(
            *colorBuffer,
            0, 0, 0, 0,
            a_Viewport.x, a_Viewport.y, 1,
            clearColorFormat, clearColorType, &clearColor.color);
    }
    if (a_FBState.clear.depth.has_value()) {
        glClearTexSubImage(
            *fbInfo.depthBuffer,
            0, 0, 0, 0,
            a_Viewport.x, a_Viewport.y, 1,
            GL_DEPTH_COMPONENT, GL_FLOAT, &a_FBState.clear.depth.value());
    }
    if (a_FBState.clear.stencil.has_value()) {
        glClearTexSubImage(
            *fbInfo.depthBuffer,
            0, 0, 0, 0,
            a_Viewport.x, a_Viewport.y, 1,
            GL_STENCIL_INDEX, GL_INT, &a_FBState.clear.stencil.value());
    }
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, *a_FBState.framebuffer);
    if (!a_FBState.drawBuffers.empty())
        glNamedFramebufferDrawBuffers(
            *a_FBState.framebuffer,
            a_FBState.drawBuffers.size(), a_FBState.drawBuffers.data());
    else
        glDrawBuffer(GL_NONE);
}

void BindInputs(const Bindings& a_Bindings)
{
    auto debugGroup = RAII::DebugGroup(__func__);
    for (const auto& info : a_Bindings.buffers) {
        if (info.buffer != nullptr)
            glBindBufferRange(info.target, info.index, *info.buffer, info.offset, info.size);
        else
            glBindBuffer(info.target, 0);
    }
    for (const auto& info : a_Bindings.textures) {
        glActiveTexture(GL_TEXTURE0 + info.bindingIndex);
        if (info.texture != nullptr)
            glBindTexture(info.target, *info.texture);
        else
            glBindTexture(info.target, 0);
        if (info.sampler != nullptr)
            glBindSampler(info.bindingIndex, *info.sampler);
        else
            glBindSampler(info.bindingIndex, 0);
    }
    for (const auto& info : a_Bindings.images) {
        if (info.texture != nullptr)
            glBindImageTexture(info.bindingIndex, *info.texture, info.level, info.layered, info.layer, info.access, info.format);
        else
            glBindImageTexture(info.bindingIndex, 0, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R8);
    }
}

void UnbindInputs(const Bindings& a_Bindings)
{
    auto debugGroup = RAII::DebugGroup(__func__);
    for (const auto& info : a_Bindings.buffers) {
        glBindBuffer(info.target, 0);
    }
    for (const auto& info : a_Bindings.textures) {
        glActiveTexture(GL_TEXTURE0 + info.bindingIndex);
        glBindTexture(info.target, 0);
        glBindSampler(info.bindingIndex, 0);
    }
    for (const auto& info : a_Bindings.images) {
        glBindImageTexture(info.bindingIndex, 0, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R8);
    }
}

void ExecuteGraphicsPipeline(const RenderPassInfo& a_Info)
{
    auto debugGroup = RAII::DebugGroup(__func__);
    for (uint32_t index = 0; index < a_Info.graphicsPipelines.size(); ++index) {
        auto graphicsPipelineInfo          = a_Info.graphicsPipelines.at(index);
        auto lastPipeline                  = index > 0 ? &a_Info.graphicsPipelines.at(index - 1) : nullptr;
        const bool firstPipeline           = lastPipeline == nullptr;
        const bool applyDepthStencilState  = firstPipeline || (firstPipeline && graphicsPipelineInfo.depthStencilState != lastPipeline->depthStencilState);
        const bool applyRasterizationState = firstPipeline || (firstPipeline && graphicsPipelineInfo.rasterizationState != lastPipeline->rasterizationState);
        if (applyDepthStencilState)
            ApplyDepthStencilState(graphicsPipelineInfo.depthStencilState);
        if (applyRasterizationState)
            ApplyRasterizationState(graphicsPipelineInfo.rasterizationState);
        BindInputs(graphicsPipelineInfo.bindings);
        if (lastPipeline == nullptr
            || lastPipeline->vertexInputState.vertexArray != graphicsPipelineInfo.vertexInputState.vertexArray) {
            glBindVertexArray(*graphicsPipelineInfo.vertexInputState.vertexArray);
        }
        if (lastPipeline == nullptr
            || lastPipeline->shaderState.program != graphicsPipelineInfo.shaderState.program) {
            glUseProgram(*graphicsPipelineInfo.shaderState.program);
        }
        if (graphicsPipelineInfo.inputAssemblyState.primitiveRestart)
            glEnable(GL_PRIMITIVE_RESTART);
        else
            glDisable(GL_PRIMITIVE_RESTART);
        if (graphicsPipelineInfo.vertexInputState.vertexArray->indexed) {
            glDrawElements(
                graphicsPipelineInfo.inputAssemblyState.primitiveTopology,
                graphicsPipelineInfo.vertexInputState.vertexArray->indexCount,
                graphicsPipelineInfo.vertexInputState.vertexArray->indexDesc.type,
                nullptr);
        } else {
            glDrawArrays(
                graphicsPipelineInfo.inputAssemblyState.primitiveTopology,
                0, graphicsPipelineInfo.vertexInputState.vertexArray->vertexCount);
        }
        UnbindInputs(graphicsPipelineInfo.bindings);
    }
}

RenderPass::RenderPass(const RenderPassInfo& a_Info)
    : info(a_Info)
{
}

void RenderPass::Execute() const
{
    auto debugGroup = RAII::DebugGroup("Execute Pass : " + info.name);
    ApplyFBState(info.frameBufferState, info.viewportState.viewport);
    glViewport(0, 0, info.viewportState.viewport.x, info.viewportState.viewport.y);
    BindInputs(info.bindings);
    ExecuteGraphicsPipeline(info);
    {
        auto clearStatesDebugGroup = RAII::DebugGroup("Clear states");
        glUseProgram(0);
        glBindVertexArray(0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        UnbindInputs(info.bindings);
        static DepthStencilState defaultDSState {};
        if (!info.graphicsPipelines.empty() && info.graphicsPipelines.back().depthStencilState != defaultDSState)
            ApplyDepthStencilState(defaultDSState);
    }
}
}