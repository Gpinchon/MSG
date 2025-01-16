#pragma once

#include <Tools/TupleHasher.hpp>

#include <unordered_map>

namespace TabGraph::Tools {
template <typename...>
struct ObjectCacheKey { };

// to make compiler happy
template <typename, typename, template <typename> typename = TupleHasher>
struct ObjectCache;

template <typename... Keys, typename Type, template <typename> typename Hasher>
struct ObjectCache<ObjectCacheKey<Keys...>, Type, Hasher> : std::unordered_map<std::tuple<Keys...>, Type, Hasher<Keys...>> {
    template <typename Factory>
    constexpr inline auto& GetOrCreate(
        const Keys&... a_Keys,
        const Factory& a_Factory)
    {
        return this->try_emplace({ a_Keys... }, a_Factory).first->second;
    }
};
}
