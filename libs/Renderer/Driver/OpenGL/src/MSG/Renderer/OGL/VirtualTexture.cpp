#include <MSG/Renderer/OGL/VirtualTexture.hpp>

#include <MSG/OGLContext.hpp>
#include <MSG/OGLTexture2D.hpp>

#include <GL/glew.h>

#include <map>

#include <glm/gtx/hash.hpp>

inline size_t std::hash<glm::uvec4>::operator()(glm::uvec4 const& a_Val) const
{
    size_t seed = 0;
    std::hash<uint32_t> hasher;
    glm::detail::hash_combine(seed, hasher(a_Val.x));
    glm::detail::hash_combine(seed, hasher(a_Val.y));
    glm::detail::hash_combine(seed, hasher(a_Val.z));
    glm::detail::hash_combine(seed, hasher(a_Val.w));
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
    return maxLvls;
}

auto GetSparseTextureInfo(const MSG::Texture& a_Src)
{
    return MSG::OGLTexture2DInfo {
        .width  = a_Src.GetSize().x,
        .height = a_Src.GetSize().y,
        .levels = uint32_t(a_Src.size()),
    };
}

MSG::Renderer::SparseTexturePages::SparseTexturePages(
    const std::shared_ptr<MSG::Texture>& a_Src,
    const glm::uvec3& a_PageSize,
    const size_t& a_MaxMips)
    : pageSize(a_PageSize)
    , pageResolution(glm::max(a_Src->GetSize() / pageSize, 1u))
{
    auto pageCount = pageResolution.x * pageResolution.y * pageResolution.z;
    pendingPages.reserve(pageCount * a_MaxMips);
    residentPages.reserve(pageCount * a_MaxMips);
}

std::vector<glm::vec4> MSG::Renderer::SparseTexturePages::GetMissingPages(
    const uint32_t& a_MinMip,
    const uint32_t& a_MaxMip,
    const glm::vec3& a_UVStart,
    const glm::vec3& a_UVEnd) const
{
    std::vector<glm::vec4> res;
    res.reserve((a_MaxMip - a_MinMip) * pageResolution.x * pageResolution.y * pageResolution.z);
    for (uint32_t level = a_MinMip; level < a_MaxMip; level++) {
        auto levelPageRes = GetLevelPageRes(level);
        for (uint32_t z = 0; z < levelPageRes.z; z++) {
            for (uint32_t y = 0; y < levelPageRes.y; y++) {
                for (uint32_t x = 0; x < levelPageRes.x; x++) {
                    glm::vec4 pageAddress(x, y, z, level);
                    if (!residentPages.contains(pageAddress) && !pendingPages.contains(pageAddress))
                        res.emplace_back(pageAddress);
                }
            }
        }
    }
    return res;
}

MSG::Renderer::VirtualTexture::VirtualTexture(OGLContext& a_Ctx, const std::shared_ptr<MSG::Texture>& a_Src)
    : VirtualTexture(std::make_shared<OGLTexture2D>(a_Ctx, GetSparseTextureInfo(*a_Src)), a_Src)
{
}

MSG::Renderer::VirtualTexture::VirtualTexture(const std::shared_ptr<OGLTexture>& a_Txt, const std::shared_ptr<MSG::Texture>& a_Src)
    : sparseTexture(a_Txt)
    , src(a_Src)
    , maxMipLevel(GetMaxMips(a_Txt->context, *a_Txt))
    , pages(a_Src, GetPageSize(a_Txt->context, a_Txt->target, a_Txt->sizedFormat), maxMipLevel)
{
    // always commit the last mip
    auto lastLevel = maxMipLevel > 1 ? maxMipLevel - 1 : maxMipLevel;
    auto pageRes   = pages.GetLevelPageRes(lastLevel);
    for (auto z = 0u; z < pageRes.z; z++) {
        for (auto y = 0u; y < pageRes.y; y++) {
            for (auto x = 0u; x < pageRes.x; x++) {
                CommitPage({ x, y, z, lastLevel });
            }
        }
    }
}

std::vector<glm::vec4> MSG::Renderer::VirtualTexture::GetMissingPages(
    const uint32_t& a_MinLevel, const uint32_t& a_MaxLevel,
    const glm::vec3& a_UVStart, const glm::vec3& a_UVEnd) const
{
    auto minLvl = std::min(uint32_t(maxMipLevel), a_MinLevel);
    auto maxLvl = std::min(uint32_t(maxMipLevel + 1), a_MaxLevel);
    return pages.GetMissingPages(minLvl, maxLvl, a_UVStart, a_UVEnd);
}

void MSG::Renderer::VirtualTexture::SetPending(const glm::vec4& a_PageAddress)
{
    glm::uvec4 pagesStart = a_PageAddress;
    glm::uvec4 pagesEnd   = pagesStart + 1u;
    pages.MakePending(pagesStart, pagesEnd);
}

void MSG::Renderer::VirtualTexture::CommitPage(const glm::vec4& a_PageAddress)
{
    auto level             = std::min(uint32_t(maxMipLevel), uint32_t(a_PageAddress.w));
    auto& srcImage         = src->at(level);
    glm::uvec4 pagesStart  = a_PageAddress;
    glm::uvec4 pagesEnd    = pagesStart + 1u;
    glm::uvec3 texelStart  = glm::uvec3(pagesStart) * pages.pageSize;
    glm::uvec3 texelEnd    = glm::uvec3(pagesEnd) * pages.pageSize;
    texelEnd               = glm::min(texelEnd, srcImage->GetSize()); // in case the texture is smaller than pageSize
    glm::uvec3 texelExtent = texelEnd - texelStart;
    OGLTextureCommitInfo info {
        .level   = level,
        .offsetX = texelStart.x,
        .offsetY = texelStart.y,
        .offsetZ = texelStart.z,
        .width   = texelExtent.x,
        .height  = texelExtent.y,
        .depth   = texelExtent.z,
        .commit  = true
    };
    sparseTexture->CommitPage(info);
    sparseTexture->UploadLevel(level, texelStart, texelExtent, *srcImage);
    pages.Commit(pagesStart, pagesEnd);
}
