#pragma once

#include <MSG/OGLBuffer.hpp>

#include <GL/glew.h>

#include <cassert>
#include <cstring>
#include <type_traits>
#include <vector>

namespace Msg {
template <typename T, typename = void>
struct has_not_equal_to : std::false_type { };

template <typename T>
struct has_not_equal_to<T,
    std::void_t<decltype(std::declval<T>() != std::declval<T>())>>
    : std::true_type { };

void UpdateOGLTypedBuffer(const OGLBuffer& a_Buffer, const size_t& a_Offset, const size_t& a_Size, void* a_Data);
void ReadOGLTypedBuffer(const OGLBuffer& a_Buffer, const size_t& a_Offset, const size_t& a_Size, void* a_Data);

template <typename T>
class OGLTypedBufferArray : public OGLBuffer {
public:
    using value_type                 = T;
    static constexpr auto value_size = sizeof(value_type);
    OGLTypedBufferArray(OGLContext& a_Ctx, const size_t& a_Count, const value_type* a_Data = {})
        : OGLBuffer(a_Ctx, sizeof(value_type) * a_Count, a_Data, GL_DYNAMIC_STORAGE_BIT)
    {
        _data = a_Data == nullptr ? std::vector<value_type>(a_Count) : std::vector<value_type>(a_Data, a_Data + a_Count);
    };
    const size_t GetCount() const { return _data.size(); }
    const value_type& Get(const size_t& a_Index) const { return _data.at(a_Index); }
    const value_type& Set(const size_t& a_Index, const value_type& a_Data);
    const void Set(const size_t& a_Index, const size_t& a_Count, const value_type* a_Data);
    /** @brief read back buffer's content into CPU side storage */
    void Read();
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
    OGLTypedBuffer(const OGLTypedBuffer<T>& a_Other)
        : OGLTypedBuffer(a_Other.context, a_Other.Get())
    {
    }
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

template <typename T>
inline const void OGLTypedBufferArray<T>::Set(const size_t& a_Index, const size_t& a_Count, const value_type* a_Data)
{
    assert((a_Index + a_Count) <= _data.size() && "Index out of range!");
    for (size_t index = a_Index; index < a_Index + a_Count; index++)
        Set(index, a_Data[index]);
    return void();
}

template <typename T>
inline void OGLTypedBufferArray<T>::Read()
{
    ReadOGLTypedBuffer(*this, 0, _data.size() * value_size, _data.data());
}
}