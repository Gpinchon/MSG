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

#include <GL/glew.h>
#include <VirtualTexturing.glsl>

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

constexpr glm::vec3 pageSize(VT_PAGE_SIZE, VT_PAGE_SIZE, 1);

Msg::Renderer::VirtualTexture::VirtualTexture(
    OGLContext& a_Ctx, const std::shared_ptr<Msg::Texture>& a_Src,
    VTPageCache& a_PageCache, VTPool& a_Pool)
    : _pool(a_Pool)
    , _src(a_Src)
    , pageCache(a_PageCache)
{
    // figure out the number of pages
    {
        glm::vec3 virtualSize = a_Src->GetSize();
        glm::vec3 pageRes     = ceil(virtualSize / pageSize);
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
            glm::vec3 pageRatio = lvlSize / pageSize;
            if (glm::all(glm::lessThanEqual(pageRatio, glm::vec3(1.f))))
                break;
            lastVirtLvl++;
        }
        _virtualPageSize  = glm::vec3(_src->GetSize()) / glm::vec3(_pageRes);
        _needsResize      = pageSize != glm::vec3(_virtualPageSize);
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
    CommitPage(_localPages.size() - 1);
    UploadPage(_localPages.size() - 1);
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
    auto& localPage      = _localPages[a_PageID];
    localPage.accessTime = std::chrono::system_clock::now();
    if (!localPage.commited) {
        _requestedPages.insert(a_PageID);
        return true;
    }
    return false;
}

typedef std::chrono::milliseconds ms;

std::chrono::milliseconds Msg::Renderer::VirtualTexture::CommitPendingPages(const std::chrono::milliseconds& a_RemainingTime)
{
    auto startTime = std::chrono::steady_clock::now();
    auto elapsed   = ms(0u);
    std::vector<uint32_t> pages(_requestedPages.begin(), _requestedPages.end());
    _requestedPages.clear();
    for (auto& pageIndex : pages) {
        CommitPage(pageIndex);
        UploadPage(pageIndex);
        elapsed += std::chrono::duration_cast<ms>(std::chrono::steady_clock::now() - startTime);
        if (elapsed > a_RemainingTime)
            break;
    }
    return elapsed;
}

void Msg::Renderer::VirtualTexture::FreeUnusedPages()
{
    auto now = std::chrono::system_clock::now();
    std::vector<uint32_t> pages(_commitedPages.begin(), _commitedPages.end());
    for (auto& pageIndex : pages) {
        if (now - _localPages[pageIndex].accessTime >= PageLifeExpetency)
            FreePage(pageIndex);
    }
}

void Msg::Renderer::VirtualTexture::UploadPage(const uint32_t& a_PageID)
{
    auto& localPage = _localPages[a_PageID];
    if (!localPage.commited)
        return; // this page could not be commited
    auto& srcImage     = _src->at(localPage.level);
    auto pageCacheData = pageCache.GetCache(this, a_PageID);
    if (pageCacheData == nullptr) {
        glm::vec3 virtualLvlSize = _GetSrcSize(localPage.level);
        glm::vec3 virtualPixBeg  = glm::floor(glm::vec3(localPage.pageCoords) * _virtualPageSize);
        glm::vec3 virtualPixEnd  = glm::min(glm::ceil(virtualPixBeg + _virtualPageSize), virtualLvlSize);
        glm::vec3 virtualPixSize = virtualPixEnd - virtualPixBeg;
        std::vector<std::byte> rawData;

        if (_src->GetCompressed()) {
            // decompression always decompresses to RGBA8
            srcImage->Map(virtualPixBeg, virtualPixSize);
            rawData = ImageDecompress(*srcImage, virtualPixBeg, virtualPixSize);
            srcImage->Unmap();
        } else {
            auto& pxDsc = srcImage->GetPixelDescriptor();
            rawData     = srcImage->Read(virtualPixBeg, virtualPixSize);
            if (pxDsc != Msg::PixelSizedFormat::Uint8_NormalizedRGBA)
                rawData = ImageConvert(rawData, pxDsc, virtualPixSize, Msg::PixelSizedFormat::Uint8_NormalizedRGBA);
        }
        if (_needsResize) // this texture size is not a multiple of page size
            rawData = ImageResize(rawData, Msg::PixelSizedFormat::Uint8_NormalizedRGBA, virtualPixSize, pageSize);
        pageCacheData = pageCache.AddCache(this, a_PageID, rawData);
    }
    _pool.UploadPage(localPage.atlasPage, (std::byte*)pageCacheData->data());
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
    if (currentPage.commited)
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

void Msg::Renderer::VirtualTexture::CommitPage(const uint32_t& a_PageID)
{
    auto atlasPage = _pool.RequestPage();
    if (atlasPage == VTNoPage)
        return;
    auto& page            = _localPages[a_PageID];
    glm::u8vec4 pageValue = glm::u8vec4(atlasPage, page.level, 0);
    UpdateFallbackPage(_localPages, _pageTableTexture, a_PageID, &pageValue);
    page.atlasPage = atlasPage;
    page.commited  = true;
    _commitedPages.insert(a_PageID);
}

uint32_t FindFallbackPage(
    std::vector<Msg::Renderer::VTLocalPage>& a_Pages,
    const uint32_t& a_PageID)
{
    if (a_PageID == -1u)
        return -1u;
    auto& page = a_Pages[a_PageID];
    if (page.commited)
        return a_PageID;
    return FindFallbackPage(a_Pages, page.topPage);
}

void Msg::Renderer::VirtualTexture::FreePage(const uint32_t& a_PageID)
{
    auto& page = _localPages[a_PageID];
    if (page.level == (GetLevels() - 1) || !page.commited)
        return; // last virtual level is always commited
    _pool.ReleasePage(page.atlasPage);
    page.atlasPage        = VTNoPage;
    page.commited         = false;
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
    return GetPageTableSize(a_Lvl) * glm::uvec3(pageSize);
}

glm::uvec3 Msg::Renderer::VirtualTexture::GetPageTableSize(const uint8_t& a_Lvl) const
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
