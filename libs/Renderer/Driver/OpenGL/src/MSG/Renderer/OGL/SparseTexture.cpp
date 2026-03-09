#include <MSG/Renderer/OGL/SparseTexture.hpp>
#include <MSG/Renderer/OGL/SparseTexturePageCache.hpp>
#include <MSG/Renderer/OGL/ToGL.hpp>

#include <MSG/Debug.hpp>
#include <MSG/ImageUtils.hpp>
#include <MSG/OGLContext.hpp>
#include <MSG/OGLTexture2D.hpp>
#include <MSG/TextureUtils.hpp>

#include <GL/glew.h>

#include <map>
#include <span>

#define VT_DISABLE_SPARSE false

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

Msg::Renderer::SparseTexture::SparseTexture(OGLContext& a_Ctx, const std::shared_ptr<Msg::Texture>& a_Src, SparseTexturePageCache& a_PageCache)
    : OGLTexture(a_Ctx)
    , _src(a_Src)
    , pageCache(a_PageCache)
{
    target          = ToGL(a_Src->GetType());
    sizedFormat     = GL_RGBA8;
    auto sparseInfo = OGLTexture::GetFormatSparseInfo(context, target, GL_RGBA8);
    _sparsePageSize = glm::uvec3(sparseInfo.pageWidth, sparseInfo.pageHeight, sparseInfo.pageDepth);

    // figure out the number of pages, if it's 1 or less, no need to use sparse texture
    glm::vec3 virtualSize = a_Src->GetSize();
    glm::vec3 pageRes     = virtualSize / glm::vec3(_sparsePageSize);
    if (VT_DISABLE_SPARSE || (pageRes.x <= 1 && pageRes.y <= 1 && pageRes.z <= 1)) {
        OGLTextureInfo info;
        info.target      = target;
        info.width       = a_Src->GetSize().x;
        info.height      = a_Src->GetSize().y;
        info.depth       = a_Src->GetSize().z;
        info.levels      = a_Src->GetLevels();
        info.sizedFormat = ToGL(a_Src->GetPixelDescriptor().GetSizedFormat());
        info.sparse      = false;
        Initialize(info);
        for (auto level = 0; level < levels; level++)
            UploadLevel(level, *a_Src->at(level));
        _src = nullptr; // no need to keep a reference to the source, it's a normal texture
        return;
    } else {
        // we need a square texture despite what the specs say
        pageRes = glm::vec3(
            glm::max(pageRes.x, pageRes.y),
            glm::max(pageRes.x, pageRes.y),
            pageRes.z);
        _src             = a_Src;
        _pageRes         = ceil(pageRes);
        _virtualPageSize = glm::vec3(_src->GetSize()) / ceil(pageRes);
        _needsResize     = _sparsePageSize != glm::uvec3(_virtualPageSize);
        // initialize texture
        {
            OGLTextureInfo info;
            info.target      = target;
            info.width       = _pageRes.x * _sparsePageSize.x;
            info.height      = _pageRes.y * _sparsePageSize.y;
            info.depth       = _pageRes.z * _sparsePageSize.z;
            info.levels      = a_Src->GetLevels();
            info.sizedFormat = sizedFormat;
            info.sparse      = true;
            Initialize(info);
        }
        _sparseLevelsCount = SparseLevels();
        // precompute local pages
        {
            uint32_t pageI = 0;
            for (uint32_t lvl = 0; lvl < _sparseLevelsCount; lvl++) {
                auto lvlPageCount = glm::max(_pageRes / uint32_t(exp2(lvl)), 1u);
                pageI += lvlPageCount.x * lvlPageCount.y * lvlPageCount.z;
            }
            _localPages.resize(pageI);
            uint32_t level         = 0;
            uint32_t levelMinIndex = 0;
            uint32_t levelMaxIndex = _pageRes.x * _pageRes.y * _pageRes.z;
            auto lvlPageRes        = _pageRes;
            for (uint32_t pageIndex = 0; pageIndex < _localPages.size(); pageIndex++) {
                if (pageIndex >= levelMaxIndex) {
                    level++;
                    lvlPageRes    = glm::max(lvlPageRes / 2u, 1u);
                    levelMinIndex = levelMaxIndex;
                    levelMaxIndex = levelMaxIndex + lvlPageRes.x * lvlPageRes.y * lvlPageRes.z;
                }
                uint32_t indexInsideLvl           = pageIndex - levelMinIndex;
                _localPages[pageIndex].pageCoords = To3D(indexInsideLvl, lvlPageRes);
                _localPages[pageIndex].level      = level;
            }
        }
        // always commit the tail mips
        for (auto lvl = _sparseLevelsCount; lvl < levels; lvl++) {
            auto sparseLvlSize = GetSparseSize(lvl);
            OGLTextureCommitInfo commitInfo;
            commitInfo.level  = lvl;
            commitInfo.width  = sparseLvlSize.x;
            commitInfo.height = sparseLvlSize.y;
            commitInfo.depth  = sparseLvlSize.z;
            commitInfo.commit = true;
            OGLTexture::CommitPage(commitInfo);

            auto& srcImage = _src->at(lvl);
            OGLTextureUploadInfo uploadInfo;
            uploadInfo.level  = lvl;
            uploadInfo.width  = sparseLvlSize.x;
            uploadInfo.height = sparseLvlSize.y;
            uploadInfo.depth  = sparseLvlSize.z;

            std::vector<std::byte> rawData;
            if (_src->GetCompressed()) {
                uploadInfo.pixelDescriptor = PixelSizedFormat::Uint8_NormalizedRGBA;
                srcImage->Map();
                rawData = ImageDecompress(*srcImage, glm::uvec3(0), srcImage->GetSize());
                srcImage->Unmap();
            } else {
                uploadInfo.pixelDescriptor = srcImage->GetPixelDescriptor();
                rawData                    = srcImage->Read();
            }
            if (_needsResize)
                rawData = ImageResize(rawData, uploadInfo.pixelDescriptor, srcImage->GetSize(), sparseLvlSize);
            OGLTexture::UploadLevel(uploadInfo, rawData);
        }
    }
}

