#include <MSG/Debug.hpp>
#include <MSG/OGLBuffer.hpp>
#include <MSG/OGLDebugGroup.hpp>
#include <MSG/OGLFrameBuffer.hpp>
#include <MSG/OGLProgram.hpp>
#include <MSG/OGLRenderPass.hpp>
#include <MSG/OGLSampler.hpp>
#include <MSG/OGLTexture2D.hpp>
#include <MSG/OGLVertexArray.hpp>

#include <GL/glew.h>
#include <iostream>

#ifndef BUFFER_OFFSET
#define BUFFER_OFFSET(i) ((char*)nullptr + (i))
#endif

namespace MSG {
static bool operator!=(const OGLColorBlendAttachmentState& a_Left, const OGLColorBlendAttachmentState& a_Right)
{
    return a_Left.alphaBlendOp != a_Right.alphaBlendOp
        || a_Left.colorBlendOp != a_Right.colorBlendOp
        || a_Left.colorWriteMask != a_Right.colorWriteMask
        || a_Left.dstAlphaBlendFactor != a_Right.dstAlphaBlendFactor
        || a_Left.dstColorBlendFactor != a_Right.dstColorBlendFactor
        || a_Left.enableBlend != a_Right.enableBlend
        || a_Left.index != a_Right.index
        || a_Left.srcAlphaBlendFactor != a_Right.srcAlphaBlendFactor
        || a_Left.srcColorBlendFactor != a_Right.srcColorBlendFactor;
}

static bool operator==(const OGLColorBlendAttachmentState& a_Left, const OGLColorBlendAttachmentState& a_Right)
{
    return !(a_Left != a_Right);
}

static bool operator!=(const OGLColorBlendState& a_Left, const OGLColorBlendState& a_Right)
{
    return a_Left.attachmentStates != a_Right.attachmentStates
        || a_Left.blendConstants != a_Right.blendConstants
        || a_Left.enableLogicOp != a_Right.enableLogicOp
        || a_Left.logicOp != a_Right.logicOp;
}

static bool operator==(const OGLColorBlendState& a_Left, const OGLColorBlendState& a_Right)
{
    return !(a_Left != a_Right);
}

static bool operator!=(const OGLStencilOpState& a_Left, const OGLStencilOpState& a_Right)
{
    return a_Left.failOp != a_Right.failOp
        || a_Left.depthFailOp != a_Right.depthFailOp
        || a_Left.passOp != a_Right.passOp
        || a_Left.compareOp != a_Right.compareOp
        || a_Left.compareMask != a_Right.compareMask
        || a_Left.writeMask != a_Right.writeMask
        || a_Left.reference != a_Right.reference;
}

static bool operator==(const OGLStencilOpState& a_Left, const OGLStencilOpState& a_Right)
{
    return !(a_Left != a_Right);
}

static bool operator!=(const OGLDepthStencilState& a_Left, const OGLDepthStencilState& a_Right)
{
    return a_Left.enableDepthTest != a_Right.enableDepthTest
        || a_Left.enableDepthWrite != a_Right.enableDepthWrite
        || a_Left.enableDepthClamp != a_Right.enableDepthClamp
        || a_Left.enableDepthBoundsTest != a_Right.enableDepthBoundsTest
        || a_Left.enableStencilTest != a_Right.enableStencilTest
        || a_Left.depthCompareOp != a_Right.depthCompareOp
        || a_Left.depthBounds != a_Right.depthBounds
        || a_Left.depthRange != a_Right.depthRange
        || a_Left.front != a_Right.front
        || a_Left.back != a_Right.back;
}

static bool operator==(const OGLDepthStencilState& a_Left, const OGLDepthStencilState& a_Right)
{
    return !(a_Left != a_Right);
}

static bool operator!=(const OGLRasterizationState& a_Left, const OGLRasterizationState& a_Right)
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

static bool operator==(const OGLRasterizationState& a_Left, const OGLRasterizationState& a_Right)
{
    return !(a_Left != a_Right);
}

static bool operator!=(const OGLBufferBindingInfo& a_Left, const OGLBufferBindingInfo& a_Right)
{
    return a_Left.buffer != a_Right.buffer
        || a_Left.offset != a_Right.offset
        || a_Left.size != a_Right.size;
}

static bool operator==(const OGLBufferBindingInfo& a_Left, const OGLBufferBindingInfo& a_Right)
{
    return !(a_Left != a_Right);
}

static bool operator!=(const OGLTextureBindingInfo& a_Left, const OGLTextureBindingInfo& a_Right)
{
    return a_Left.sampler != a_Right.sampler
        || a_Left.texture != a_Right.texture;
}

static bool operator==(const OGLTextureBindingInfo& a_Left, const OGLTextureBindingInfo& a_Right)
{
    return !(a_Left != a_Right);
}

static bool operator!=(const OGLImageBindingInfo& a_Left, const OGLImageBindingInfo& a_Right)
{
    return a_Left.access != a_Right.access
        || a_Left.format != a_Right.format
        || a_Left.layer != a_Right.layer
        || a_Left.layered != a_Right.layered
        || a_Left.level != a_Right.level
        || a_Left.texture != a_Right.texture;
}

static bool operator==(const OGLImageBindingInfo& a_Left, const OGLImageBindingInfo& a_Right)
{
    return !(a_Left != a_Right);
}

void ApplyAttachmentBlendState(const OGLColorBlendAttachmentState& a_State)
{
    glEnablei(
        GL_BLEND,
        a_State.index);
    glBlendEquationSeparatei(
        a_State.index,
        a_State.colorBlendOp,
        a_State.alphaBlendOp);
    glBlendFuncSeparatei(
        a_State.index,
        a_State.srcColorBlendFactor, a_State.dstColorBlendFactor,
        a_State.srcAlphaBlendFactor, a_State.dstAlphaBlendFactor);
    glColorMaski(
        a_State.index,
        a_State.colorWriteMask.r,
        a_State.colorWriteMask.g,
        a_State.colorWriteMask.b,
        a_State.colorWriteMask.a);
}

void ApplyBlendState(const OGLColorBlendState& a_CbState)
{
    a_CbState.enableLogicOp ? glEnable(GL_COLOR_LOGIC_OP) : glDisable(GL_COLOR_LOGIC_OP);
    glLogicOp(a_CbState.logicOp);
    glBlendColor(
        a_CbState.blendConstants.r,
        a_CbState.blendConstants.g,
        a_CbState.blendConstants.b,
        a_CbState.blendConstants.a);
    for (auto& attachment : a_CbState.attachmentStates) {
        ApplyAttachmentBlendState(attachment);
    }
}

void ResetBlendState(const OGLColorBlendState& a_CbState)
{
    static OGLColorBlendState defaultState;
    if (defaultState == a_CbState)
        return;
    ApplyBlendState(defaultState);
    for (auto& attachment : a_CbState.attachmentStates) {
        OGLColorBlendAttachmentState blendState = { attachment.index };
        ApplyAttachmentBlendState(blendState);
    }
}

void ApplyDepthStencilState(const OGLDepthStencilState& a_DsStates)
{
    auto applyDepthStatesDebugGroup = OGLDebugGroup("Apply Depth States");
    a_DsStates.enableDepthBoundsTest ? glEnable(GL_DEPTH_BOUNDS_TEST_EXT) : glDisable(GL_DEPTH_BOUNDS_TEST_EXT);
    a_DsStates.enableDepthClamp ? glEnable(GL_DEPTH_CLAMP) : glDisable(GL_DEPTH_CLAMP);
    a_DsStates.enableDepthTest ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    a_DsStates.enableDepthWrite ? glDepthMask(GL_TRUE) : glDepthMask(GL_FALSE);
    a_DsStates.enableStencilTest ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST);
    glDepthFunc(a_DsStates.depthCompareOp);
    glDepthBoundsEXT(a_DsStates.depthBounds[0], a_DsStates.depthBounds[1]);
    glDepthRange(a_DsStates.depthRange[0], a_DsStates.depthRange[1]);
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

void ApplyRasterizationState(const OGLRasterizationState& a_RasterizationState)
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

struct OGLClearFormat {
    GLenum format = GL_NONE;
    GLenum type   = GL_NONE;
};

OGLClearFormat GetClearFormat(const GLenum& a_SizedFormat);

void ApplyFBState(const OGLFrameBufferState& a_FBState, const glm::uvec2& a_Viewport)
{
    auto clearFBDebugGroup = OGLDebugGroup(__func__);
    if (a_FBState.framebuffer == nullptr) {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        return;
    }
    auto& fbInfo = a_FBState.framebuffer->info;
    for (auto& clearColor : a_FBState.clear.colors) {
        auto& colorBuffer = fbInfo.colorBuffers.at(clearColor.index).texture;
#ifndef NDEBUG
        int supported;
        glGetInternalformativ(colorBuffer->target, colorBuffer->sizedFormat, GL_CLEAR_TEXTURE, 1, &supported);
        assert(supported == GL_FULL_SUPPORT);
#endif // NDEBUG
        OGLClearFormat clearFormat { GetClearFormat(colorBuffer->sizedFormat) };
        glClearTexSubImage(
            *colorBuffer,
            0, 0, 0, 0,
            a_Viewport.x, a_Viewport.y, 1,
            clearFormat.format, clearFormat.type, &clearColor.color);
    }
    if (a_FBState.clear.depthStencil.has_value()) {
        glClearTexSubImage(
            *fbInfo.depthBuffer.texture,
            0, 0, 0, fbInfo.depthBuffer.layer,
            a_Viewport.x, a_Viewport.y, 1,
            GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8_EXT, &a_FBState.clear.depthStencil.value());
    }
    if (a_FBState.clear.depth.has_value()) {
        glClearTexSubImage(
            *fbInfo.depthBuffer.texture,
            0, 0, 0, fbInfo.depthBuffer.layer,
            a_Viewport.x, a_Viewport.y, 1,
            GL_DEPTH_COMPONENT, GL_FLOAT, &a_FBState.clear.depth.value());
    }
    if (a_FBState.clear.stencil.has_value()) {
        glClearTexSubImage(
            *fbInfo.depthBuffer.texture,
            0, 0, 0, fbInfo.depthBuffer.layer,
            a_Viewport.x, a_Viewport.y, 1,
            GL_STENCIL_INDEX, GL_INT, &a_FBState.clear.stencil.value());
    }
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, *a_FBState.framebuffer);
    if (!a_FBState.drawBuffers.empty())
        glNamedFramebufferDrawBuffers(
            *a_FBState.framebuffer,
            GLsizei(a_FBState.drawBuffers.size()), a_FBState.drawBuffers.data());
    else
        glDrawBuffer(GL_NONE);
    glViewport(0, 0, a_Viewport.x, a_Viewport.y);
}

