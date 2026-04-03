#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/VirtualTexture.hpp>

#include <MSG/Renderer/OGL/RenderPasses/PassVTFeedback.hpp>
#include <MSG/Renderer/OGL/Subsystems/TexturingSubsystem.hpp>

#include <span>

#include <glm/gtc/packing.hpp>

Msg::Renderer::TexturingSubsystem::TexturingSubsystem(Renderer::Impl& a_Rdr)
{
}

void Msg::Renderer::TexturingSubsystem::Load(Renderer::Impl& a_Rdr, const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
}

void Msg::Renderer::TexturingSubsystem::Unload(Renderer::Impl& a_Rdr, const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
}

void Msg::Renderer::TexturingSubsystem::Update(Renderer::Impl& a_Rdr, const SubsystemsLibrary& a_Subsystems)
{
    _FetchUsedPages(a_Rdr);
    _UploadPages(a_Rdr);
}

void Msg::Renderer::TexturingSubsystem::_FetchUsedPages(Renderer::Impl& a_Rdr)
{
    auto& feedbackPass = a_Rdr.renderPassesLibrary.Get<PassVTFeedback>();
    if (!feedbackPass.GetFeedbackReady())
        return;
    // gather the used pages
    std::array<std::future<std::unordered_set<VirtualTexture*>>, SAMPLERS_MATERIAL_COUNT> jobs;
    std::atomic_uint32_t requestedPages = 0;
    // iterate over each texture layer (ie TexCoords)
    const auto& feedbackBuff   = feedbackPass.GetFeedbackBuffer();
    const auto& feedbackRes    = feedbackPass.GetFeedbackRes();
    const uint32_t textureSize = feedbackRes.x * feedbackRes.y;
    for (uint8_t samplerI = 0; samplerI < feedbackRes.z; samplerI++) {
        auto spanBeg   = feedbackBuff.begin() + textureSize * (samplerI + 0);
        auto spanEnd   = feedbackBuff.begin() + textureSize * (samplerI + 1);
        jobs[samplerI] = _feedbackThreadPool.Enqueue([&, feedbackSpan = std::span<const glm::uvec3>(spanBeg, spanEnd)] {
            std::unordered_set<VirtualTexture*> managedTextures;
            for (auto& val : feedbackSpan) {
                auto sampler = reinterpret_cast<VirtualTexture*>(glm::packUint2x32({ val.x, val.y }));
                if (sampler == nullptr)
                    continue;
                auto uv              = glm::unpackUnorm4x8(val.z);
                auto level           = uv[3] * sampler->GetLevels();
                uint32_t curReqPages = 0;
                if (sampler->RequestPage(sampler->GetPageID(glm::vec3(uv.x, uv.y, uv.z), floor(level))))
                    curReqPages++;
                if (sampler->RequestPage(sampler->GetPageID(glm::vec3(uv.x, uv.y, uv.z), ceil(level))))
                    curReqPages++;
                if (curReqPages > 0)
                    managedTextures.insert(sampler);
                curReqPages = requestedPages.fetch_add(curReqPages, std::memory_order_acquire) + curReqPages;
                if (curReqPages >= VTMaxRequestsPerFrame)
                    break;
            }
            return managedTextures;
        });
    }
    // add managed textures to the managed textures list and start baking new pages
    for (auto& managedTextures : jobs) {
        for (auto& texture : managedTextures.get()) {
            texture->BakeRequestedPages(_pagesBakingThread);
            _managedTextures.insert(texture->shared_from_this());
        }
    }
    // avoid accumulating too many jobs
    while (_pagesBakingThread.PendingTaskCount() > VTMaxBakingJobs)
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void Msg::Renderer::TexturingSubsystem::_UploadPages(Renderer::Impl& a_Rdr)
{
    // early bail if the upload function did not run yet
    if (_pagesUploaded.load() && !_managedTextures.empty()) {
        _pagesUploaded.store(false);
        a_Rdr.context.PushCmd([this] {
            auto managedItr = _managedTextures.begin();
            while (managedItr != _managedTextures.end()) {
                auto& managedTxt = *managedItr;
                managedTxt->UploadBakedPages();
                managedTxt->FreeUnusedPages();
                if (managedTxt->Empty())
                    managedItr = _managedTextures.erase(managedItr);
                else
                    managedItr++;
            }
            _pagesUploaded.store(true);
        });
    }
}
