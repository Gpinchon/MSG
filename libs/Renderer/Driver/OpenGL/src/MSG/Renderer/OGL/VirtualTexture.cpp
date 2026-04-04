#include <MSG/Renderer/OGL/ToGL.hpp>
#include <MSG/Renderer/OGL/VTPageCache.hpp>
#include <MSG/Renderer/OGL/VTPool.hpp>
#include <MSG/Renderer/OGL/VirtualTexture.hpp>

#include <MSG/Debug.hpp>
#include <MSG/ImageUtils.hpp>
#include <MSG/OGLBindlessTextureSampler.hpp>
#include <MSG/OGLContext.hpp>
#include <MSG/OGLTexture2D.hpp>
#include <MSG/TextureUtils.hpp>
#include <MSG/WorkerThread.hpp>

#include <GL/glew.h>
#include <VirtualTexturing.glsl>

#include <span>

constexpr glm::vec3 s_VTPageSize(VT_PAGE_SIZE, VT_PAGE_SIZE, 1);

static void SetPageState(Msg::Renderer::VTPageState& a_InputState,
    const Msg::Renderer::VTPageState& a_ExpectedState, const Msg::Renderer::VTPageState& a_OutputState)
{
    assert(a_InputState == a_ExpectedState);
    a_InputState = a_OutputState;
}

inline uint32_t To1D(const glm::uvec3& a_Coords, const glm::uvec3& a_Max)
{
    return (a_Coords.z * a_Max.x * a_Max.y) + (a_Coords.y * a_Max.x) + a_Coords.x;
}

inline glm::uvec3 To3D(uint32_t a_Index, const glm::uvec3& a_Max)
{
    uint32_t z = a_Index / (a_Max.x * a_Max.y);
    a_Index    = a_Index - (z * a_Max.x * a_Max.y);
    uint32_t y = a_Index / a_Max.x;
    uint32_t x = a_Index % a_Max.x;
    return { x, y, z };
}

// go down the pyramid and inform lower levels this page is commited if necessary
void UpdateFallbackPage(
    std::vector<Msg::Renderer::VTLocalPage>& a_Pages,
    const std::shared_ptr<Msg::OGLTexture>& a_PageTable,
    const uint32_t& a_PageID,
    glm::u8vec4* a_FallbackPageValue)
{
    if (a_PageID == -1u)
        return;
    auto& currentPage = a_Pages[a_PageID];
    if (currentPage.state == Msg::Renderer::VTPageState::Commited)
        return;
    for (auto& bottomPageI : currentPage.bottomPages) {
        UpdateFallbackPage(a_Pages, a_PageTable, bottomPageI, a_FallbackPageValue);
    }
    Msg::OGLTextureUploadInfo uploadInfo;
    uploadInfo.width   = 1;
    uploadInfo.height  = 1;
    uploadInfo.depth   = 1;
    uploadInfo.offsetX = currentPage.pageCoords.x;
    uploadInfo.offsetY = currentPage.pageCoords.y;
    uploadInfo.offsetZ = currentPage.pageCoords.z;
    uploadInfo.level   = currentPage.level;
    uploadInfo.format  = GL_RGBA_INTEGER;
    uploadInfo.type    = GL_UNSIGNED_BYTE;
    a_PageTable->UploadLevel(uploadInfo, a_FallbackPageValue);
}

// go up the pyramid to find a commited page
uint32_t FindFallbackPage(
    std::vector<Msg::Renderer::VTLocalPage>& a_Pages,
    const uint32_t& a_PageID)
{
    if (a_PageID == -1u)
        return -1u;
    auto& page = a_Pages[a_PageID];
    if (page.state == Msg::Renderer::VTPageState::Commited)
        return a_PageID;
    return FindFallbackPage(a_Pages, page.topPage);
}

