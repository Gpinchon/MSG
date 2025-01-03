#pragma once

#include <memory>

#define TABGRAPH_HANDLE(name)         \
namespace name {                      \
class Impl;                           \
using Handle = std::shared_ptr<Impl>; \
}
