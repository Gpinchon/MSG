#pragma once

#include <MSG/Renderer/SubsystemInterface.hpp>

#include <MSG/ThreadPool.hpp>

#include <Bindings.glsl>

#include <atomic>
#include <memory>
#include <unordered_set>

#include <glm/vec2.hpp>

namespace Msg::Renderer {
class VirtualTexture;
}

namespace Msg::Renderer {
constexpr uint32_t VTMaxBakingJobs       = 512;
constexpr uint32_t VTMaxRequestsPerFrame = 32;
class TexturingSubsystem : public SubsystemInterface {
public:
    TexturingSubsystem(Renderer::Impl& a_Rdr);
    void Load(Renderer::Impl& a_Rdr, const ECS::DefaultRegistry::EntityRefType& a_Entity) override;
    void Unload(Renderer::Impl& a_Rdr, const ECS::DefaultRegistry::EntityRefType& a_Entity) override;
    void Update(Renderer::Impl& a_Rdr, const SubsystemsLibrary& a_Subsystems) override;

private:
    void _FetchUsedPages(Renderer::Impl& a_Rdr);
    void _UploadPages(Renderer::Impl& a_Rdr);
    WorkerThread _pagesBakingThread;
    ThreadPool _feedbackThreadPool = { SAMPLERS_MATERIAL_COUNT };
    std::unordered_set<std::shared_ptr<VirtualTexture>> _managedTextures;
    std::atomic<bool> _pagesUploaded = true;
};
}