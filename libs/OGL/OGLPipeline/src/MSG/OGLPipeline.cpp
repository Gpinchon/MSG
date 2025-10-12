#include <MSG/Debug.hpp>
#include <MSG/OGLBuffer.hpp>
#include <MSG/OGLDebugGroup.hpp>
#include <MSG/OGLPipeline.hpp>
#include <MSG/OGLProgram.hpp>
#include <MSG/OGLSampler.hpp>
#include <MSG/OGLTexture2D.hpp>
#include <MSG/OGLVertexArray.hpp>

#include <GL/glew.h>
#include <iostream>

#ifndef BUFFER_OFFSET
#define BUFFER_OFFSET(i) ((char*)nullptr + (i))
#endif

namespace Msg {
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
    auto applyDepthStatesDebugGroup = OGLDebugGroup(std::string("OGLPipeline::") + __func__);
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

static void BindInputs(const OGLBindings& a_Bindings, const OGLBindings& a_BindingsPrev)
{
    auto debugGroup = OGLDebugGroup(std::string("OGLPipeline::") + __func__);
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

    a_LastCP = &a_Pipeline;
}

void BindBase(const OGLBasePipelineInfo& a_Info, const OGLBasePipelineInfo& a_Prev)
{
    if (a_Prev.shaderState.program != a_Info.shaderState.program)
        glUseProgram(*a_Info.shaderState.program);
    BindInputs(a_Info.bindings, a_Prev.bindings);
}

void RestoreBase(const OGLBasePipelineInfo& a_Info)
{
    BindInputs({}, a_Info.bindings);
    if (a_Info.shaderState.program != nullptr)
        glUseProgram(0);
}

static const OGLBasePipelineInfo s_DefaultBasePipeline;

Msg::OGLGraphicsPipeline::OGLGraphicsPipeline(const OGLGraphicsPipelineInfo& a_Info)
    : OGLGraphicsPipelineInfo(a_Info)
{
}

void Msg::OGLGraphicsPipeline::Bind(const OGLPipeline* a_Prev) const
{
    auto debugGroup = OGLDebugGroup(std::string("OGLGraphicsPipeline::") + __func__);
    if (a_Prev != nullptr) {
        std::visit(
            [this](const OGLBasePipelineInfo& a_BasePipeline) {
                BindBase(*this, a_BasePipeline);
            },
            *a_Prev);
    } else
        BindBase(*this, s_DefaultBasePipeline);
    // bind graphics pipeline specifics
    auto prevGP                        = std::get_if<OGLGraphicsPipeline>(a_Prev);
    const bool firstPipeline           = prevGP == nullptr;
    const bool applyBlendState         = firstPipeline || colorBlend != prevGP->colorBlend;
    const bool applyDepthStencilState  = firstPipeline || depthStencilState != prevGP->depthStencilState;
    const bool applyRasterizationState = firstPipeline || rasterizationState != prevGP->rasterizationState;
    if (applyBlendState) [[likely]] {
        if (!firstPipeline) [[likely]]
            ResetBlendState(prevGP->colorBlend);
        ApplyBlendState(colorBlend);
    }
    if (applyDepthStencilState) [[likely]]
        ApplyDepthStencilState(depthStencilState);
    if (applyRasterizationState) [[likely]]
        ApplyRasterizationState(rasterizationState);
    if (inputAssemblyState.primitiveRestart) [[unlikely]]
        glEnable(GL_PRIMITIVE_RESTART);
    else
        glDisable(GL_PRIMITIVE_RESTART);
    if (firstPipeline
        || prevGP->vertexInputState.vertexArray != vertexInputState.vertexArray) {
        glBindVertexArray(*vertexInputState.vertexArray);
    }
}

void Msg::OGLGraphicsPipeline::Restore() const
{
    auto debugGroup = OGLDebugGroup(std::string("OGLGraphicsPipeline::") + __func__);
    RestoreBase(*this);
    if (vertexInputState.vertexArray != nullptr)
        glBindVertexArray(0);
    static OGLDepthStencilState defaultDSState {};
    static OGLColorBlendState defaultCBState {};
    if (depthStencilState != defaultDSState) [[likely]]
        ApplyDepthStencilState(defaultDSState);
    if (colorBlend != defaultCBState) [[likely]]
        ResetBlendState(colorBlend);
}

Msg::OGLComputePipeline::OGLComputePipeline(const OGLComputePipelineInfo& a_Info)
    : OGLComputePipelineInfo(a_Info)
{
}

void Msg::OGLComputePipeline::Bind(const OGLPipeline* a_Prev) const
{
    auto debugGroup = OGLDebugGroup(std::string("OGLComputePipeline::") + __func__);
    if (a_Prev != nullptr) {
        std::visit(
            [this](const OGLBasePipelineInfo& a_BasePipeline) {
                BindBase(*this, a_BasePipeline);
            },
            *a_Prev);
    } else
        BindBase(*this, s_DefaultBasePipeline);
}

void Msg::OGLComputePipeline::Restore() const
{
    auto debugGroup = OGLDebugGroup(std::string("OGLComputePipeline") + __func__);
    RestoreBase(*this);
}
}
