#pragma once

#include <array>
#include <memory>
#include <variant>
#include <vector>

#include <glm/glm.hpp>

#include <GL/glew.h>

namespace MSG {
class OGLBuffer;
class OGLTexture;
class OGLProgram;
class OGLSampler;
class OGLVertexArray;
}

namespace MSG {
struct OGLInputAssemblyState {
    bool primitiveRestart    = false;
    GLenum primitiveTopology = GL_NONE;
};
struct OGLRasterizationState {
    bool rasterizerDiscardEnable  = false;
    bool depthClampEnable         = false;
    bool depthBiasEnable          = false;
    float depthBiasConstantFactor = 0;
    float depthBiasSlopeFactor    = 0;
    float depthBiasClamp          = 0;
    float lineWidth               = 1;
    GLenum polygonOffsetMode      = GL_POLYGON_OFFSET_FILL;
    GLenum polygonMode            = GL_FILL;
    GLenum cullMode               = GL_BACK;
    GLenum frontFace              = GL_CCW;
};
struct OGLVertexInputState {
    unsigned vertexCount = 0;
    unsigned indexCount  = 0;
    std::shared_ptr<OGLVertexArray> vertexArray;
};
struct OGLShaderState {
    std::shared_ptr<OGLProgram> program;
};
struct OGLColorBlendAttachmentState {
    unsigned index             = 0;
    bool enableBlend           = false;
    GLenum srcColorBlendFactor = GL_ONE;
    GLenum dstColorBlendFactor = GL_ZERO;
    GLenum colorBlendOp        = GL_FUNC_ADD;
    GLenum srcAlphaBlendFactor = GL_ONE;
    GLenum dstAlphaBlendFactor = GL_ZERO;
    GLenum alphaBlendOp        = GL_FUNC_ADD;
    glm::bvec4 colorWriteMask  = { true, true, true, true };
};
struct OGLColorBlendState {
    bool enableLogicOp       = false;
    GLenum logicOp           = GL_COPY;
    glm::vec4 blendConstants = { 0, 0, 0, 0 };
    std::vector<OGLColorBlendAttachmentState> attachmentStates;
};
struct OGLStencilOpState {
    GLenum failOp        = GL_KEEP; // the operation to be realized when stencil test FAILS
    GLenum depthFailOp   = GL_KEEP; // the operation to be realized when stencil test PASSES but depth test FAILS
    GLenum passOp        = GL_KEEP; // the operation to be realized when stencil & depth test PASSES
    GLenum compareOp     = GL_ALWAYS;
    uint32_t compareMask = ~0u; // a mask that is ANDed with ref and the buffer's content
    uint32_t writeMask   = ~0u; // a mask that is ANDed with the stencil value about to be written to the buffer
    uint32_t reference   = 0; // the reference value used in comparison.
};
struct OGLDepthStencilState {
    bool enableDepthTest       = true;
    bool enableDepthWrite      = true;
    bool enableDepthClamp      = false;
    bool enableDepthBoundsTest = false;
    bool enableStencilTest     = false;
    GLenum depthCompareOp      = GL_LESS;
    glm::dvec2 depthBounds     = { 0, 1 };
    glm::dvec2 depthRange      = { 0, 1 };
    OGLStencilOpState front    = {};
    OGLStencilOpState back     = {};
};
struct OGLBufferBindingInfo {
    std::shared_ptr<OGLBuffer> buffer = nullptr;
    uint32_t offset                   = 0;
    uint32_t size                     = 0;
};
struct OGLImageBindingInfo {
    std::shared_ptr<OGLTexture> texture = nullptr;
    GLenum access                       = GL_NONE;
    GLenum format                       = GL_NONE;
    int level                           = 0;
    int layer                           = 0;
    bool layered                        = false;
};
struct OGLTextureBindingInfo {
    std::shared_ptr<OGLTexture> texture = nullptr;
    std::shared_ptr<OGLSampler> sampler = nullptr;
};
struct OGLBindings {
    OGLBindings& operator+=(const OGLBindings& a_Other);
    std::array<OGLImageBindingInfo, 32> images;
    std::array<OGLTextureBindingInfo, 32> textures;
    std::array<OGLBufferBindingInfo, 32> uniformBuffers;
    std::array<OGLBufferBindingInfo, 32> storageBuffers;
};

struct OGLBasePipelineInfo {
    OGLBindings bindings; // the bindings for this Pipeline
    OGLShaderState shaderState; // the shader used to render the graphic pipeline
};

struct OGLGraphicsPipelineInfo : OGLBasePipelineInfo {
    OGLColorBlendState colorBlend;
    OGLDepthStencilState depthStencilState;
    OGLInputAssemblyState inputAssemblyState;
    OGLRasterizationState rasterizationState;
    OGLVertexInputState vertexInputState;
};

struct OGLComputePipelineInfo : OGLBasePipelineInfo {
    GLenum memoryBarrier = GL_NONE;
};
}
