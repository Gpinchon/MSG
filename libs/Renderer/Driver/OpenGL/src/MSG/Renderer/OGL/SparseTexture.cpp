#include <MSG/Renderer/OGL/SparseTexture.hpp>
#include <MSG/Renderer/OGL/SparseTexturePageCache.hpp>
#include <MSG/Renderer/OGL/ToGL.hpp>

#include <MSG/Debug.hpp>
#include <MSG/ImageUtils.hpp>
#include <MSG/OGLContext.hpp>
#include <MSG/OGLTexture2D.hpp>
#include <MSG/Tools/HashCombine.hpp>

#include <GL/glew.h>

#include <map>
#include <span>

#include <glm/gtx/hash.hpp>

inline size_t std::hash<glm::uvec4>::operator()(glm::uvec4 const& a_Val) const
{
    size_t seed = 0;
    std::hash<uint32_t> hasher;
    MSG_HASH_COMBINE(seed, hasher(a_Val.x));
    MSG_HASH_COMBINE(seed, hasher(a_Val.y));
    MSG_HASH_COMBINE(seed, hasher(a_Val.z));
    MSG_HASH_COMBINE(seed, hasher(a_Val.w));
    return seed;
}

bool IsCompSparseTexturesSupported(MSG::OGLContext& a_Ctx)
{
    static bool queried      = false;
    static int32_t pageSizes = 0;
    if (!queried) {
        MSG::ExecuteOGLCommand(a_Ctx, [&pageSizes = pageSizes]() mutable { glGetInternalformativ(GL_TEXTURE_2D, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_NUM_VIRTUAL_PAGE_SIZES_ARB, 1, &pageSizes); }, true);
        if (pageSizes == 0)
            errorWarning("Compressed sparse textures unsupported, compressed textures will be decompressed on the fly!");
        queried = true;
    }
    return false;
    return pageSizes > 0;
}

glm::uvec3 GetPageSize(MSG::OGLContext& a_Ctx, const uint32_t& a_Target, const uint32_t& a_SizedFormat)
{
    static std::map<std::pair<uint32_t, uint32_t>, glm::ivec3> s_PageSize;
    auto key = std::make_pair(a_Target, a_SizedFormat);
    auto itr = s_PageSize.find(key);
    if (itr == s_PageSize.end()) {
        itr = s_PageSize.emplace(key, glm::ivec3(0)).first;
        MSG::ExecuteOGLCommand(a_Ctx, [&pageSize = itr->second, &a_Target, &a_SizedFormat]() mutable {
            glGetInternalformativ(a_Target, a_SizedFormat, GL_VIRTUAL_PAGE_SIZE_X_ARB, 1, &pageSize.x);
            glGetInternalformativ(a_Target, a_SizedFormat, GL_VIRTUAL_PAGE_SIZE_Y_ARB, 1, &pageSize.y);
            glGetInternalformativ(a_Target, a_SizedFormat, GL_VIRTUAL_PAGE_SIZE_Z_ARB, 1, &pageSize.z); }, true);
    }
    return itr->second;
}

uint32_t GetMaxMips(MSG::OGLContext& a_Ctx, const MSG::OGLTexture& a_Txt)
{
    uint32_t maxLvls = 0;
    MSG::ExecuteOGLCommand(a_Ctx, [&a_Txt, &maxLvls]() mutable { glGetTextureParameterIuiv(a_Txt, GL_NUM_SPARSE_LEVELS_ARB, &maxLvls); }, true);
    return maxLvls + 1;
}

auto GetSparseTextureInfo(const MSG::Texture& a_Src, const bool a_CompressedSparseSupported, const bool a_Sparse)
{
    uint32_t format = MSG::Renderer::ToGL(a_Src.GetPixelDescriptor().GetSizedFormat());
    if (!a_CompressedSparseSupported && a_Sparse && format == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT)
        format = GL_RGBA8; // if the driver doesn't support compressed sparse textures, we will decompress on the fly
    return MSG::OGLTextureInfo {
        .target      = uint32_t(MSG::Renderer::ToGL(a_Src.GetType())),
        .width       = a_Src.GetSize().x,
        .height      = a_Src.GetSize().y,
        .depth       = a_Src.GetSize().z,
        .levels      = uint32_t(a_Src.size()),
        .sizedFormat = format,
        .sparse      = a_Sparse
    };
}

template <typename T>
static inline T RoundUp(const T& numToRound, const T& multiple)
{
    double fnum = numToRound;
    double fmul = multiple;
    return static_cast<T>((1 + (numToRound - 1) / multiple) * multiple);
}

static inline glm::uvec3 RoundUp(const glm::uvec3& a_Val, const glm::uvec3& a_Multiple)
{
    return {
        RoundUp(a_Val[0], a_Multiple[0]),
        RoundUp(a_Val[1], a_Multiple[1]),
        RoundUp(a_Val[2], a_Multiple[2]),
    };
}

