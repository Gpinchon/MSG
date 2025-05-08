#pragma once

#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace MSG::Renderer {
class SubsystemInterface;
}

namespace MSG::Renderer {
using Subsystems = std::vector<std::unique_ptr<MSG::Renderer::SubsystemInterface>>;
class SubsystemLibrary {
public:
    template <typename T, typename... Args>
    void Add(Args&&... a_Args);
    template <typename T>
    void Remove();
    template <typename T>
    const T& Get() const;
    void Sort();
    Subsystems subsystems;

private:
    std::unordered_map<std::type_index, SubsystemInterface*> _subsystemsLUT;
};

template <typename T, typename... Args>
inline void SubsystemLibrary::Add(Args&&... a_Args)
{
    auto systemPtr            = subsystems.emplace_back(std::make_unique<T>(std::forward<Args>(a_Args)...)).get();
    _subsystemsLUT[typeid(T)] = systemPtr;
}

template <typename T>
inline void SubsystemLibrary::Remove()
{
    std::erase_if(
        subsystems,
        [](auto& a_Subsystem) { typeid(*a_Subsystem) == typeid(T); });
    _subsystemsLUT.erase(typeid(T));
}

template <typename T>
const T& SubsystemLibrary::Get() const
{
    return *reinterpret_cast<T*>(_subsystemsLUT.find(typeid(T))->second);
}
}