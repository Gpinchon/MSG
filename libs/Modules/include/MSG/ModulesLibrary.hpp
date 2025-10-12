#pragma once

#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace Msg {
template <typename ModulesT>
class ModulesLibrary {
public:
    using type    = ModulesT;
    using Modules = std::vector<std::shared_ptr<type>>;
    template <typename T, typename... Args>
    void Add(Args&&... a_Args);
    template <typename T>
    void Remove();
    template <typename T>
    const T& Get() const;
    void Sort();
    void Clear();
    Modules modules;

private:
    std::unordered_map<std::type_index, type*> _modulesLUT;
};
}

#ifndef MODULES_LIBRARY_HPP
#define MODULES_LIBRARY_HPP
#include <MSG/ModulesLibrary.inl>
#endif
