#pragma once

#include <queue>
#include <unordered_map>
#include <vector>

#include <glm/vec4.hpp>

// Forward declarations
namespace Msg::Renderer {
class SparseTexture;
struct SparseTexturePageCacheKey;
}

// Hash function
namespace std {
template <typename T>
struct hash;
template <>
struct hash<Msg::Renderer::SparseTexturePageCacheKey> {
    size_t operator()(Msg::Renderer::SparseTexturePageCacheKey const& a_Value) const;
};
}

// Class declarations
namespace Msg::Renderer {
constexpr size_t SparseTexturePageCacheMaxSize = 1073741824; // 1 Gb
using SparseTexturePageCacheData               = std::vector<std::byte>;
struct SparseTexturePageCacheKey {
    bool operator==(const SparseTexturePageCacheKey& a_Other) const
    {
        return a_Other.texture == texture
            && a_Other.address == address;
    }
    const SparseTexture* texture;
    const glm::uvec4 address;
};

class SparseTexturePageCache {
public:
    const SparseTexturePageCacheData* AddCache(const SparseTexture* a_Texture, const glm::uvec4& a_Address, const SparseTexturePageCacheData& a_Data);
    const SparseTexturePageCacheData* GetCache(const SparseTexture* a_Texture, const glm::uvec4& a_Address) const;

private:
    size_t _size = 0;
    std::queue<SparseTexturePageCacheKey> _availableCache;
    std::unordered_map<SparseTexturePageCacheKey, SparseTexturePageCacheData> _cacheData;
};
}