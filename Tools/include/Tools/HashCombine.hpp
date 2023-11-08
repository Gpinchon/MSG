#pragma once

#include <functional>

// similar to vulkan_hash.hpp because it works very well
#if !defined(TABGRAPH_HASH_COMBINE)
#define TABGRAPH_HASH_COMBINE(seed, value) \
    seed ^= std::hash<std::decay<decltype(value)>::type> {}(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2)
#endif
