#pragma once

#include <MSG/OGLContext.hpp>

#include <functional>
#include <memory>

namespace MSG::Renderer::RAII {
template <typename Type, typename ContextType, typename... Args>
std::shared_ptr<Type> MakePtr(ContextType& a_Context, Args&&... a_Args)
{
    std::pmr::polymorphic_allocator<Type> al(&a_Context.memoryResource);
    Type* ptr = std::allocator_traits<decltype(al)>::allocate(al, 1);
    a_Context.PushImmediateCmd([ptr, &a_Args...]() { new (ptr) Type(std::forward<Args>(a_Args)...); }, true);
    return {
        ptr, [&context = a_Context](Type* a_Ptr) mutable {
            context.PushCmd([ptr = a_Ptr, mr = &context.memoryResource] {
                std::pmr::polymorphic_allocator<Type> al(mr);
                std::allocator_traits<decltype(al)>::destroy(al, ptr);
                std::allocator_traits<decltype(al)>::deallocate(al, ptr, 1);
            });
        }
    };
}

}
