#pragma once

#include <MSG/OGLBuffer.hpp>

#include <GL/glew.h>

#include <cstring>
#include <type_traits>
#include <vector>

namespace MSG {
template <typename T, typename = void>
struct has_not_equal_to : std::false_type { };

template <typename T>
struct has_not_equal_to<T,
    std::void_t<decltype(std::declval<T>() != std::declval<T>())>>
    : std::true_type { };

void UpdateOGLTypedBuffer(const OGLBuffer& a_Buffer, const size_t& a_Offset, const size_t& a_Size, void* a_Data);

template <typename T>
class OGLTypedBufferArray : public OGLBuffer {
public:
    using value_type                 = T;
    static constexpr auto value_size = sizeof(value_type);
    OGLTypedBufferArray(OGLContext& a_Ctx, const size_t& a_Count, const value_type* a_Data = {})
        : OGLBuffer(a_Ctx, sizeof(value_type) * a_Count, a_Data, GL_DYNAMIC_STORAGE_BIT)
    {
        _data = a_Data == nullptr ? std::vector<value_type> { a_Count } : std::vector<value_type> { a_Data, a_Data + a_Count };
    };
    const value_type& Get(const size_t& a_Index) const { return _data.at(a_Index); }
    const value_type& Set(const size_t& a_Index, const value_type& a_Data);
    void Update()
    {
        if (!needsUpdate)
            return;
        UpdateOGLTypedBuffer(*this, 0, sizeof(value_type) * _data.size(), _data.data());
        needsUpdate = false;
    }
    bool needsUpdate = false;

private:
    std::vector<value_type> _data;
};

template <typename T>
class OGLTypedBuffer : public OGLTypedBufferArray<T> {
public:
    using value_type                 = T;
    static constexpr auto value_size = sizeof(value_type);
    OGLTypedBuffer(OGLContext& a_Ctx, const value_type& a_Data = {})
        : OGLTypedBufferArray<T>(a_Ctx, 1, &a_Data) { };
    const value_type& Get() const { return OGLTypedBufferArray<T>::Get(0); }
    const value_type& Set(const value_type& a_Data) { return OGLTypedBufferArray<T>::Set(0, a_Data); }
};

template <typename T>
auto OGLTypedBufferArray<T>::Set(const size_t& a_Index, const value_type& a_Data) -> const value_type&
{
    bool different = false;
    if constexpr (has_not_equal_to<value_type>::value) {
        different = Get(a_Index) != a_Data;
    } else {
        different = std::memcmp(&Get(a_Index), &a_Data, value_size) != 0;
    }
    if (different) {
        _data.at(a_Index) = a_Data;
        needsUpdate       = true;
    }
    return _data.at(a_Index);
}

}