MSG::Renderer::SparseTexturePages::SparseTexturePages(
    const std::shared_ptr<MSG::Texture>& a_Src,
    const glm::uvec3& a_PageSize,
    const size_t& a_NumLevels)
    : pageSize(a_PageSize)
    , pageResolution(glm::max(RoundUp(a_Src->GetSize(), pageSize) / pageSize, 1u))
{
    auto pageCount = pageResolution.x * pageResolution.y * pageResolution.z;
    pendingPages.reserve(pageCount * a_NumLevels);
    residentPages.reserve(pageCount * a_NumLevels);
}

bool MSG::Renderer::SparseTexturePages::Request(
    const uint32_t& a_MinMip, const uint32_t& a_MaxMip,
    const glm::vec3& a_UVStart, const glm::vec3& a_UVEnd)
{
    bool anyMissing = false;
    auto now        = std::chrono::system_clock::now();
    for (int64_t level = a_MaxMip - 1; level >= a_MinMip; level--) { // prioritize the lowest levels to reduce pop-in
        auto levelPageRes = glm::vec3(GetLevelPageRes(level));
        auto pageStart    = glm::uvec3(glm::clamp(levelPageRes * a_UVStart, glm::vec3(0.f), levelPageRes));
        auto pageEnd      = glm::uvec3(glm::clamp(levelPageRes * a_UVEnd + 1.f, glm::vec3(pageStart) + 1.f, levelPageRes));
        for (uint32_t z = pageStart.z; z < pageEnd.z; z++) {
            for (uint32_t y = pageStart.y; y < pageEnd.y; y++) {
                for (uint32_t x = pageStart.x; x < pageEnd.x; x++) {
                    glm::uvec4 pageAddress(x, y, z, level);
                    lastAccess[pageAddress] = now;
                    if (!residentPages.contains(pageAddress) && !pendingPages.contains(pageAddress)) {
                        pendingPages.insert(pageAddress);
                        anyMissing = true;
                    }
                }
            }
        }
    }
    return anyMissing;
}

MSG::Renderer::SparseTexture::SparseTexture(OGLContext& a_Ctx, const std::shared_ptr<MSG::Texture>& a_Src, const bool& a_Sparse, SparseTexturePageCache& a_PageCache)
    : OGLTexture(a_Ctx, GetSparseTextureInfo(*a_Src, IsCompSparseTexturesSupported(a_Ctx), a_Sparse))
    , src(a_Src)
    , sparseLevelsCount(GetMaxMips(context, *this))
    , pages(a_Src, GetPageSize(context, target, sizedFormat), sparseLevelsCount)
    , pageCache(a_PageCache)
{
    if (!sparse) {
        for (auto level = 0; level < a_Src->size(); level++)
            UploadLevel(level, *a_Src->at(level));
        return;
    }
    // always commit the tail mips
    auto lastSparseLevel = sparseLevelsCount - 1;
    auto lastLevel       = std::min(uint32_t(src->size()) - 1, lastSparseLevel);
    auto& srcImage       = src->at(lastLevel);
    auto pageRes         = pages.GetLevelPageRes(lastLevel) * pages.pageSize;
    pageRes              = glm::min(pageRes, srcImage->GetSize()); // in case the texture is smaller than pageSize
    OGLTextureCommitInfo commitInfo {
        .level   = lastLevel,
        .offsetX = 0,
        .offsetY = 0,
        .offsetZ = 0,
        .width   = pageRes.x,
        .height  = pageRes.y,
        .depth   = pageRes.z,
        .commit  = true
    };
    OGLTexture::CommitPage(commitInfo);

    for (auto level = lastSparseLevel; level < src->size(); level++) {
        auto pageRes = pages.GetLevelPageRes(level);
        for (auto z = 0u; z < pageRes.z; z++) {
            for (auto y = 0u; y < pageRes.y; y++) {
                for (auto x = 0u; x < pageRes.x; x++) {
                    pages.Commit({ x, y, z, level });
                    UploadPage({ x, y, z, level });
                }
            }
        }
    }
}

bool MSG::Renderer::SparseTexture::RequestPages(
    const uint32_t& a_MinLevel, const uint32_t& a_MaxLevel,
    const glm::vec3& a_UVStart, const glm::vec3& a_UVEnd)
{
    auto minLvl = glm::clamp(a_MinLevel, 0u, uint32_t(sparseLevelsCount - 1));
    auto maxLvl = glm::clamp(a_MaxLevel, minLvl + 1, uint32_t(sparseLevelsCount - 1));
    if (minLvl == sparseLevelsCount - 1) // last levels are always commited
        return false;
    return pages.Request(minLvl, maxLvl, a_UVStart, a_UVEnd);
}

typedef std::chrono::milliseconds ms;

std::chrono::milliseconds MSG::Renderer::SparseTexture::CommitPendingPages(const std::chrono::milliseconds& a_RemainingTime)
{
    auto startTime    = std::chrono::steady_clock::now();
    auto elapsed      = ms(0u);
    auto pendingPages = pages.pendingPages; // do a local copy
    for (auto& pendingPage : pendingPages) {
        CommitPage(pendingPage);
        UploadPage(pendingPage);
        elapsed += std::chrono::duration_cast<ms>(std::chrono::steady_clock::now() - startTime);
        if (elapsed > a_RemainingTime)
            break;
    }
    return elapsed;
}

