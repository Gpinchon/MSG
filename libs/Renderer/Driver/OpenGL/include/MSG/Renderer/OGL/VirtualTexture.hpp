#pragma once

#include <MSG/OGLTexture.hpp>
#include <MSG/Sampler.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <array>
#include <chrono>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Msg {
class Texture;
class OGLContext;
class OGLBindlessTextureSampler;
}

namespace Msg::Renderer {
class VTPageCache;
class VTPool;
}

namespace Msg::Renderer {
struct VTLocalPage {
    std::array<uint32_t, 4> bottomPages = { -1u, -1u, -1u, -1u }; // corresponding pages on the lower (bigger) level
    uint32_t topPage                    = -1u; // corresponding page on the higher (smaller) level
    bool commited                       = false;
    glm::uvec2 atlasPage                = glm::uvec2(-1u);
    glm::uvec3 pageCoords               = glm::vec3(0);
    uint8_t level                       = 0;
    std::chrono::system_clock::time_point accessTime;
};

class VirtualTexture : public std::enable_shared_from_this<VirtualTexture> {
public:
    static constexpr std::chrono::seconds PageLifeExpetency = std::chrono::seconds(30);
    VirtualTexture(
        OGLContext& a_Ctx,
        const std::shared_ptr<Msg::Texture>& a_Src, const SamplerWrap& a_WrapS, const SamplerWrap& a_WrapT,
        VTPageCache& a_PageCache, VTPool& a_Pool);
    ~VirtualTexture();
    /**  @return true if any page is missing */
    bool RequestPage(const uint32_t& a_PageID);
    /** @return the time this operation took to complete */
    void CommitPendingPages();
    void FreeUnusedPages();
    bool Empty() const { return _commitedPages.empty(); }
    std::shared_ptr<OGLTexture> GetPageTable() const;
    uint32_t GetPageID(const glm::vec3& a_UV, const uint8_t& a_Level) const;
    glm::uvec3 GetVirtualSize(const uint8_t& a_Lvl = 0) const;
    /** @return the number of levels of this texture virtual texture */
    uint32_t GetLevels() const;
    VTPageCache& pageCache;

private:
    void _CommitPage(const uint32_t& a_PageID);
    void _UploadPage(const uint32_t& a_PageID);
    void _FreePage(const uint32_t& a_PageID);
    // @return the pages resolution for the specified level
    glm::uvec3 _GetPageRes(const uint32_t& a_Lvl = 0) const;
    glm::uvec3 _GetSrcSize(const uint32_t& a_Lvl = 0) const;
    glm::uvec3 _GetPageTableSize(const uint8_t& a_Lvl = 0) const;
    std::mutex _mutex;
    VTPool& _pool;
    bool _needsResize = false;
    std::shared_ptr<Msg::Texture> _src;
    glm::uvec3 _pageRes; // the number of pages inside the sparse texture
    glm::vec3 _virtualPageSize; // the page size of the virtual texture, used to resize pages if necessary
    std::unordered_set<uint32_t> _requestedPages;
    std::unordered_set<uint32_t> _commitedPages;
    std::vector<VTLocalPage> _localPages;
    std::shared_ptr<OGLTexture> _pageTableTexture;
    Sampler _sampler;
};
}