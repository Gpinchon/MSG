#pragma once

#include <memory>

#define TABGRAPH_RENDERER_HANDLE(name)    \
    namespace name {                      \
    struct Impl;                          \
    using Handle = std::shared_ptr<Impl>; \
    }
