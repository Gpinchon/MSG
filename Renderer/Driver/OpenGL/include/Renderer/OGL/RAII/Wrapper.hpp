#pragma once

#include <Renderer/OGL/Win32/Context.hpp>

#include <functional>
#include <memory>

namespace TabGraph::Renderer::RAII {
template <typename T>
using Wrapper = std::shared_ptr<T>;

template <typename Type, typename... Args>
Wrapper<Type> MakeWrapper(Context& a_Context, Args&&... a_Args)
{
    Type* ptr = nullptr;
    a_Context.PushImmediateCmd([&ptr, &a_Args...]() { ptr = new Type(std::forward<Args>(a_Args)...); }, true);
    return {
        ptr, [&context = a_Context](Type* a_Ptr) mutable {
            context.PushCmd([ptr = a_Ptr] { delete ptr; });
        }
    };
}

}
