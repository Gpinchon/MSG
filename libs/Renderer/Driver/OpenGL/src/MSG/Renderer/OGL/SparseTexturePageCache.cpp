#include <MSG/Renderer/OGL/SparseTexturePageCache.hpp>
#include <MSG/Tools/HashCombine.hpp>

namespace std {
size_t hash<Msg::Renderer::SparseTexturePageCacheKey>::operator()(Msg::Renderer::SparseTexturePageCacheKey const& a_Value) const
{
    size_t seed = 0;
    MSG_HASH_COMBINE(seed, a_Value.texture);
    MSG_HASH_COMBINE(seed, a_Value.pageIndex);
    return seed;
}
}

void Msg::Renderer::SparseTexturePageCache::Cleanup()
{
    auto now = std::chrono::system_clock::now();
    std::vector<SparseTexturePageCacheKey> cacheKeys(_availableCache.begin(), _availableCache.end());
    for (auto& cacheKey : cacheKeys) {
        auto& cacheData = _cacheData[cacheKey];
        if (now - cacheData.lastAccess >= PageCacheLifeExpetency)
            RemoveCache(cacheKey.texture, cacheKey.pageIndex);
    }
}

void Msg::Renderer::SparseTexturePageCache::RemoveCache(const SparseTexture* a_Texture, const uint32_t& a_PageIndex)
{
    auto itr = _cacheData.find(SparseTexturePageCacheKey { a_Texture, a_PageIndex });
    if (itr == _cacheData.end())
        return; // no corresponding cache, ignore
    _size -= itr->second.rawData.size();
    _availableCache.erase(std::remove(_availableCache.begin(), _availableCache.end(), itr->first));
    _cacheData.erase(itr);
}

const std::vector<std::byte>* Msg::Renderer::SparseTexturePageCache::AddCache(const SparseTexture* a_Texture, const uint32_t& a_PageIndex, const std::vector<std::byte>& a_Data)
{
    SparseTexturePageCacheKey cacheKey { a_Texture, a_PageIndex };
    assert(GetCache(a_Texture, a_PageIndex) == nullptr);
    while (_size + a_Data.size() > SparseTexturePageCacheMaxSize) {
        auto lastCacheData = _cacheData.find(_availableCache.front());
        _size -= lastCacheData->second.rawData.size();
        _availableCache.pop_front();
        _cacheData.erase(lastCacheData);
    }
    _size += a_Data.size();
    _availableCache.push_back(cacheKey);
    auto& cacheData      = _cacheData[cacheKey];
    cacheData.rawData    = a_Data;
    cacheData.lastAccess = std::chrono::system_clock::now();
    return &cacheData.rawData;
}

const std::vector<std::byte>* Msg::Renderer::SparseTexturePageCache::GetCache(const SparseTexture* a_Texture, const uint32_t& a_PageIndex)
{
    auto itr = _cacheData.find(SparseTexturePageCacheKey { a_Texture, a_PageIndex });
    if (itr == _cacheData.end())
        return nullptr;
    else {
        itr->second.lastAccess = std::chrono::system_clock::now();
        return &itr->second.rawData;
    }
}