void MSG::Renderer::SparseTexture::FreeUnusedPages()
{
    auto now          = std::chrono::system_clock::now();
    auto pageAccesses = pages.lastAccess; // make a local copy
    for (auto& pageAccess : pageAccesses) {
        if (pageAccess.first.w >= sparseLevelsCount - 1)
            continue;
        if (now - pageAccess.second >= PageLifeExpetency)
            FreePage(pageAccess.first);
    }
}

void MSG::Renderer::SparseTexture::UploadPage(const glm::uvec4& a_PageAddress)
{
    auto textureLevel      = std::min(uint32_t(src->size() - 1), uint32_t(a_PageAddress.w));
    auto& srcImage         = src->at(textureLevel);
    glm::uvec4 pagesStart  = a_PageAddress;
    glm::uvec4 pagesEnd    = pagesStart + 1u;
    glm::uvec3 texelStart  = glm::uvec3(pagesStart) * pages.pageSize;
    glm::uvec3 texelEnd    = glm::uvec3(pagesEnd) * pages.pageSize;
    texelEnd               = glm::min(texelEnd, srcImage->GetSize()); // in case the texture is smaller than pageSize
    glm::uvec3 texelExtent = texelEnd - texelStart;
    auto pageCacheData     = pageCache.GetCache(this, a_PageAddress);
    OGLTextureUploadInfo info {
        .level           = textureLevel,
        .offsetX         = texelStart.x,
        .offsetY         = texelStart.y,
        .offsetZ         = texelStart.z,
        .width           = texelExtent.x,
        .height          = texelExtent.y,
        .depth           = texelExtent.z,
        .pixelDescriptor = srcImage->GetPixelDescriptor(),
    };
    if (!IsCompSparseTexturesSupported(context) && src->GetPixelDescriptor().GetSizedFormat() == MSG::PixelSizedFormat::DXT5_RGBA) {
        info.pixelDescriptor = MSG::PixelSizedFormat::Uint8_NormalizedRGBA;
        if (pageCacheData == nullptr)
            pageCacheData = pageCache.AddCache(this, a_PageAddress, ImageDecompress(*srcImage, texelStart, texelExtent));
    } else {
        if (pageCacheData == nullptr)
            pageCacheData = pageCache.AddCache(this, a_PageAddress, srcImage->Read(texelStart, texelExtent));
    }
    UploadLevel(info, *pageCacheData);
}

void MSG::Renderer::SparseTexture::CommitPage(const glm::uvec4& a_PageAddress)
{
    auto textureLevel      = std::min(uint32_t(src->size() - 1), uint32_t(a_PageAddress.w));
    auto& srcImage         = src->at(textureLevel);
    glm::uvec4 pagesStart  = a_PageAddress;
    glm::uvec4 pagesEnd    = pagesStart + 1u;
    glm::uvec3 texelStart  = glm::uvec3(pagesStart) * pages.pageSize;
    glm::uvec3 texelEnd    = glm::uvec3(pagesEnd) * pages.pageSize;
    texelEnd               = glm::min(texelEnd, srcImage->GetSize()); // in case the texture is smaller than pageSize
    glm::uvec3 texelExtent = texelEnd - texelStart;
    OGLTextureCommitInfo commitInfo {
        .level   = textureLevel,
        .offsetX = texelStart.x,
        .offsetY = texelStart.y,
        .offsetZ = texelStart.z,
        .width   = texelExtent.x,
        .height  = texelExtent.y,
        .depth   = texelExtent.z,
        .commit  = true
    };
    pages.Commit(a_PageAddress);
    OGLTexture::CommitPage(commitInfo);
}

void MSG::Renderer::SparseTexture::FreePage(const glm::uvec4& a_PageAddress)
{
    auto textureLevel      = std::min(uint32_t(src->size() - 1), uint32_t(a_PageAddress.w));
    auto& srcImage         = src->at(textureLevel);
    glm::uvec4 pagesStart  = a_PageAddress;
    glm::uvec4 pagesEnd    = pagesStart + 1u;
    glm::uvec3 texelStart  = glm::uvec3(pagesStart) * pages.pageSize;
    glm::uvec3 texelEnd    = glm::uvec3(pagesEnd) * pages.pageSize;
    texelEnd               = glm::min(texelEnd, srcImage->GetSize()); // in case the texture is smaller than pageSize
    glm::uvec3 texelExtent = texelEnd - texelStart;
    OGLTextureCommitInfo commitInfo {
        .level   = textureLevel,
        .offsetX = texelStart.x,
        .offsetY = texelStart.y,
        .offsetZ = texelStart.z,
        .width   = texelExtent.x,
        .height  = texelExtent.y,
        .depth   = texelExtent.z,
        .commit  = false
    };
    pages.Free(a_PageAddress);
    OGLTexture::CommitPage(commitInfo);
}
