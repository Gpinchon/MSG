#pragma once

#include <MSG/Cube.hpp>
#include <MSG/Texture.hpp>

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
class OGLTexture;
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
    void MakePending(const glm::uvec4& a_PageAddress)
    {
        pendingPages.insert(a_PageAddress);
    }
    void MakePending(
        const glm::uvec4& a_PageStart,
        const glm::uvec4& a_PageEnd)
    {
        for (auto level = a_PageStart.w; level < a_PageEnd.w; level++) {
            for (auto z = a_PageStart.z; z < a_PageEnd.z; z++) {
                for (auto y = a_PageStart.y; y < a_PageEnd.y; y++) {
                    for (auto x = a_PageStart.x; x < a_PageEnd.x; x++) {
                        glm::uvec4 pageAddress(x, y, z, level);
                        MakePending(pageAddress);
                    }
                }
            }
        }
    }
    void Commit(const glm::uvec4& a_PageStart)
    {
        pendingPages.erase(a_PageStart);
        residentPages.insert(a_PageStart);
    }
    void Commit(
        const glm::uvec4& a_PageStart,
        const glm::uvec4& a_PageEnd)
    {
        for (auto level = a_PageStart.w; level < a_PageEnd.w; level++) {
            for (auto z = a_PageStart.z; z < a_PageEnd.z; z++) {
                for (auto y = a_PageStart.y; y < a_PageEnd.y; y++) {
                    for (auto x = a_PageStart.x; x < a_PageEnd.x; x++) {
                        glm::uvec4 pageAddress(x, y, z, level);
                        Commit(pageAddress);
                    }
                }
            }
        }
    }
    void Free(const glm::uvec4& a_PageAddress)
    {
        residentPages.erase(a_PageAddress);
        pendingPages.erase(a_PageAddress);
        lastAccess.erase(a_PageAddress);
    }
    void Free(
        const glm::uvec4& a_PageStart,
        const glm::uvec4& a_PageEnd)
    {
        for (auto level = a_PageStart.w; level < a_PageEnd.w; level++) {
            for (auto z = a_PageStart.z; z < a_PageEnd.z; z++) {
                for (auto y = a_PageStart.y; y < a_PageEnd.y; y++) {
                    for (auto x = a_PageStart.x; x < a_PageEnd.x; x++) {
                        glm::uvec4 pageAddress(x, y, z, level);
                        Free(pageAddress);
                    }
                }
            }
        }
    }
    std::vector<glm::uvec4> GetMissingPages(
        const uint32_t& a_MinMip, const uint32_t& a_MaxMip,
        const glm::vec3& a_UVStart, const glm::vec3& a_UVEnd);
    const glm::uvec3 pageSize;
    const glm::uvec3 pageResolution;
    std::unordered_set<glm::uvec4> pendingPages;
    std::unordered_set<glm::uvec4> residentPages;
    std::unordered_map<glm::uvec4, AccessTime> lastAccess;
};

class VirtualTexture {
public:
    static constexpr std::chrono::seconds PageLifeExpetency = std::chrono::seconds(5);
    VirtualTexture(OGLContext& a_Ctx, const std::shared_ptr<MSG::Texture>& a_Src);
    VirtualTexture(const std::shared_ptr<OGLTexture>& a_Txt, const std::shared_ptr<MSG::Texture>& a_Src);
    std::vector<glm::uvec4> GetMissingPages(
        const uint32_t& a_MinLevel, const uint32_t& a_MaxLevel,
        const glm::vec3& a_UVStart, const glm::vec3& a_UVEnd);
    std::vector<glm::uvec4> GetUnusedPages() const;
    void SetPending(const glm::uvec4& a_PageAddress);
    void CommitPage(const glm::uvec4& a_PageAddress);
    void FreePage(const glm::uvec4& a_PageAddress);
    bool Empty() const { return pages.lastAccess.empty(); }
    std::shared_ptr<OGLTexture> sparseTexture;
    std::shared_ptr<MSG::Texture> src;
    const uint32_t sparseLevelsCount;
    SparseTexturePages pages;
};
}