std::shared_ptr<Msg::OGLTexture> CreatePageTable(
    Msg::OGLContext& a_Context,
    const uint32_t& a_Target,
    const uint32_t& a_Width, const uint32_t& a_Height, const uint32_t& a_Depth,
    const uint32_t& a_Levels)
{
    Msg::OGLTextureInfo info;
    info.target      = a_Target;
    info.width       = a_Width;
    info.height      = a_Height;
    info.depth       = a_Depth;
    info.levels      = a_Levels;
    info.sizedFormat = GL_RGBA8UI;
    info.sparse      = false;
    auto texture     = std::make_shared<Msg::OGLTexture>(a_Context, info);
    glm::u8vec4 clearValue(0, 0, a_Levels, 0); // tail mips are always available
    for (uint32_t lvl = 0; lvl < a_Levels; lvl++)
        texture->Clear(GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, lvl, &clearValue);
    return texture;
}

std::vector<std::byte> GetPage(const Msg::Image& a_Src, const Msg::Sampler3D& a_Sampler,
    const glm::vec3& a_PageCoord,
    const glm::vec3& a_SrcSize, const glm::vec3& a_DstSize)
{
    glm::vec3 borderSize   = glm::vec3(VT_BORDER_WIDTH, VT_BORDER_WIDTH, 0);
    glm::vec3 srcTexExtent = a_SrcSize + (borderSize * 2.f);
    uint32_t srcBufSize    = srcTexExtent.x * srcTexExtent.y * srcTexExtent.z;
    std::vector<std::byte> res(srcBufSize * sizeof(glm::u8vec4), std::byte(0));
    std::span<glm::u8vec4> resu8v4(reinterpret_cast<glm::u8vec4*>(res.data()), srcBufSize);
    glm::vec3 pageBeg = (a_PageCoord + 0.f) * a_SrcSize;
    glm::vec3 pageEnd = (a_PageCoord + 1.f) * a_SrcSize;
    // fetch page
    {
        for (uint32_t z = 0; z < uint32_t(a_SrcSize.z); z++) {
            uint32_t texZ = pageBeg.z + z;
            for (uint32_t y = 0; y < uint32_t(a_SrcSize.y); y++) {
                uint32_t texY     = pageBeg.y + y;
                glm::uvec3 mapBeg = glm::max(glm::vec3(pageBeg.x, texY + 0, texZ + 0), 0.f);
                glm::uvec3 mapEnd = glm::min(glm::vec3(pageEnd.x, texY + 1, texZ + 1), glm::vec3(a_Src.GetSize()));
                a_Src.Map(mapBeg, mapEnd - mapBeg);
                for (uint32_t x = 0; x < uint32_t(a_SrcSize.x); x++) {
                    uint32_t texX     = pageBeg.x + x;
                    uint32_t pixIndex = To1D(borderSize + glm::vec3(x, y, z), srcTexExtent);
                    resu8v4[pixIndex] = a_Src.Load(glm::vec3(texX, texY, texZ)) * 255.f;
                }
                a_Src.Unmap();
            }
        }
    }
    // fill borders
    {
        glm::vec3 srcTexBeg = ((a_PageCoord + 0.f) * a_SrcSize) - borderSize;
        glm::vec3 srcTexEnd = ((a_PageCoord + 1.f) * a_SrcSize) + borderSize;
        for (uint32_t z = 0; z < uint32_t(srcTexExtent.z); z++) {
            uint32_t texZ = a_Sampler.WrapR(a_Src.GetSize().z, srcTexBeg.z + z);
            for (uint32_t y = 0; y < uint32_t(srcTexExtent.y); y++) {
                // top column
                {
                    uint32_t x        = 0;
                    uint32_t texX     = a_Sampler.WrapS(a_Src.GetSize().x, srcTexBeg.x + x);
                    uint32_t texY     = a_Sampler.WrapT(a_Src.GetSize().y, srcTexBeg.y + y);
                    uint32_t pixIndex = To1D(glm::uvec3(x, y, z), srcTexExtent);
                    a_Src.Map(glm::uvec3(texX, texY, texZ), glm::uvec3(1, 1, 1));
                    glm::ivec3 texCoord = glm::ivec3(texX, texY, texZ);
                    if (Msg::Sampler::IsClampedToBorder(a_Src, texCoord))
                        resu8v4[pixIndex] = a_Sampler.GetBorderColor() * 255.f;
                    else
                        resu8v4[pixIndex] = a_Src.Load(texCoord) * 255.f;
                    a_Src.Unmap();
                }
                // bottom column
                {
                    uint32_t x        = srcTexExtent.x - 1;
                    uint32_t texX     = a_Sampler.WrapS(a_Src.GetSize().x, srcTexBeg.x + x);
                    uint32_t texY     = a_Sampler.WrapT(a_Src.GetSize().y, srcTexBeg.y + y);
                    uint32_t pixIndex = To1D(glm::uvec3(x, y, z), srcTexExtent);
                    a_Src.Map(glm::uvec3(texX, texY, texZ), glm::uvec3(1, 1, 1));
                    glm::ivec3 texCoord = glm::ivec3(texX, texY, texZ);
                    if (Msg::Sampler::IsClampedToBorder(a_Src, texCoord))
                        resu8v4[pixIndex] = a_Sampler.GetBorderColor() * 255.f;
                    else
                        resu8v4[pixIndex] = a_Src.Load(texCoord) * 255.f;
                    a_Src.Unmap();
                }
            }
            for (uint32_t x = 1; x < uint32_t(srcTexExtent.x - 1); x++) {
                // top row
                {
                    uint32_t y        = 0;
                    uint32_t texX     = a_Sampler.WrapS(a_Src.GetSize().x, srcTexBeg.x + x);
                    uint32_t texY     = a_Sampler.WrapT(a_Src.GetSize().y, srcTexBeg.y + y);
                    uint32_t pixIndex = To1D(glm::uvec3(x, y, z), srcTexExtent);
                    a_Src.Map(glm::uvec3(texX, texY, texZ), glm::uvec3(1, 1, 1));
                    glm::ivec3 texCoord = glm::ivec3(texX, texY, texZ);
                    if (Msg::Sampler::IsClampedToBorder(a_Src, texCoord))
                        resu8v4[pixIndex] = a_Sampler.GetBorderColor() * 255.f;
                    else
                        resu8v4[pixIndex] = a_Src.Load(texCoord) * 255.f;
                    a_Src.Unmap();
                }
                // bottom row
                {
                    uint32_t y        = srcTexExtent.y - 1;
                    uint32_t texX     = a_Sampler.WrapS(a_Src.GetSize().x, srcTexBeg.x + x);
                    uint32_t texY     = a_Sampler.WrapT(a_Src.GetSize().y, srcTexBeg.y + y);
                    uint32_t pixIndex = To1D(glm::uvec3(x, y, z), srcTexExtent);
                    a_Src.Map(glm::uvec3(texX, texY, texZ), glm::uvec3(1, 1, 1));
                    glm::ivec3 texCoord = glm::ivec3(texX, texY, texZ);
                    if (Msg::Sampler::IsClampedToBorder(a_Src, texCoord))
                        resu8v4[pixIndex] = a_Sampler.GetBorderColor() * 255.f;
                    else
                        resu8v4[pixIndex] = a_Src.Load(texCoord) * 255.f;
                    a_Src.Unmap();
                }
            }
        }
    }
    return Msg::ImageResize(res, Msg::PixelSizedFormat::Uint8_NormalizedRGBA, srcTexExtent, a_DstSize);
}

