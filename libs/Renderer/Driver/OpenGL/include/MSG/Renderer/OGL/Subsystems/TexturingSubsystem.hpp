#pragma once

#include <MSG/Renderer/Structs.hpp>
#include <MSG/Renderer/SubsystemInterface.hpp>

#include <MSG/ThreadPool.hpp>

#include <atomic>
#include <chrono>
#include <memory>
#include <unordered_set>

#include <glm/vec3.hpp>

namespace Msg {
template <typename>
class OGLTypedBuffer;
}

namespace Msg::Renderer::GLSL {
struct VTSettings;
}

namespace Msg::Renderer {
class VirtualTexture;
struct RendererSettings;
}

namespace Msg::Renderer {
constexpr uint32_t VTMaxBakingJobs    = 512;
constexpr uint32_t VTFeedbackUpdateMs = 32; // get feedback only 30 times/seconds
struct VTFeedbackData {
    glm::uvec3 feedbackRes;
    std::vector<glm::uvec3> feedbackBuffer;
};

class TexturingSubsystem : public SubsystemInterface {
public:
    TexturingSubsystem(Renderer::Impl& a_Rdr);
    void Load(Renderer::Impl& a_Rdr, const ECS::DefaultRegistry::EntityRefType& a_Entity) override;
    void Unload(Renderer::Impl& a_Rdr, const ECS::DefaultRegistry::EntityRefType& a_Entity) override;
    void Update(Renderer::Impl& a_Rdr, const SubsystemsLibrary& a_Subsystems) override;
    void UpdateSettings(Renderer::Impl& a_Renderer, const RendererSettings& a_Settings) override;
    std::shared_ptr<OGLTypedBuffer<GLSL::VTSettings>> vtSettingsBuffer;
    mutable VTFeedbackData* feedbackData = nullptr;

private:
    TextureSettings _currentSettings;
    void _FetchUsedPages(Renderer::Impl& a_Rdr);
    void _UploadPages(Renderer::Impl& a_Rdr);
    std::chrono::system_clock::time_point _lastUpdate;
    WorkerThread _pagesBakingThread;
    ThreadPool _feedbackThreadPool;
    std::unordered_set<std::shared_ptr<VirtualTexture>> _managedTextures;
    std::atomic<bool> _pagesUploaded = true;
};
}