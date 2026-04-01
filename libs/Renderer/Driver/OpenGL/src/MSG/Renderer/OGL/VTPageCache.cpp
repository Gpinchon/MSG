#include <MSG/Renderer/OGL/VTPageCache.hpp>
#include <MSG/Tools/HashCombine.hpp>

namespace std {
size_t hash<Msg::Renderer::VTPageCacheKey>::operator()(Msg::Renderer::VTPageCacheKey const& a_Value) const
{
    size_t seed = 0;
    MSG_HASH_COMBINE(seed, a_Value.texture);
    MSG_HASH_COMBINE(seed, a_Value.pageIndex);
    return seed;
}
}

void Msg::Renderer::VTPageCache::RemoveCache(const VirtualTexture* a_Texture, const uint32_t& a_PageIndex)
{
    std::lock_guard lock(_mutex);
    auto itr = _cacheData.find(VTPageCacheKey { a_Texture, a_PageIndex });
    if (itr == _cacheData.end())
        return; // no corresponding cache, ignore
    _size -= itr->second.rawData.size();
    _availableCache.erase(std::remove(_availableCache.begin(), _availableCache.end(), itr->first));
    _cacheData.erase(itr);
}

const std::vector<std::byte>* Msg::Renderer::VTPageCache::AddCache(const VirtualTexture* a_Texture, const uint32_t& a_PageIndex, const std::vector<std::byte>& a_Data)
{
    std::lock_guard lock(_mutex);
    VTPageCacheKey cacheKey { a_Texture, a_PageIndex };
    assert(!_cacheData.contains(cacheKey));
    while (_size + a_Data.size() > VTPageCacheMaxSize) {
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

const std::vector<std::byte>* Msg::Renderer::VTPageCache::GetCache(const VirtualTexture* a_Texture, const uint32_t& a_PageIndex)
{
    std::lock_guard lock(_mutex);
    auto itr = _cacheData.find(VTPageCacheKey { a_Texture, a_PageIndex });
    if (itr == _cacheData.end())
        return nullptr;
    else {
        itr->second.lastAccess = std::chrono::system_clock::now();
        return &itr->second.rawData;
    }
}