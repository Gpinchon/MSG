#pragma once

#include <MSG/Cube.hpp>
#include <MSG/Texture.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <optional>
#include <vector>

namespace MSG {
class OGLTexture;
class OGLContext;
}

namespace MSG::Renderer {
class SparseTextureLevel {
public:
    SparseTextureLevel(const glm::uvec3& a_Resolution, const glm::uvec3& a_PageSize)
        : resolution(a_Resolution)
        , pageSize(a_PageSize)
    {
        auto pageRes   = glm::max(resolution / pageSize, 1u);
        auto pageCount = pageRes.x * pageRes.y * pageRes.z;
        pages.resize(pageCount, false);
    }

    auto GetTexCoordPageCoord(const glm::uvec3& a_TexCoord) const { return a_TexCoord / pageSize; }
    auto GetUVPageCoord(const glm::vec3& a_UV) const { return GetTexCoordPageCoord(a_UV * glm::vec3(resolution)); }

    auto GetPageIndex(const glm::uvec3& a_PageCoord) const
    {
        return (a_PageCoord.z * resolution.x * resolution.y) + (a_PageCoord.y * resolution.x) + a_PageCoord.x;
    }
    auto GetTexCoordPageIndex(const glm::uvec3& a_TexCoord) const { return GetPageIndex(GetTexCoordPageCoord(a_TexCoord)); }
    auto GetUVPageIndex(const glm::vec3& a_UV) const { return GetTexCoordPageIndex(a_UV * glm::vec3(resolution)); }
    void SetRange(const glm::vec3& a_Start, const glm::vec3& a_End, const bool& a_State)
    {
        glm::vec3 start      = glm::min(a_Start * glm::vec3(resolution), glm::vec3(resolution) - 1.f);
        glm::vec3 end        = glm::min(a_End * glm::vec3(resolution), glm::vec3(resolution) - 1.f);
        glm::vec3 width      = end - start;
        glm::uvec3 pageStart = start / glm::vec3(pageSize);
        glm::uvec3 pageWidth = width / glm::vec3(pageSize);
        glm::uvec3 pageEnd   = end / glm::vec3(pageSize);
        for (auto z = pageStart.z; z < pageEnd.z; z++) {
            for (auto y = pageStart.y; y < pageEnd.y; y++) {
                auto lineBeg = pages.begin() + GetPageIndex(glm::uvec3 { pageStart.x, y, z });
                std::fill_n(lineBeg, pageWidth.x, a_State);
            }
        }
    }
    void Set(const glm::vec3& a_UV, const bool& a_State) { pages[GetUVPageIndex(a_UV)] = a_State; }
    bool Get(const glm::vec3& a_UV) const { return pages[GetUVPageIndex(a_UV)]; }

    const glm::uvec3 resolution;
    const glm::uvec3 pageSize;
    std::vector<bool> pages;
};

class SparseTexturePages {
public:
    SparseTexturePages(const std::shared_ptr<MSG::Texture>& a_Src, const glm::uvec3& a_PageSize);
    // find the right mip level and void recursive for performance
    auto& GetMipLevel(uint32_t a_Mip) const { return levels[a_Mip]; }
    auto& GetMipLevel(uint32_t a_Mip) { return levels[a_Mip]; }
    void SetRange(const uint32_t& a_Mip, const glm::vec3& a_Offset, const glm::vec3& a_Size, const bool& a_State) { GetMipLevel(a_Mip).SetRange(a_Offset, a_Size, a_State); }
    void Set(const uint32_t& a_Mip, const glm::vec3& a_UV, const bool& a_State) { GetMipLevel(a_Mip).Set(a_UV, a_State); }
    void CommitRange(const uint32_t& a_Mip, const glm::vec3& a_Offset, const glm::vec3& a_Size) { return SetRange(a_Mip, a_Offset, a_Size, true); }
    void Commit(const uint32_t& a_Mip, const glm::vec3& a_UV) { Set(a_Mip, a_UV, true); }
    void FreeRange(const uint32_t& a_Mip, const glm::vec3& a_Offset, const glm::vec3& a_Size) { return SetRange(a_Mip, a_Offset, a_Size, false); }
    void Free(const uint32_t& a_Mip, const glm::vec3& a_UV) { Set(a_Mip, a_UV, false); }
    bool IsResident(const uint32_t& a_Mip, const glm::vec3& a_UV) const { GetMipLevel(a_Mip).Get(a_UV); }
    std::vector<SparseTextureLevel> levels;
};

class VirtualTexture {
public:
    VirtualTexture(OGLContext& a_Ctx, const std::shared_ptr<MSG::Texture>& a_Src);
    VirtualTexture(const std::shared_ptr<OGLTexture>& a_Txt, const std::shared_ptr<MSG::Texture>& a_Src);
    void Commit(const uint32_t& a_Level, const glm::vec3& a_UVStart, const glm::vec3& a_UVEnd);
    void Uncommit(const uint32_t& a_Level, const glm::vec3& a_UVOffset, const glm::vec3& a_UVSize);
    std::shared_ptr<OGLTexture> sparseTexture;
    std::shared_ptr<MSG::Texture> src;
    SparseTexturePages pages;
};
}