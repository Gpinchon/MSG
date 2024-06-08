#pragma once

#ifdef WIN32
#include <Renderer/OGL/Win32/Context.hpp>
#elif defined __linux__
#include <Renderer/OGL/Unix/Context.hpp>
#endif

#include <functional>
#include <memory>

namespace TabGraph::Renderer::RAII {
template <typename Type, typename... Args>
std::shared_ptr<Type> MakePtr(Context& a_Context, Args&&... a_Args)
{
    std::pmr::polymorphic_allocator<Type> al(&a_Context.memoryResource);
    Type* ptr = al.allocate(1);
    a_Context.PushImmediateCmd([ptr, &a_Args...]() { new (ptr) Type(std::forward<Args>(a_Args)...); }, true);
    return {
        ptr, [&context = a_Context](Type* a_Ptr) mutable {
            context.PushCmd([ptr = a_Ptr, mr = &context.memoryResource] {
                std::pmr::polymorphic_allocator<Type> al(mr);
                al.destroy(ptr);
                al.deallocate(ptr, 1);
            });
        }
    };
}

}