static void BindInputs(const OGLBindings& a_Bindings, const OGLBindings& a_BindingsPrev)
{
    auto debugGroup = OGLDebugGroup(__func__);
    for (uint8_t index = 0; index < a_Bindings.storageBuffers.size(); index++) {
        auto& infoPrev = a_BindingsPrev.storageBuffers.at(index);
        auto& info     = a_Bindings.storageBuffers.at(index);
        if (info == infoPrev)
            continue;
        if (info.buffer != nullptr)
            glBindBufferRange(GL_SHADER_STORAGE_BUFFER, index, *info.buffer, info.offset, info.size);
        else
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, 0);
    }
    for (uint8_t index = 0; index < a_Bindings.uniformBuffers.size(); index++) {
        auto& infoPrev = a_BindingsPrev.uniformBuffers.at(index);
        auto& info     = a_Bindings.uniformBuffers.at(index);
        if (info == infoPrev)
            continue;
        if (info.buffer != nullptr)
            glBindBufferRange(GL_UNIFORM_BUFFER, index, *info.buffer, info.offset, info.size);
        else
            glBindBufferBase(GL_UNIFORM_BUFFER, index, 0);
    }
    for (uint8_t index = 0; index < a_Bindings.textures.size(); index++) {
        auto& infoPrev = a_BindingsPrev.textures.at(index);
        auto& info     = a_Bindings.textures.at(index);
        if (info == infoPrev)
            continue;
        if (info.texture != nullptr)
            glBindTextureUnit(index, *info.texture);
        else
            glBindTextureUnit(index, 0);
        if (info.sampler != nullptr)
            glBindSampler(index, *info.sampler);
        else
            glBindSampler(index, 0);
    }
    for (uint8_t index = 0; index < a_Bindings.images.size(); index++) {
        auto& infoPrev = a_BindingsPrev.images.at(index);
        auto& info     = a_Bindings.images.at(index);
        if (info == infoPrev)
            continue;
        if (info.texture != nullptr)
            glBindImageTexture(index, *info.texture, info.level, info.layered, info.layer, info.access, info.format);
        else
            glBindImageTexture(index, 0, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R8);
    }
}

