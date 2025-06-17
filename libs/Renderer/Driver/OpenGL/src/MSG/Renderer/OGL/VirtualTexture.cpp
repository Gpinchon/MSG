#include <MSG/Renderer/OGL/VirtualTexture.hpp>

#include <MSG/OGLContext.hpp>
#include <MSG/OGLTexture2D.hpp>

#include <GL/glew.h>

#include <map>

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

auto GetSparseTextureInfo(const MSG::Texture& a_Src)
{
    return MSG::OGLTexture2DInfo {
        .width  = a_Src.GetSize().x,
        .height = a_Src.GetSize().y,
        .levels = uint32_t(a_Src.size()),
    };
}

MSG::Renderer::SparseTexturePages::SparseTexturePages(const std::shared_ptr<MSG::Texture>& a_Src, const glm::uvec3& a_PageSize)
{
    levels.reserve(a_Src->size());
    for (auto& level : *a_Src)
        levels.emplace_back(level->GetSize(), a_PageSize);
}

MSG::Renderer::VirtualTexture::VirtualTexture(OGLContext& a_Ctx, const std::shared_ptr<MSG::Texture>& a_Src)
    : VirtualTexture(std::make_shared<OGLTexture2D>(a_Ctx, GetSparseTextureInfo(*a_Src)), a_Src)
{
}

MSG::Renderer::VirtualTexture::VirtualTexture(const std::shared_ptr<OGLTexture>& a_Txt, const std::shared_ptr<MSG::Texture>& a_Src)
    : sparseTexture(a_Txt)
    , src(a_Src)
    , pages(a_Src, GetPageSize(a_Txt->context, a_Txt->target, a_Txt->sizedFormat))
{
    // always commit the last mip
    auto lastMip = src->size() - 1;
    Commit(lastMip, glm::vec3(0), glm::vec3(1));
}

void MSG::Renderer::VirtualTexture::Commit(
    const uint32_t& a_Level, const glm::vec3& a_UVStart, const glm::vec3& a_UVEnd)
{
    auto level            = std::min(uint32_t(src->size() - 1), a_Level);
    auto& pagesLevel      = pages.levels.at(level);
    auto pagesRes         = glm::max(pagesLevel.resolution / pagesLevel.pageSize, 1u);
    glm::vec3 srcSize     = pagesLevel.resolution;
    glm::vec3 srcStart    = glm::min(a_UVStart * srcSize, srcSize - 1.f);
    glm::vec3 srcEnd      = glm::min(a_UVEnd * srcSize, srcSize - 1.f);
    glm::uvec3 pageStart  = glm::uvec3(srcStart / glm::vec3(pagesLevel.pageSize)) * pagesLevel.pageSize;
    glm::uvec3 pageEnd    = glm::max(glm::uvec3(srcEnd / glm::vec3(pagesLevel.pageSize)), 1u) * pagesLevel.pageSize;
    pageEnd               = glm::min(pageEnd, pagesLevel.resolution); // in case the texture is smaller than pageSize
    glm::uvec3 pageExtent = pageEnd - pageStart;
    // SOMETIMES THE EXTENT GETS BELLOW ZERO, CLAMP DURING FEEDBACK PASS
    OGLTextureCommitInfo info {
        .level   = level,
        .offsetX = pageStart.x,
        .offsetY = pageStart.y,
        .offsetZ = pageStart.z,
        .width   = pageExtent.x,
        .height  = pageExtent.y,
        .depth   = pageExtent.z,
        .commit  = true
    };
    sparseTexture->CommitPage(info);
    sparseTexture->UploadLevel(level, pageStart, pageExtent, *src->at(level));
    pages.CommitRange(level, a_UVStart, a_UVEnd);
}