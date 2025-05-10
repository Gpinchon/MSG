#pragma once

#include <MSG/OGLPipelineInfo.hpp>

#include <variant>

namespace MSG {
class OGLPipeline;
}

namespace MSG {
class OGLGraphicsPipeline : public OGLGraphicsPipelineInfo {
public:
    OGLGraphicsPipeline(const OGLGraphicsPipelineInfo& a_Info);
    void Bind(const OGLPipeline* a_PreviousPipeline) const;
    void Restore() const;
};

class OGLComputePipeline : public OGLComputePipelineInfo {
public:
    OGLComputePipeline(const OGLComputePipelineInfo& a_Info);
    void Bind(const OGLPipeline* a_PreviousPipeline) const;
    void Restore() const;
};

class OGLPipeline : public std::variant<OGLGraphicsPipeline, OGLComputePipeline> {
public:
    using std::variant<OGLGraphicsPipeline, OGLComputePipeline>::variant;
};
}
