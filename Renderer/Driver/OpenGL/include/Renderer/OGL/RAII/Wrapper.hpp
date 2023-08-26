#pragma once

#include <Renderer/OGL/Win32/Context.hpp>

namespace TabGraph::Renderer::RAII {
template <typename T>
struct Wrapper {
    using type       = Wrapper<T>;
    using value_type = T;
    template <typename... Args>
    Wrapper(Context* a_Context, Args&&... a_Args)
        : context(a_Context)
    {
        context->PushResourceCreationCmd(
            [this, &a_Args...] {
                data = new value_type(std::forward<Args>(a_Args)...);
            },
            true);
    }
    Wrapper(Wrapper&& a_Other)
    {
        std::swap(context, a_Other.context);
        std::swap(data, a_Other.data);
    }
    Wrapper()               = default;
    Wrapper(const Wrapper&) = delete;
    ~Wrapper()
    {
        if (data != nullptr)
            context->PushResourceDestructionCmd(
                [data = data] {
                    delete data;
                },
                false);
    }
    operator value_type&() const { return *data; }
    operator value_type&() { return *data; }
    template <typename... Args>
    void reset(Context* a_Context, Args&&... a_Args)
    {
        if (data != nullptr)
            context->PushResourceDestructionCmd(
                [data = data] {
                    delete data;
                },
                false);
        context = a_Context;
        data    = nullptr;
        if (context != nullptr)
            context->PushResourceCreationCmd(
                [this, &a_Args...] {
                    data = new value_type(std::forward<Args>(a_Args)...);
                },
                true);
    }
    type& operator=(type&& a_Other)
    {
        if (data != nullptr)
            context->PushResourceDestructionCmd(
                [data = data] {
                    delete data;
                },
                false);
        // context         = a_Other.context;
        // data            = a_Other.data;
        // a_Other.context = nullptr;
        // a_Other.data    = nullptr;
        std::swap(context, a_Other.context);
        std::swap(data, a_Other.data);
        return *this;
    }
    value_type* operator->()
    {
        return data;
    }
    value_type& operator*()
    {
        return *data;
    }

private:
    Context* context = nullptr;
    value_type* data = nullptr;
};
}