bool Msg::Renderer::SparseTexture::RequestPage(const uint32_t& a_PageIndex)
{
    if (a_PageIndex == -1u) // last levels are always commited
        return false;
    auto& localPage      = _localPages[a_PageIndex];
    localPage.accessTime = std::chrono::system_clock::now();
    if (!localPage.commited) {
        _requestedPages.insert(a_PageIndex);
        return true;
    }
    return false;
}

typedef std::chrono::milliseconds ms;

std::chrono::milliseconds Msg::Renderer::SparseTexture::CommitPendingPages(const std::chrono::milliseconds& a_RemainingTime)
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

void Msg::Renderer::SparseTexture::FreeUnusedPages()
{
    auto now = std::chrono::system_clock::now();
    std::vector<uint32_t> pages(_commitedPages.begin(), _commitedPages.end());
    for (auto& pageIndex : pages) {
        auto& localPage = _localPages[pageIndex];
        if (localPage.level >= _sparseLevelsCount)
            continue;
        if (now - localPage.accessTime >= PageLifeExpetency)
            FreePage(pageIndex);
    }
}

void Msg::Renderer::SparseTexture::UploadPage(const uint32_t& a_PageIndex)
{
    auto& localPage    = _localPages[a_PageIndex];
    auto& srcImage     = _src->at(localPage.level);
    auto pageCacheData = pageCache.GetCache(this, a_PageIndex);
    OGLTextureUploadInfo info;
    if (_src->GetCompressed())
        info.pixelDescriptor = Msg::PixelSizedFormat::Uint8_NormalizedRGBA;
    else
        info.pixelDescriptor = srcImage->GetPixelDescriptor();
    if (pageCacheData == nullptr) {
        glm::vec3 virtualLvlSize = GetVirtualSize(localPage.level);
        glm::vec3 virtualPixBeg  = glm::floor(glm::vec3(localPage.pageCoords) * _virtualPageSize);
        glm::vec3 virtualPixEnd  = glm::min(glm::ceil(virtualPixBeg + _virtualPageSize), virtualLvlSize);
        glm::vec3 virtualPixSize = virtualPixEnd - virtualPixBeg;
        std::vector<std::byte> rawData;
        if (_src->GetCompressed()) {
            // decompression always decompresses to RGBA8
            srcImage->Map(virtualPixBeg, virtualPixSize);
            rawData = ImageDecompress(*srcImage, virtualPixBeg, virtualPixSize);
            srcImage->Unmap();
        } else
            rawData = srcImage->Read(virtualPixBeg, virtualPixSize);
        if (_needsResize) // this texture size is not a multiple of page size
            rawData = ImageResize(rawData, info.pixelDescriptor, virtualPixSize, _sparsePageSize);
        pageCacheData = pageCache.AddCache(this, a_PageIndex, rawData);
    }
    glm::uvec3 sparsePixBeg = localPage.pageCoords * _sparsePageSize;
    info.level              = localPage.level;
    info.offsetX            = sparsePixBeg.x;
    info.offsetY            = sparsePixBeg.y;
    info.offsetZ            = sparsePixBeg.z;
    info.width              = _sparsePageSize.x;
    info.height             = _sparsePageSize.y;
    info.depth              = _sparsePageSize.z;
    UploadLevel(info, *pageCacheData);
}

