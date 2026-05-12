#include <MSG/Debug.hpp>
#include <MSG/OGLTypedBuffer.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/Subsystems/TexturingSubsystem.hpp>
#include <MSG/Renderer/OGL/VirtualTexture.hpp>
#include <MSG/Renderer/Structs.hpp>

#include <span>

#include <glm/gtc/packing.hpp>

#include <Bindings.glsl>
#include <VirtualTexturing.glsl>

float GetLodBias(const Msg::Renderer::QualitySetting& a_TextureQuality)
{
    using enum Msg::Renderer::QualitySetting;
    float lodBias = 0;
    switch (a_TextureQuality) {
    case Low:
        lodBias = 2.0f;
        break;
    case Medium:
        lodBias = 1.5f;
        break;
    case High:
        lodBias = 1.0f;
        break;
    case VeryHigh:
        lodBias = 0.0f;
        break;
    default:
        MSGErrorFatal("Unknown texture quality setting");
        break;
    }
    return lodBias;
}

float GetMaxAniso(const Msg::Renderer::QualitySetting& a_FilteringQuality)
{
    using enum Msg::Renderer::QualitySetting;
    float maxAniso = 0;
    switch (a_FilteringQuality) {
    case Low:
        maxAniso = 0.0f;
        break;
    case Medium:
        maxAniso = 4.0f;
        break;
    case High:
        maxAniso = 8.0f;
        break;
    case VeryHigh:
        maxAniso = 16.0f;
        break;
    default:
        MSGErrorFatal("Unknown filtering quality setting");
        break;
    }
    return maxAniso;
}

uint32_t GetPoolPageCount(const Msg::Renderer::QualitySetting& a_MemoryBudget)
{
    using enum Msg::Renderer::QualitySetting;
    uint32_t pageCount = 0;
    switch (a_MemoryBudget) {
    case Low:
        pageCount = 32;
        break;
    case Medium:
        pageCount = 64;
        break;
    case High:
        pageCount = 128;
        break;
    case VeryHigh:
        pageCount = 256;
        break;
    default:
        MSGErrorFatal("Unknown memory budget setting");
        break;
    }
    return pageCount;
}

Msg::Renderer::TexturingSubsystem::TexturingSubsystem(Renderer::Impl& a_Rdr)
    : vtSettingsBuffer(std::make_shared<OGLTypedBuffer<GLSL::VTSettings>>(a_Rdr.context))
    , _feedbackThreadPool(SAMPLERS_MATERIAL_COUNT)
{
    GLSL::VTSettings settings = vtSettingsBuffer->Get();
    settings.lodBias          = GetLodBias(_currentSettings.quality);
    settings.maxAniso         = GetMaxAniso(_currentSettings.filtering);
    uint32_t pageCount        = GetPoolPageCount(_currentSettings.memoryBudget);
    a_Rdr.vtLoader.SetPageCount(a_Rdr.context, pageCount);
    vtSettingsBuffer->Set(settings);
    vtSettingsBuffer->Update();
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

void Msg::Renderer::TexturingSubsystem::UpdateSettings(Renderer::Impl& a_Renderer, const RendererSettings& a_Settings)
{
    GLSL::VTSettings settings = vtSettingsBuffer->Get();
    bool qualityChanged       = _currentSettings.quality != a_Settings.texture.quality;
    bool memoryBudgetChanged  = _currentSettings.memoryBudget != a_Settings.texture.memoryBudget;
    bool filteringChanged     = _currentSettings.filtering != a_Settings.texture.filtering;
    settings.lodBias          = GetLodBias(_currentSettings.quality);
    settings.maxAniso         = GetMaxAniso(_currentSettings.filtering);
    uint32_t pageCount        = GetPoolPageCount(_currentSettings.memoryBudget);
    if (memoryBudgetChanged) { // fully recreate pages pool
        _pagesBakingThread.Wait();
        a_Renderer.vtLoader.SetPageCount(a_Renderer.context, pageCount);
    } else if (qualityChanged || filteringChanged) { // reload textures
        _pagesBakingThread.Wait();
        for (auto& txt : _managedTextures) {
            txt->Clear();
            txt->Allocate();
        }
    }
    vtSettingsBuffer->Set(settings);
    vtSettingsBuffer->Update();
    _currentSettings = a_Settings.texture;
}

void Msg::Renderer::TexturingSubsystem::_FetchUsedPages(Renderer::Impl& a_Rdr)
{
    const auto now = std::chrono::system_clock::now();
    if (now - _lastUpdate < std::chrono::milliseconds(VTFeedbackUpdateMs))
        return;
    // avoid accumulating too many jobs
    if (_pagesBakingThread.PendingTaskCount() > VTMaxBakingJobs)
        return;
    if (feedbackData == nullptr)
        return;
    auto& feedbackRes = feedbackData->feedbackRes;
    auto& feedbackBuf = feedbackData->feedbackBuffer;
    // gather the used pages
    std::array<std::future<std::unordered_set<VirtualTexture*>>, SAMPLERS_MATERIAL_COUNT> jobs;
    // iterate over each texture layer (ie TexCoords)
    const uint32_t textureSize = feedbackRes.x * feedbackRes.y;
    for (uint8_t samplerI = 0; samplerI < feedbackRes.z; samplerI++) {
        auto spanBeg   = feedbackBuf.begin() + textureSize * (samplerI + 0);
        auto spanEnd   = feedbackBuf.begin() + textureSize * (samplerI + 1);
        jobs[samplerI] = _feedbackThreadPool.Enqueue([&, feedbackSpan = std::span<const glm::uvec3>(spanBeg, spanEnd)] {
            std::unordered_set<VirtualTexture*> managedTextures;
            for (auto& val : feedbackSpan) {
                auto sampler = reinterpret_cast<VirtualTexture*>(glm::packUint2x32({ val.x, val.y }));
                if (sampler == nullptr)
                    continue;
                auto uv               = glm::unpackUnorm4x8(val.z);
                auto level            = uv[3] * sampler->GetLevels();
                bool newPageRequested = false;
                newPageRequested |= sampler->RequestPage(sampler->GetPageID(glm::vec3(uv.x, uv.y, uv.z), floor(level + 0.0f)));
                newPageRequested |= sampler->RequestPage(sampler->GetPageID(glm::vec3(uv.x, uv.y, uv.z), floor(level + 1.0f)));
                if (newPageRequested)
                    managedTextures.insert(sampler);
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
