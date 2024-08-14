#pragma once

#include <Renderer/OGL/RAII/Wrapper.hpp>

#include <cstring>
#include <type_traits>
#include <vector>

namespace TabGraph::Renderer::RAII {
class Buffer;
}

template <typename T, typename = void>
struct has_not_equal_to : std::false_type { };

template <typename T>
struct has_not_equal_to<T,
    std::void_t<decltype(std::declval<T>() != std::declval<T>())>>
    : std::true_type { };

namespace TabGraph::Renderer {
struct UniformBuffer {
    UniformBuffer(Context& a_Context, const size_t& a_Size, const std::byte* a_Data);
    UniformBuffer(const std::shared_ptr<RAII::Buffer>& a_Buffer)
        : buffer(a_Buffer)
    {
    }
    bool needsUpdate                     = true;
    uint32_t offset                      = 0;
    std::shared_ptr<RAII::Buffer> buffer = nullptr;
};

template <typename T>
struct UniformBufferT : UniformBuffer {
    using value_type                 = T;
    static constexpr auto value_size = sizeof(value_type);
    UniformBufferT(Context& a_Context, const value_type& a_Data = {})
        : UniformBuffer(a_Context, value_size, (std::byte*)&a_Data)
        , _data(a_Data)
    {
    }
    auto& GetData() const
    {
        return _data;
    }
    value_type& SetData(const value_type& a_Data)
    {
        bool different = false;
        if constexpr (has_not_equal_to<value_type>::value) {
            different = _data != a_Data;
        } else {
            different = std::memcmp(&_data, &a_Data, value_size) != 0;
        }
        if (different) {
            _data       = a_Data;
            needsUpdate = true;
        }
        return _data;
    }

protected:
    value_type _data;
};
}
