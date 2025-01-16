#pragma once

#include <Renderer/OGL/UniformBuffer.hpp>

#include <memory>

namespace TabGraph::Renderer {
struct UniformBufferUpdateI {
    virtual ~UniformBufferUpdateI() = default;
    virtual void operator()()       = 0;
};

struct UniformBufferUpdate {
    template <typename T>
    UniformBufferUpdate(UniformBufferT<T>& a_UniformBuffer)
        : _buffer(a_UniformBuffer.buffer)
        , _size(sizeof(T))
        , _offset(a_UniformBuffer.offset)
        , _data(std::make_shared<T>(a_UniformBuffer.GetData()))

    {
        a_UniformBuffer.needsUpdate = false;
    }
    void operator()() const;

private:
    std::shared_ptr<RAII::Buffer> _buffer;
    const uint32_t _size   = 0;
    const uint32_t _offset = 0;
    std::shared_ptr<void> _data;
};
}