#pragma once

#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace MSG {
class OGLCmdBuffer;
class OGLFrameBuffer;
}

namespace MSG::Renderer {
class PassInterface;
}

namespace MSG::Renderer {
class PassLibrary {
public:
    template <typename T, typename... Args>
    void Add(Args&&... a_Args);
    template <typename T>
    void Remove();
    std::vector<std::unique_ptr<PassInterface>> passes;
};

template <typename T, typename... Args>
inline void PassLibrary::Add(Args&&... a_Args)
{
    passes.emplace_back(std::make_unique<T>(std::forward(a_Args)...));
}

template <typename T>
inline void PassLibrary::Remove()
{
    std::erase_if(
        passes,
        [](auto& a_Subsystem) { typeid(*a_Subsystem) == typeid(T); });
}
}