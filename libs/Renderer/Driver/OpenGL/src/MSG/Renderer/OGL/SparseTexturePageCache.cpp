#include <MSG/Renderer/OGL/SparseTexturePageCache.hpp>
#include <MSG/Tools/HashCombine.hpp>

namespace std {
size_t hash<Msg::Renderer::SparseTexturePageCacheKey>::operator()(Msg::Renderer::SparseTexturePageCacheKey const& a_Value) const
{
    size_t seed = 0;
    MSG_HASH_COMBINE(seed, a_Value.texture);
    MSG_HASH_COMBINE(seed, a_Value.address.x);
    MSG_HASH_COMBINE(seed, a_Value.address.y);
    MSG_HASH_COMBINE(seed, a_Value.address.z);
    MSG_HASH_COMBINE(seed, a_Value.address.w);
    return seed;
}
}

const Msg::Renderer::SparseTexturePageCacheData* Msg::Renderer::SparseTexturePageCache::AddCache(const SparseTexture* a_Texture, const glm::uvec4& a_Address, const SparseTexturePageCacheData& a_Data)
{
    SparseTexturePageCacheKey cacheKey { a_Texture, a_Address };
    assert(GetCache(a_Texture, a_Address) == nullptr);
    while (_size + a_Data.size() > SparseTexturePageCacheMaxSize) {
        auto lastCacheData = _cacheData.find(_availableCache.front());
        _size -= lastCacheData->second.size();
        _availableCache.pop();
        _cacheData.erase(lastCacheData);
    }
    _size += a_Data.size();
    _availableCache.push(cacheKey);
    return &(_cacheData[cacheKey] = a_Data);
}

const Msg::Renderer::SparseTexturePageCacheData* Msg::Renderer::SparseTexturePageCache::GetCache(const SparseTexture* a_Texture, const glm::uvec4& a_Address) const
{
    auto itr = _cacheData.find(SparseTexturePageCacheKey { a_Texture, a_Address });
    if (itr == _cacheData.end())
        return nullptr;
    else
        return &itr->second;
}