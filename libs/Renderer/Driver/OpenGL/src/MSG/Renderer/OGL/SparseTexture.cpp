#include <MSG/Renderer/OGL/SparseTexture.hpp>

#include <MSG/OGLContext.hpp>
#include <MSG/OGLTexture2D.hpp>
#include <MSG/Tools/HashCombine.hpp>

#include <GL/glew.h>

#include <map>

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

auto GetSparseTextureInfo(const MSG::Texture& a_Src)
{
    return MSG::OGLTexture2DInfo {
        .width  = a_Src.GetSize().x,
        .height = a_Src.GetSize().y,
        .levels = uint32_t(a_Src.size()),
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
        auto pageStart    = glm::clamp(glm::uvec3(levelPageRes * a_UVStart), glm::uvec3(0u), glm::uvec3(levelPageRes));
        auto pageEnd      = glm::clamp(glm::uvec3(levelPageRes * a_UVEnd + 0.5f), pageStart + 1u, glm::uvec3(levelPageRes));
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

MSG::Renderer::SparseTexture::SparseTexture(OGLContext& a_Ctx, const std::shared_ptr<MSG::Texture>& a_Src)
    : SparseTexture(std::make_shared<OGLTexture2D>(a_Ctx, GetSparseTextureInfo(*a_Src)), a_Src)
{
}

MSG::Renderer::SparseTexture::SparseTexture(const std::shared_ptr<OGLTexture>& a_Txt, const std::shared_ptr<MSG::Texture>& a_Src)
    : sparseTexture(a_Txt)
    , src(a_Src)
    , sparseLevelsCount(GetMaxMips(a_Txt->context, *a_Txt))
    , pages(a_Src, GetPageSize(a_Txt->context, a_Txt->target, a_Txt->sizedFormat), sparseLevelsCount)
{
    // always commit the tail mips
    auto lastSparseLevel = sparseLevelsCount - 1;
    auto lastLevel       = std::max(uint32_t(src->size()), lastSparseLevel);
    for (auto level = lastSparseLevel; level < lastLevel; level++) {
        auto pageRes = pages.GetLevelPageRes(level);
        for (auto z = 0u; z < pageRes.z; z++) {
            for (auto y = 0u; y < pageRes.y; y++) {
                for (auto x = 0u; x < pageRes.x; x++) {
                    CommitPage({ x, y, z, level });
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

size_t MSG::Renderer::SparseTexture::CommitPendingPages(const size_t& a_RemainingBudget)
{
    size_t commitedPages = 0;
    auto pendingPages    = pages.pendingPages; // do a local copy
    for (auto& pendingPage : pendingPages) {
        CommitPage(pendingPage);
        if (commitedPages++ >= a_RemainingBudget)
            break;
    }
    return commitedPages;
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
    OGLTextureCommitInfo info {
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
    sparseTexture->CommitPage(info);
    sparseTexture->UploadLevel(textureLevel, texelStart, texelExtent, *srcImage);
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
    OGLTextureCommitInfo info {
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
    sparseTexture->CommitPage(info);
}