Msg::Renderer::VirtualTexture::VirtualTexture(
    OGLContext& a_Ctx,
    const std::shared_ptr<Msg::Texture>& a_Src, const SamplerWrap& a_WrapS, const SamplerWrap& a_WrapT,
    VTPageCache& a_PageCache, VTPool& a_Pool)
    : _pool(a_Pool)
    , _src(a_Src)
    , pageCache(a_PageCache)
{
    _sampler.SetWrapS(a_WrapS);
    _sampler.SetWrapT(a_WrapT);
    _sampler.SetMagFilter(SamplerFilter::Nearest);
    _sampler.SetMinFilter(SamplerFilter::Nearest);
    // figure out the number of pages
    {
        glm::vec3 virtualSize = a_Src->GetSize();
        glm::vec3 pageRes     = ceil(virtualSize / s_VTPageSize);
        // we need a square texture despite what the specs say
        pageRes = glm::vec3(
            glm::max(pageRes.x, pageRes.y),
            glm::max(pageRes.x, pageRes.y),
            pageRes.z);
        // we also need the texture size to be a power of two to avoid fractional level
        pageRes  = glm::pow(glm::vec3(2.f), ceil(log(pageRes) / float(log(2))));
        _pageRes = pageRes;
    }
    // create page table
    {
        // figure out the number of virtual levels
        uint32_t lastVirtLvl = 0;
        while (lastVirtLvl < _src->GetLevels()) {
            glm::vec3 lvlSize   = _src->GetSize(lastVirtLvl);
            glm::vec3 pageRatio = lvlSize / s_VTPageSize;
            if (glm::all(glm::lessThanEqual(pageRatio, glm::vec3(1.f))))
                break;
            lastVirtLvl++;
        }
        _virtualPageSize  = glm::vec3(_src->GetSize()) / glm::vec3(_pageRes);
        _needsResize      = s_VTPageSize != glm::vec3(_virtualPageSize);
        _pageTableTexture = CreatePageTable(a_Ctx, ToGL(_src->GetType()),
            _pageRes.x, _pageRes.y, _pageRes.z,
            lastVirtLvl + 1);
    }
    // precompute local pages
    {
        uint32_t pageI = 0;
        for (uint32_t lvl = 0; lvl < GetLevels(); lvl++) {
            auto lvlPageCount = glm::max(_pageRes / uint32_t(exp2(lvl)), 1u);
            pageI += lvlPageCount.x * lvlPageCount.y * lvlPageCount.z;
        }
        _localPages.resize(pageI);
        uint32_t level              = 0;
        uint32_t levelMinIndex      = 0;
        uint32_t levelMaxIndex      = _pageRes.x * _pageRes.y * _pageRes.z;
        glm::uvec3 lvlPageRes       = _pageRes;
        glm::uvec3 bottomLvlPageRes = _pageRes;
        for (uint32_t pageIndex = 0; pageIndex < _localPages.size(); pageIndex++) {
            if (pageIndex >= levelMaxIndex) {
                level++;
                bottomLvlPageRes = lvlPageRes;
                lvlPageRes       = glm::max(lvlPageRes / 2u, 1u);
                levelMinIndex    = levelMaxIndex;
                levelMaxIndex    = levelMaxIndex + lvlPageRes.x * lvlPageRes.y * lvlPageRes.z;
            }
            uint32_t indexInsideLvl = pageIndex - levelMinIndex;
            glm::uvec3 pageCoords   = To3D(indexInsideLvl, lvlPageRes);
            glm::vec3 pageUV        = glm::vec3(pageCoords) / glm::vec3(lvlPageRes);
            if (level < GetLevels() - 1)
                _localPages[pageIndex].topPage = GetPageID(pageUV, level + 1);
            if (level > 0) {
                glm::vec3 bottomPageRes               = lvlPageRes * 2u;
                _localPages[pageIndex].bottomPages[0] = GetPageID(pageUV + glm::vec3(To3D(0, glm::uvec3(2, 2, 1))) / bottomPageRes, level - 1);
                _localPages[pageIndex].bottomPages[1] = GetPageID(pageUV + glm::vec3(To3D(1, glm::uvec3(2, 2, 1))) / bottomPageRes, level - 1);
                _localPages[pageIndex].bottomPages[2] = GetPageID(pageUV + glm::vec3(To3D(2, glm::uvec3(2, 2, 1))) / bottomPageRes, level - 1);
                _localPages[pageIndex].bottomPages[3] = GetPageID(pageUV + glm::vec3(To3D(3, glm::uvec3(2, 2, 1))) / bottomPageRes, level - 1);
            }
            _localPages[pageIndex].pageCoords = pageCoords;
            _localPages[pageIndex].level      = level;
        }
    }
    // always commit the last level
    _RequestMemory(_localPages.size() - 1);
    _CommitPage(_localPages.size() - 1);
    SetPageState(_localPages.back().state, VTPageState::Uncommited, VTPageState::Commited);
    _UploadPage(_localPages.size() - 1,
        GetPage(*_src->at(GetLevels() - 1), _sampler, glm::vec3(0), _virtualPageSize, s_VTPageSize));
}

