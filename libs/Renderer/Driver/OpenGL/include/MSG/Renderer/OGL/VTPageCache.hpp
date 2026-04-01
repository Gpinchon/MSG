#pragma once

#include <chrono>
#include <list>
#include <mutex>
#include <unordered_map>
#include <vector>

#include <glm/vec4.hpp>

// Forward declarations
namespace Msg::Renderer {
class VirtualTexture;
struct VTPageCacheKey;
}

// Hash function
namespace std {
template <typename T>
struct hash;
template <>
struct hash<Msg::Renderer::VTPageCacheKey> {
    size_t operator()(Msg::Renderer::VTPageCacheKey const& a_Value) const;
};
}

// Class declarations
namespace Msg::Renderer {
constexpr size_t VTPageCacheMaxSize = 4294967296; // 4 Gb
struct VTPageCacheData {
    std::vector<std::byte> rawData;
    std::chrono::system_clock::time_point lastAccess;
};
struct VTPageCacheKey {
    bool operator==(const VTPageCacheKey& a_Other) const
    {
        return a_Other.texture == texture
            && a_Other.pageIndex == pageIndex;
    }
    VirtualTexture const* texture;
    uint32_t pageIndex;
};

class VTPageCache {
public:
    void RemoveCache(const VirtualTexture* a_Texture, const uint32_t& a_PageIndex);
    const std::vector<std::byte>* AddCache(const VirtualTexture* a_Texture, const uint32_t& a_PageIndex, const std::vector<std::byte>& a_Data);
    const std::vector<std::byte>* GetCache(const VirtualTexture* a_Texture, const uint32_t& a_PageIndex);

private:
    size_t _size = 0;
    std::mutex _mutex;
    std::list<VTPageCacheKey> _availableCache;
    std::unordered_map<VTPageCacheKey, VTPageCacheData> _cacheData;
};
}