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
class WorkerThread;
}

namespace Msg::Renderer {
class VTPageCache;
class VTPool;
}

namespace Msg::Renderer {
enum VTPageState : uint8_t {
    Uncommited,
    Requested,
    Baking,
    Baked,
    Commited
};
struct VTLocalPage {
    VTPageState state                   = VTPageState::Uncommited;
    std::array<uint32_t, 4> bottomPages = { -1u, -1u, -1u, -1u }; // corresponding pages on the lower (bigger) level
    uint32_t topPage                    = -1u; // corresponding page on the higher (smaller) level
    glm::uvec2 atlasPage                = glm::uvec2(-1u);
    glm::uvec3 pageCoords               = glm::vec3(0);
    uint8_t level                       = 0;
    std::chrono::system_clock::time_point accessTime;
};

struct VTBakedPage {
    uint32_t pageID;
    std::vector<std::byte> const* rawData; // raw data comes from the VTPageCache
};

class VirtualTexture : public std::enable_shared_from_this<VirtualTexture> {
public:
    static constexpr std::chrono::seconds PageExpiration          = std::chrono::seconds(30);
    static constexpr std::chrono::seconds EmergencyPageExpiration = std::chrono::seconds(1); // page life expetency when pool is full
    static constexpr std::chrono::seconds BakingJobsExpiration    = std::chrono::seconds(1); // how long the page baking jobs are allowed to run before being killed
    VirtualTexture(
        OGLContext& a_Ctx,
        const std::shared_ptr<Msg::Texture>& a_Src, const SamplerWrap& a_WrapS, const SamplerWrap& a_WrapT,
        VTPageCache& a_PageCache, VTPool& a_Pool);
    ~VirtualTexture();
    /** @brief returns any allocated page to the pages pool */
    void Clear();
    /** @brief requests memory for the last mips level */
    void Allocate();
    /** @return true if this page is missing */
    bool RequestPage(const uint32_t& a_PageID);
    /** @brief prepares requested pages for upload if */
    void BakeRequestedPages(WorkerThread& a_WorkerThread);
    /** @brief consumes and uploads the pages that are ready to be uploaded to the atlas */
    void UploadBakedPages();
    /** @brief frees the pages that were not accessed for longer than PageExpiration */
    void FreeUnusedPages();
    bool Empty() const;
    std::shared_ptr<OGLTexture> GetPageTable() const;
    uint32_t GetPageID(const glm::vec3& a_UV, const uint8_t& a_Level) const;
    glm::uvec3 GetVirtualSize(const uint8_t& a_Lvl = 0) const;
    /** @return the number of levels of this texture virtual texture */
    uint32_t GetLevels() const;
    uint32_t GetCommitedCount() const { return _commitedPages.size(); }
    VTPageCache& pageCache;

private:
    void _RequestMemory(const uint32_t& a_PageID);
    void _CommitPage(const uint32_t& a_PageID);
    void _UploadPage(const uint32_t& a_PageID, const std::vector<std::byte>& a_RawData);
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
    std::vector<VTLocalPage> _localPages;
    std::vector<uint32_t> _requestedPages;
    uint32_t _bakingPages = 0; // the number of pages currently baking
    std::vector<VTBakedPage> _bakedPages; // the pages that are ready to be uploaded
    std::unordered_set<uint32_t> _commitedPages;
    std::shared_ptr<OGLTexture> _pageTableTexture;
    Sampler _sampler;
};
}