void Msg::Renderer::SparseTexture::CommitPage(const uint32_t& a_PageIndex)
{
    OGLTexture::CommitPage(_GetCommitInfo(a_PageIndex, true));
    _localPages[a_PageIndex].commited = true;
    _commitedPages.insert(a_PageIndex);
}

void Msg::Renderer::SparseTexture::FreePage(const uint32_t& a_PageIndex)
{
    OGLTexture::CommitPage(_GetCommitInfo(a_PageIndex, false));
    _localPages[a_PageIndex].commited = false;
    _commitedPages.erase(a_PageIndex);
}

uint32_t Msg::Renderer::SparseTexture::GetPageIndex(const uint32_t& a_Lvl, const glm::vec3& a_UV) const
{
    if (a_Lvl >= _sparseLevelsCount)
        return -1u; // this is not backed by any page
    uint32_t index     = 0;
    glm::uvec3 pageRes = _GetPageRes(0);
    for (uint32_t lvl = 0; lvl < a_Lvl; lvl++) {
        index += pageRes.x * pageRes.y * pageRes.z;
        pageRes = glm::max(pageRes / 2u, 1u);
    }
    glm::vec3 pageCoord(glm::min(a_UV * glm::vec3(pageRes), glm::vec3(pageRes - 1u)));
    index += To1D(pageCoord, pageRes);
    assert(index <= _localPages.size() && "Index out of bounds");
    return index;
}

Msg::OGLTextureCommitInfo Msg::Renderer::SparseTexture::_GetCommitInfo(const uint32_t& a_PageIndex, const bool& a_Commit) const
{
    auto& localPage          = _localPages[a_PageIndex];
    glm::uvec3 sparseLvlSize = GetSparseSize(localPage.level);
    glm::uvec3 pixelStart    = localPage.pageCoords * _sparsePageSize;
    glm::uvec3 pixelEnd      = glm::min(pixelStart + _sparsePageSize, sparseLvlSize);
    glm::uvec3 pixelSize     = pixelEnd - pixelStart;
    return {
        .level   = localPage.level,
        .offsetX = pixelStart.x,
        .offsetY = pixelStart.y,
        .offsetZ = pixelStart.z,
        .width   = pixelSize.x,
        .height  = pixelSize.y,
        .depth   = pixelSize.z,
        .commit  = a_Commit
    };
}

glm::uvec3 Msg::Renderer::SparseTexture::GetVirtualSize(const uint32_t& a_Lvl) const
{
    return glm::max(_src->GetSize() / uint32_t(exp2(a_Lvl)), 1u);
}

glm::uvec3 Msg::Renderer::SparseTexture::GetSparseSize(const uint32_t& a_Lvl) const
{
    return glm::max(glm::uvec3(width, height, depth) / uint32_t(exp2(a_Lvl)), 1u);
}

glm::uvec3 Msg::Renderer::SparseTexture::_GetPageRes(const uint32_t& a_Lvl) const
{
    return glm::max(_pageRes / uint32_t(exp2(a_Lvl)), 1u);
}
