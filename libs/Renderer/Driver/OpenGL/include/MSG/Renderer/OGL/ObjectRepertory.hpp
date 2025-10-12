#pragma once

#include <string>
#include <unordered_map>

namespace Msg::Renderer {
template <typename T>
class ObjectRepertory {
public:
    auto& operator=(const T& a_Value) { return storage = a_Value; }
    auto& operator*() { return storage; }
    auto& operator->() { return &storage; }
    auto& operator[](const std::string& a_Key) { return subRepertories[a_Key]; }

private:
    T storage;
    std::unordered_map<std::string, ObjectRepertory<T>> subRepertories;
};
}