Msg::Renderer::VirtualTexture::~VirtualTexture()
{
    for (auto& pageID : _commitedPages) {
        auto& page = _localPages[pageID];
        _pool.ReleasePage(page.atlasPage);
        pageCache.RemoveCache(this, pageID);
    }
}

bool Msg::Renderer::VirtualTexture::RequestPage(const uint32_t& a_PageID)
{
    if (a_PageID == -1u) // last levels are always commited
        return false;
    std::lock_guard lock(_mutex);
    auto& page      = _localPages[a_PageID];
    page.accessTime = std::chrono::system_clock::now();
    if (page.state != VTPageState::Uncommited)
        return false;
    SetPageState(page.state, VTPageState::Uncommited, VTPageState::Requested);
    _requestedPages.push_back(a_PageID);
    return true;
}

void Msg::Renderer::VirtualTexture::BakeRequestedPages(WorkerThread& a_WorkerThread)
{
    std::lock_guard lock(_mutex);
    auto now = std::chrono::system_clock::now();
    for (auto& pageID : _requestedPages) {
        auto& localPage    = _localPages[pageID];
        auto pageCacheData = pageCache.GetCache(this, pageID);
        if (pageCacheData != nullptr) {
            // there already is cache for this page, no baking required
            SetPageState(localPage.state, VTPageState::Requested, VTPageState::Baked);
            _bakedPages.emplace_back(pageID, pageCacheData);
            continue;
        }
        SetPageState(localPage.state, VTPageState::Requested, VTPageState::Baking);
        _bakingPages++;
        a_WorkerThread.PushCommand([&, pageID = pageID, enqueTime = now] {
            auto now = std::chrono::system_clock::now();
            if (now - enqueTime > BakingJobsExpiration) {
                // this job took to long, don't stall thread and return page to uncommited state
                std::lock_guard lock(_mutex);
                SetPageState(localPage.state, VTPageState::Baking, VTPageState::Uncommited);
            } else {
                const auto& srcImage           = _src->at(localPage.level);
                std::vector<std::byte> rawData = GetPage(*srcImage, _sampler,
                    glm::vec3(localPage.pageCoords), _virtualPageSize, glm::vec3(s_VTPageSize));
                std::lock_guard lock(_mutex);
                SetPageState(localPage.state, VTPageState::Baking, VTPageState::Baked);
                _bakedPages.emplace_back(pageID, pageCache.AddCache(this, pageID, rawData));
            }
            _bakingPages--;
        });
    }
    _requestedPages.clear();
}