void ExecutePipeline(
    const OGLComputePipelineInfo& a_Pipeline,
    const OGLGraphicsPipelineInfo*& a_LastGP,
    const OGLComputePipelineInfo*& a_LastCP)
{
    glDispatchCompute(a_Pipeline.workgroupX, a_Pipeline.workgroupY, a_Pipeline.workgroupZ);
    if (a_Pipeline.memoryBarrier != GL_NONE)
        glMemoryBarrier(a_Pipeline.memoryBarrier);
    a_LastCP = &a_Pipeline;
}

void ExecutePipeline(
    const OGLGraphicsPipelineInfo& a_Pipeline,
    const OGLGraphicsPipelineInfo*& a_LastGP,
    const OGLComputePipelineInfo*& a_LastCP)
{
    auto debugGroup = OGLDebugGroup(__func__);
    if (a_Pipeline.drawCommands.empty())
        return; // no need to continue if we don't draw anything
    const bool firstPipeline           = a_LastGP == nullptr;
    const bool applyBlendState         = firstPipeline || a_Pipeline.colorBlend != a_LastGP->colorBlend;
    const bool applyDepthStencilState  = firstPipeline || a_Pipeline.depthStencilState != a_LastGP->depthStencilState;
    const bool applyRasterizationState = firstPipeline || a_Pipeline.rasterizationState != a_LastGP->rasterizationState;
    if (applyBlendState) [[likely]] {
        if (!firstPipeline) [[likely]]
            ResetBlendState(a_LastGP->colorBlend);
        ApplyBlendState(a_Pipeline.colorBlend);
    }
    if (applyDepthStencilState) [[likely]]
        ApplyDepthStencilState(a_Pipeline.depthStencilState);
    if (applyRasterizationState) [[likely]]
        ApplyRasterizationState(a_Pipeline.rasterizationState);
    if (a_Pipeline.inputAssemblyState.primitiveRestart) [[unlikely]]
        glEnable(GL_PRIMITIVE_RESTART);
    else
        glDisable(GL_PRIMITIVE_RESTART);
    if (firstPipeline
        || a_LastGP->vertexInputState.vertexArray != a_Pipeline.vertexInputState.vertexArray) {
        glBindVertexArray(*a_Pipeline.vertexInputState.vertexArray);
    }
    for (auto& drawCmd : a_Pipeline.drawCommands) {
        if (drawCmd.indexed) {
            glDrawElementsInstancedBaseVertexBaseInstance(
                a_Pipeline.inputAssemblyState.primitiveTopology,
                drawCmd.indexCount,
                a_Pipeline.vertexInputState.vertexArray->indexDesc.type,
                BUFFER_OFFSET(drawCmd.indexOffset),
                drawCmd.instanceCount,
                drawCmd.vertexOffset,
                drawCmd.instanceOffset);
        } else {
            glDrawArraysInstancedBaseInstance(
                a_Pipeline.inputAssemblyState.primitiveTopology,
                drawCmd.vertexOffset,
                drawCmd.vertexCount,
                drawCmd.instanceCount,
                drawCmd.instanceOffset);
        }
    }
    a_LastGP = &a_Pipeline;
}

