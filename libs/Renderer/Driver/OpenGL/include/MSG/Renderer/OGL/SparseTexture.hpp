#pragma once

#include <MSG/OGLTexture.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <chrono>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace std {
template <typename T>
struct hash;
template <>
struct hash<glm::uvec4> {
    size_t operator()(glm::uvec4 const& a_Val) const;
};
}

namespace MSG {
class Texture;
class OGLContext;
}

namespace MSG::Renderer {
class SparseTexturePages {
public:
    using AccessTime = std::chrono::system_clock::time_point;
    SparseTexturePages(
        const std::shared_ptr<MSG::Texture>& a_Src,
        const glm::uvec3& a_PageSize,
        const size_t& a_MaxMips);
    glm::uvec3 GetLevelPageRes(const uint32_t& a_Level) const
    {
        return glm::max(pageResolution / uint32_t(pow(a_Level + 1, 2)), 1u);
    }
    void Commit(const glm::uvec4& a_PageAddress)
    {
        pendingPages.erase(a_PageAddress);
        residentPages.insert(a_PageAddress);
        lastAccess[a_PageAddress] = std::chrono::system_clock::now();
    }
    void Free(const glm::uvec4& a_PageAddress)
    {
        residentPages.erase(a_PageAddress);
        pendingPages.erase(a_PageAddress);
        lastAccess.erase(a_PageAddress);
    }
    bool Request(
        const uint32_t& a_MinMip, const uint32_t& a_MaxMip,
        const glm::vec3& a_UVStart, const glm::vec3& a_UVEnd);
    const glm::uvec3 pageSize;
    const glm::uvec3 pageResolution;
    std::unordered_set<glm::uvec4> pendingPages;
    std::unordered_set<glm::uvec4> residentPages;
    std::unordered_map<glm::uvec4, AccessTime> lastAccess;
};

class SparseTexture : public OGLTexture {
public:
    static constexpr std::chrono::seconds PageLifeExpetency = std::chrono::seconds(5);
    SparseTexture(OGLContext& a_Ctx, const std::shared_ptr<MSG::Texture>& a_Src, const bool& a_Sparse);
    /** @return true if any page is missing */
    bool RequestPages(
        const uint32_t& a_MinLevel, const uint32_t& a_MaxLevel,
        const glm::vec3& a_UVStart, const glm::vec3& a_UVEnd);
    /** @return the number of Mo uploaded */
    std::chrono::milliseconds CommitPendingPages(const std::chrono::milliseconds& a_RemainingTime);
    void FreeUnusedPages();
    void CommitPage(const glm::uvec4& a_PageAddress);
    void FreePage(const glm::uvec4& a_PageAddress);
    bool Empty() const { return pages.lastAccess.empty(); }
    std::shared_ptr<MSG::Texture> src;
    const uint32_t sparseLevelsCount;
    SparseTexturePages pages;
};
}