void Msg::Renderer::VirtualTexture::UploadBakedPages()
{
    std::lock_guard lock(_mutex);
    auto now            = std::chrono::system_clock::now();
    auto pageExpiration = _pool.Full() ? EmergencyPageExpiration : PageExpiration; // if we need memory, free pages faster !
    for (const auto& bakedPage : _bakedPages) {
        auto& page = _localPages[bakedPage.pageID];
        if (now - _localPages[bakedPage.pageID].accessTime < pageExpiration) { // check if the page is not already dead
            _RequestMemory(bakedPage.pageID);
            if (page.atlasPage != VTNoPage) {
                _CommitPage(bakedPage.pageID);
                SetPageState(page.state, VTPageState::Baked, VTPageState::Commited);
                _UploadPage(bakedPage.pageID, *bakedPage.rawData);
            } else {
                SetPageState(page.state, VTPageState::Baked, VTPageState::Uncommited);
            }
        }
    }
    _bakedPages.clear();
}

void Msg::Renderer::VirtualTexture::FreeUnusedPages()
{
    std::lock_guard lock(_mutex);
    auto now            = std::chrono::system_clock::now();
    auto pageExpiration = _pool.Full() ? EmergencyPageExpiration : PageExpiration; // if we need memory, free pages faster !
    std::vector<uint32_t> pages(_commitedPages.begin(), _commitedPages.end());
    for (auto& pageIndex : pages) {
        if (now - _localPages[pageIndex].accessTime >= pageExpiration)
            _FreePage(pageIndex);
    }
}

bool Msg::Renderer::VirtualTexture::Empty() const
{
    return _requestedPages.empty() && _bakingPages == 0 && _bakedPages.empty() && _commitedPages.size() <= 1;
}

