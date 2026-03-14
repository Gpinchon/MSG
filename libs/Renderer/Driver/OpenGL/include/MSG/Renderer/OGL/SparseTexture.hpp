#pragma once

#include <MSG/OGLTexture.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <chrono>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Msg {
class Texture;
class OGLContext;
}

namespace Msg::Renderer {
class SparseTexturePageCache;
}

namespace Msg::Renderer {
struct SparseTextureLocalPage {
    bool commited         = false;
    glm::uvec3 pageCoords = glm::vec3(0);
    uint32_t level        = 0;
    std::chrono::system_clock::time_point accessTime;
};

class SparseTexture : public OGLTexture, public std::enable_shared_from_this<SparseTexture> {
public:
    static constexpr std::chrono::seconds PageLifeExpetency = std::chrono::seconds(30);
    SparseTexture(OGLContext& a_Ctx, const std::shared_ptr<Msg::Texture>& a_Src, SparseTexturePageCache& a_PageCache);
    /**  @return true if any page is missing */
    bool RequestPage(const uint32_t& a_PageIndex);
    /** @return the time this operation took to complete */
    std::chrono::milliseconds CommitPendingPages(const std::chrono::milliseconds& a_RemainingTime);
    void FreeUnusedPages();
    void UploadPage(const uint32_t& a_PageIndex);
    void CommitPage(const uint32_t& a_PageIndex);
    void FreePage(const uint32_t& a_PageIndex);
    bool Empty() const { return _commitedPages.empty(); }
    uint32_t GetPageIndex(const glm::vec3& a_UV, const uint32_t& a_Level) const;
    glm::uvec3 GetVirtualSize(const uint32_t& a_Lvl = 0) const;
    glm::uvec3 GetSparseSize(const uint32_t& a_Lvl = 0) const;
    /** @return the number of levels of this texture wether they're sparse or not */
    uint32_t GetLevels() const;
    /** @return the number of levels that are actually sparse, the rest of them are packed and always available */
    uint32_t GetVirtualLevels() const;
    SparseTexturePageCache& pageCache;

private:
    OGLTextureCommitInfo _GetCommitInfo(const uint32_t& a_PageIndex, const bool& a_Commit) const;
    // @return the pages resolution for the specified level
    glm::uvec3 _GetPageRes(const uint32_t& a_Lvl = 0) const;
    bool _needsResize = false;
    std::shared_ptr<Msg::Texture> _src;
    uint32_t _sparseLevelsCount = 0;
    glm::uvec3 _pageRes; // the number of pages inside the sparse texture
    glm::uvec3 _sparsePageSize;
    glm::vec3 _virtualPageSize; // the page size of the virtual texture, used to resize pages if necessary
    std::unordered_set<uint32_t> _requestedPages;
    std::unordered_set<uint32_t> _commitedPages;
    std::vector<SparseTextureLocalPage> _localPages;
};
}