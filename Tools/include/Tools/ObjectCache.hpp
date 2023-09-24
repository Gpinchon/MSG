#pragma once

#include <Tools/TupleHasher.hpp>

#include <unordered_map>

namespace TabGraph::Tools {
template <typename...>
struct ObjectCacheKey { };

// to make compiler happy
template <typename, typename>
struct ObjectCache;

template <typename... Keys, typename Type>
struct ObjectCache<ObjectCacheKey<Keys...>, Type> : std::unordered_map<std::tuple<Keys...>, Type, TupleHasher<Keys...>> {
    template <typename Factory>
    constexpr inline auto& GetOrCreate(
        const Keys&... a_Keys,
        const Factory& a_Factory)
    {
        return try_emplace({ a_Keys... }, a_Factory).first->second;
    }
};
}