void Msg::Renderer::VirtualTexture::_UploadPage(const uint32_t& a_PageID, const std::vector<std::byte>& a_RawData)
{
    _pool.UploadPage(_localPages[a_PageID].atlasPage, (std::byte*)a_RawData.data());
}

void Msg::Renderer::VirtualTexture::_RequestMemory(const uint32_t& a_PageID)
{
    _localPages[a_PageID].atlasPage = _pool.RequestPage();
}

void Msg::Renderer::VirtualTexture::_CommitPage(const uint32_t& a_PageID)
{
    auto& page = _localPages[a_PageID];
    if (page.atlasPage != VTNoPage) {
        glm::u8vec4 pageValue = glm::u8vec4(page.atlasPage, page.level, 0);
        UpdateFallbackPage(_localPages, _pageTableTexture, a_PageID, &pageValue);
        _commitedPages.insert(a_PageID);
    }
}

void Msg::Renderer::VirtualTexture::_FreePage(const uint32_t& a_PageID)
{
    auto& page = _localPages[a_PageID];
    if (page.level == (GetLevels() - 1) || page.state != VTPageState::Commited)
        return; // last virtual level is always commited
    _pool.ReleasePage(page.atlasPage);
    page.atlasPage = VTNoPage;
    SetPageState(page.state, VTPageState::Commited, VTPageState::Uncommited);
    auto& tailPage        = _localPages.back();
    glm::u8vec4 pageValue = glm::u8vec4(tailPage.atlasPage, tailPage.level, 0);
    uint32_t fbPageID     = FindFallbackPage(_localPages, a_PageID);
    if (fbPageID != -1u) {
        auto& fbPage = _localPages[FindFallbackPage(_localPages, a_PageID)];
        pageValue    = glm::u8vec4(fbPage.atlasPage, fbPage.level, 0);
    }
    UpdateFallbackPage(_localPages, _pageTableTexture, a_PageID, &pageValue);
    _commitedPages.erase(a_PageID);
}

std::shared_ptr<Msg::OGLTexture> Msg::Renderer::VirtualTexture::GetPageTable() const
{
    return _pageTableTexture;
}

uint32_t Msg::Renderer::VirtualTexture::GetPageID(const glm::vec3& a_UV, const uint8_t& a_Lvl) const
{
    if (a_Lvl >= (GetLevels() - 1))
        return -1u; // last virtual level is always commited
    uint32_t index     = 0;
    glm::uvec3 pageRes = _GetPageRes(0);
    for (uint32_t lvl = 0; lvl < a_Lvl; lvl++) {
        index += pageRes.x * pageRes.y * pageRes.z;
        pageRes = glm::max(pageRes / 2u, 1u);
    }
    glm::vec3 pageCoord(glm::min(a_UV * glm::vec3(pageRes), glm::vec3(pageRes - 1u)));
    index += To1D(pageCoord, pageRes);
    assert(index < _localPages.size() && "Index out of bounds");
    return index;
}

glm::uvec3 Msg::Renderer::VirtualTexture::GetVirtualSize(const uint8_t& a_Lvl) const
{
    return _GetPageTableSize(a_Lvl) * glm::uvec3(s_VTPageSize);
}

glm::uvec3 Msg::Renderer::VirtualTexture::_GetPageTableSize(const uint8_t& a_Lvl) const
{
    glm::uvec3 res(_pageTableTexture->width, _pageTableTexture->height, _pageTableTexture->depth);
    return glm::max(res / uint32_t(exp2(a_Lvl)), 1u);
}

uint32_t Msg::Renderer::VirtualTexture::GetLevels() const
{
    return _pageTableTexture->levels;
}

glm::uvec3 Msg::Renderer::VirtualTexture::_GetPageRes(const uint32_t& a_Lvl) const
{
    return glm::max(_pageRes / uint32_t(exp2(a_Lvl)), 1u);
}

glm::uvec3 Msg::Renderer::VirtualTexture::_GetSrcSize(const uint32_t& a_Lvl) const
{
    return _src->GetSize(a_Lvl);
}