struct PipelineFunctor {
    ~PipelineFunctor()
    {
        auto clearStatesDebugGroup = OGLDebugGroup("Clear states");
        glUseProgram(0);
        glBindVertexArray(0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        BindInputs({}, bindingsPrev);
        if (lastGP != nullptr) {
            static OGLDepthStencilState defaultDSState {};
            static OGLColorBlendState defaultCBState {};
            if (lastGP->depthStencilState != defaultDSState) [[likely]]
                ApplyDepthStencilState(defaultDSState);
            if (lastGP->colorBlend != defaultCBState) [[likely]]
                ResetBlendState(lastGP->colorBlend);
        }
    }
    template <typename PipelineType>
    void operator()(const PipelineType& a_Pipeline)
    {
        const bool firstPipeline = lastPipeline == nullptr;
        if (firstPipeline
            || lastPipeline->shaderState.program != a_Pipeline.shaderState.program) {
            glUseProgram(*a_Pipeline.shaderState.program);
        }
        BindInputs(a_Pipeline.bindings, bindingsPrev);
        ExecutePipeline(a_Pipeline, lastGP, lastCP);
        bindingsPrev = a_Pipeline.bindings;
        lastPipeline = &a_Pipeline;
    };
    OGLBindings bindingsPrev;
    const OGLBasePipelineInfo* lastPipeline = nullptr;
    const OGLGraphicsPipelineInfo* lastGP   = nullptr;
    const OGLComputePipelineInfo* lastCP    = nullptr;
};

void ExecutePipelines(const std::vector<OGLPipelineInfo>& a_Pipelines)
{
    auto debugGroup = OGLDebugGroup(__func__);
    PipelineFunctor functor;
    for (auto& pipelineInfo : a_Pipelines)
        std::visit(functor, pipelineInfo);
}

OGLRenderPass::OGLRenderPass(const OGLRenderPassInfo& a_Info)
    : info(a_Info)
{
}

void OGLRenderPass::Execute() const
{
    auto debugGroup = OGLDebugGroup("Execute Pass : " + info.name);
    ApplyFBState(info.frameBufferState, info.viewportState.viewport);
    ExecutePipelines(info.pipelines);
}

OGLClearFormat GetClearFormat(const GLenum& a_SizedFormat)
{
    OGLClearFormat format;
    switch (a_SizedFormat) {
    case GL_R8:
        format.format = GL_RED;
        format.type   = GL_FLOAT;
        break;
    case GL_R8_SNORM:
        format.format = GL_RED;
        format.type   = GL_FLOAT;
        break;
    case GL_R8UI:
        format.format = GL_RED_INTEGER;
        format.type   = GL_UNSIGNED_INT;
        break;
    case GL_R8I:
        format.format = GL_RED_INTEGER;
        format.type   = GL_INT;
        break;
    case GL_R16:
        format.format = GL_RED;
        format.type   = GL_FLOAT;
        break;
    case GL_R16_SNORM:
        format.format = GL_RED;
        format.type   = GL_FLOAT;
        break;
    case GL_R16UI:
        format.format = GL_RED_INTEGER;
        format.type   = GL_UNSIGNED_INT;
        break;
    case GL_R16I:
        format.format = GL_RED_INTEGER;
        format.type   = GL_INT;
        break;
    case GL_R32UI:
        format.format = GL_RED_INTEGER;
        format.type   = GL_UNSIGNED_INT;
        break;
    case GL_R32I:
        format.format = GL_RED_INTEGER;
        format.type   = GL_INT;
        break;
    case GL_R16F:
        format.format = GL_RED;
        format.type   = GL_FLOAT;
        break;
    case GL_R32F:
        format.format = GL_RED;
        format.type   = GL_FLOAT;
        break;
    case GL_RG8:
        format.format = GL_RG;
        format.type   = GL_UNSIGNED_BYTE;
        break;
    case GL_RG8_SNORM:
        format.format = GL_RG;
        format.type   = GL_FLOAT;
        break;
    case GL_RG8UI:
        format.format = GL_RG_INTEGER;
        format.type   = GL_UNSIGNED_INT;
        break;
    case GL_RG8I:
        format.format = GL_RG_INTEGER;
        format.type   = GL_INT;
        break;
    case GL_RG16:
        format.format = GL_RG;
        format.type   = GL_FLOAT;
        break;
    case GL_RG16_SNORM:
        format.format = GL_RG;
        format.type   = GL_FLOAT;
        break;
    case GL_RG16UI:
        format.format = GL_RG_INTEGER;
        format.type   = GL_UNSIGNED_INT;
        break;
    case GL_RG16I:
        format.format = GL_RG_INTEGER;
        format.type   = GL_INT;
        break;
    case GL_RG32UI:
        format.format = GL_RG_INTEGER;
        format.type   = GL_UNSIGNED_INT;
        break;
    case GL_RG32I:
        format.format = GL_RG_INTEGER;
        format.type   = GL_INT;
        break;
    case GL_RG16F:
        format.format = GL_RG;
        format.type   = GL_FLOAT;
        break;
    case GL_RG32F:
        format.format = GL_RG;
        format.type   = GL_FLOAT;
        break;
    case GL_RGB8:
        format.format = GL_RGB;
        format.type   = GL_FLOAT;
        break;
    case GL_RGB8_SNORM:
        format.format = GL_RGB;
        format.type   = GL_FLOAT;
        break;
    case GL_RGB8UI:
        format.format = GL_RGB_INTEGER;
        format.type   = GL_UNSIGNED_INT;
        break;
    case GL_RGB8I:
        format.format = GL_RGB_INTEGER;
        format.type   = GL_INT;
        break;
    case GL_RGB16:
        format.format = GL_RGB;
        format.type   = GL_FLOAT;
        break;
    case GL_RGB16_SNORM:
        format.format = GL_RGB;
        format.type   = GL_FLOAT;
        break;
    case GL_RGB16UI:
        format.format = GL_RGB_INTEGER;
        format.type   = GL_UNSIGNED_INT;
        break;
    case GL_RGB16I:
        format.format = GL_RGB_INTEGER;
        format.type   = GL_INT;
        break;
    case GL_RGB32UI:
        format.format = GL_RGB_INTEGER;
        format.type   = GL_UNSIGNED_INT;
        break;
    case GL_RGB32I:
        format.format = GL_RGB_INTEGER;
        format.type   = GL_INT;
        break;
    case GL_RGB16F:
        format.format = GL_RGB;
        format.type   = GL_FLOAT;
        break;
    case GL_RGB32F:
        format.format = GL_RGB;
        format.type   = GL_FLOAT;
        break;
    case GL_RGBA8:
        format.format = GL_RGBA;
        format.type   = GL_FLOAT;
        break;
    case GL_RGBA8_SNORM:
        format.format = GL_RGBA;
        format.type   = GL_FLOAT;
        break;
    case GL_RGBA8UI:
        format.format = GL_RGBA_INTEGER;
        format.type   = GL_UNSIGNED_INT;
        break;
    case GL_RGBA8I:
        format.format = GL_RGBA_INTEGER;
        format.type   = GL_INT;
        break;
    case GL_RGBA16:
        format.format = GL_RGBA;
        format.type   = GL_FLOAT;
        break;
    case GL_RGBA16_SNORM:
        format.format = GL_RGBA;
        format.type   = GL_FLOAT;
        break;
    case GL_RGBA16UI:
        format.format = GL_RGBA_INTEGER;
        format.type   = GL_UNSIGNED_INT;
        break;
    case GL_RGBA16I:
        format.format = GL_RGBA_INTEGER;
        format.type   = GL_INT;
        break;
    case GL_RGBA32UI:
        format.format = GL_RGBA_INTEGER;
        format.type   = GL_UNSIGNED_INT;
        break;
    case GL_RGBA32I:
        format.format = GL_RGBA_INTEGER;
        format.type   = GL_INT;
        break;
    case GL_RGBA16F:
        format.format = GL_RGBA;
        format.type   = GL_FLOAT;
        break;
    case GL_RGBA32F:
        format.format = GL_RGBA;
        format.type   = GL_FLOAT;
        break;
    case GL_RGBA_DXT5_S3TC:
        format.format = GL_RGBA;
        format.type   = GL_RGBA_DXT5_S3TC;
        break;
    default:
        errorFatal("Unknown Format");
    }
    return format;
}
}
