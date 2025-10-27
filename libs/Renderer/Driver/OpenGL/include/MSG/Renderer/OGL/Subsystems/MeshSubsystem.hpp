#pragma once

#include <MSG/OGLCmd.hpp>
#include <MSG/OGLPipeline.hpp>
#include <MSG/Renderer/OGL/Primitive.hpp>
#include <MSG/Renderer/SubsystemInterface.hpp>
#include <MSG/Tools/ObjectCache.hpp>

namespace Msg::Renderer {
using PrimitiveCacheKey = Tools::ObjectCacheKey<MeshPrimitive*>;
using PrimitiveCache    = Tools::ObjectCache<PrimitiveCacheKey, std::shared_ptr<Primitive>>;
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
    ~MeshSubsystem();
    void Load(Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity) override;
    void Unload(Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity) override;
    void Update(Renderer::Impl& a_Renderer, const SubsystemsLibrary& a_Subsystems) override;
    OGLBindings globalBindings;
    std::vector<MeshInfo> opaque;
    std::vector<MeshInfo> blended;
    std::shared_ptr<OGLTexture> brdfLut;
    std::shared_ptr<OGLSampler> brdfLutSampler;

private:
    const std::shared_ptr<Primitive>& LoadPrimitive(Renderer::Impl& a_Renderer, Msg::MeshPrimitive* const a_Primitive);
    PrimitiveCache primitiveCache;
};
}