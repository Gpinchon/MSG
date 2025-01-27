#pragma once

#include <memory>

#define MSG_HANDLE(name)                  \
    namespace name {                      \
    class Impl;                           \
    using Handle = std::shared_ptr<Impl>; \
    }
