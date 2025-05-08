#pragma once

#include <MSG/OGLCmd.hpp>
#include <MSG/OGLPipeline.hpp>
#include <MSG/Renderer/OGL/Subsystems/SubsystemInterface.hpp>

namespace MSG::Renderer {
struct MeshInfo {
    OGLGraphicsPipelineInfo pipeline;
    OGLCmdDrawInfo drawCmd;
    bool isMetRough;
    bool isSpecGloss;
    bool isUnlit;
};
class MeshSubsystem : public SubsystemInterface {
public:
    MeshSubsystem(Renderer::Impl& a_Renderer);
    void Update(Renderer::Impl& a_Renderer, const SubsystemLibrary& a_Subsystems) override;
    OGLBindings globalBindings;
    std::vector<MeshInfo> opaque;
    std::vector<MeshInfo> blended;
    std::shared_ptr<OGLTexture> brdfLut;
    std::shared_ptr<OGLSampler> brdfLutSampler;
};
}