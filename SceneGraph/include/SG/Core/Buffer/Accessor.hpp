/*
 * @Author: gpinchon
 * @Date:   2020-06-18 13:31:08
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-07-21 21:49:52
 */
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Core/Buffer/Buffer.hpp>
#include <SG/Core/Buffer/Iterator.hpp>
#include <SG/Core/Buffer/View.hpp>
#include <SG/Core/DataType.hpp>
#include <SG/Core/Inherit.hpp>
#include <SG/Core/Object.hpp>
#include <SG/Core/Property.hpp>

#include <cassert>
#include <memory>
#include <stdexcept>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
template <typename>
class TypedBufferAccessor;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class BufferAccessor : public Inherit<Object, BufferAccessor> {
public:
    PROPERTY(int, ByteOffset, 0);
    PROPERTY(size_t, Size, 0);
    READONLYPROPERTY(uint8_t, ComponentTypeSize, 0);
    READONLYPROPERTY(DataType, ComponentType, DataType::Unknown);
    PROPERTY(uint8_t, ComponentNbr, 0);
    PROPERTY(std::shared_ptr<BufferView>, BufferView, );
    PROPERTY(bool, Normalized, false);

public:
    BufferAccessor()
        : Inherit()
    {
        SetName("Buffer::Accessor_" + std::to_string(++s_bufferAccessorNbr));
    }
    BufferAccessor(
        const std::shared_ptr<BufferView>& a_BufferView,
        const int& a_ByteOffset,
        const size_t& a_Size,
        const DataType& a_DataType,
        const uint8_t& a_ComponentsNbr)
        : BufferAccessor()
    {
        SetBufferView(a_BufferView);
        SetByteOffset(a_ByteOffset);
        SetSize(a_Size);
        SetComponentType(a_DataType);
        SetComponentNbr(a_ComponentsNbr);
    }
    BufferAccessor(
        const int& byteOffset,
        const size_t& size,
        const DataType& componentType,
        const uint8_t& componentsNbr)
        : BufferAccessor()
    {
        SetByteOffset(byteOffset);
        SetSize(size);
        SetComponentType(componentType);
        SetComponentNbr(componentsNbr);
        SetBufferView(std::make_shared<BufferView>(0, GetByteOffset() + GetByteLength()));
    }

    size_t GetByteLength() const
    {
        return GetDataByteSize() * GetSize();
    }

    void SetComponentType(const DataType& a_Type)
    {
        _SetComponentType(a_Type);
        _SetComponentTypeSize(DataTypeSize(a_Type));
    }
    inline size_t GetDataByteSize() const
    {
        return size_t(GetComponentTypeSize()) * GetComponentNbr();
    }

    template <typename T>
    inline auto& GetComponent(size_t a_Index, size_t a_ComponentIndex)
    {
#ifndef NDEBUG
        assert(sizeof(T) == GetComponentTypeSize());
        assert(a_ComponentIndex < GetComponentNbr());
#endif
        T* ptr = reinterpret_cast<T*>(&*(begin() + a_Index));
        return *(ptr + a_ComponentIndex);
    }

    template <typename T>
    inline auto& GetComponent(size_t a_Index, size_t a_ComponentIndex) const
    {
#ifndef NDEBUG
        assert(sizeof(T) == GetComponentTypeSize());
        assert(a_ComponentIndex < GetComponentNbr());
#endif
        T* ptr = reinterpret_cast<T*>(&*(begin() + a_Index));
        return *(ptr + a_ComponentIndex);
    }

    inline const BufferIterator begin() const
    {
        const auto& bufferView = GetBufferView();
        const auto& buffer     = bufferView->GetBuffer();
        const auto byteOffset  = bufferView->GetByteOffset() + GetByteOffset();
        const auto byteStride  = bufferView->GetByteStride() > 0 ? bufferView->GetByteStride() : GetDataByteSize();
        return { &buffer->at(byteOffset), byteStride };
    }
    inline BufferIterator begin()
    {
        const auto& bufferView = GetBufferView();
        const auto& buffer     = bufferView->GetBuffer();
        const auto byteOffset  = bufferView->GetByteOffset() + GetByteOffset();
        const auto byteStride  = bufferView->GetByteStride() > 0 ? bufferView->GetByteStride() : GetDataByteSize();
        return { &buffer->at(byteOffset), byteStride };
    }

    inline const BufferIterator end() const
    {
        return begin() + GetSize();
    }
    inline BufferIterator end()
    {
        return begin() + GetSize();
    }

    inline auto& at(const size_t& a_Index) const
    {
        return *(begin() + a_Index);
    }
    inline auto& at(const size_t& a_Index)
    {
        return *(begin() + a_Index);
    }

    bool empty() const
    {
        return GetBufferView() == nullptr || GetBufferView()->empty();
    }

    template <typename T>
    inline operator TypedBufferAccessor<T>() const
    {
#ifndef NDEBUG
        assert(GetDataByteSize() == sizeof(T));
#endif
        return TypedBufferAccessor<T>(GetBufferView(), GetByteOffset(), GetSize());
    }

private:
    template <typename>
    friend class TypedBufferAccessor;
    static size_t s_bufferAccessorNbr;
    static size_t s_typedBufferAccessorNbr;
};

/**
 * A typed view into a bufferView
 * A bufferView contains raw binary data.
 * An accessor provides a typed view into a bufferView or a subset of a bufferView.
 */
template <typename T>
class TypedBufferAccessor : public Inherit<Object, TypedBufferAccessor<T>> {
public:
    PROPERTY(std::shared_ptr<BufferView>, BufferView, );
    /**
     * @brief Is the data to be normalized by OpenGL ?
     */
    PROPERTY(bool, Normalized, false);
    /**
     * @brief Byte offset inside the BufferView
     */
    PROPERTY(size_t, ByteOffset, 0);
    /**
     * @brief Number of data chunks
     */
    PROPERTY(size_t, Size, 0);

public:
    TypedBufferAccessor()
        : Inherit<Object, TypedBufferAccessor<T>>()
    {
        Object::SetName("Buffer::TypedAccessor_" + std::to_string(++BufferAccessor::s_typedBufferAccessorNbr));
    }
    TypedBufferAccessor(const std::shared_ptr<BufferView>& a_BufferView)
        : TypedBufferAccessor()
    {
        SetSize(a_BufferView->GetByteSize() / sizeof(T));
        SetBufferView(a_BufferView);
    }
    TypedBufferAccessor(const std::shared_ptr<BufferView>& a_BufferView, const size_t& a_ByteOffset, const size_t& a_Size)
        : TypedBufferAccessor(a_BufferView)
    {
        SetSize(a_Size);
        SetByteOffset(a_ByteOffset);
    }
    /**
     * @brief Use this constructor to allocate a new BufferView
     * @param count : the number of data chunks
     */
    TypedBufferAccessor(const size_t a_Size)
        : TypedBufferAccessor(std::make_shared<BufferView>(0, a_Size * sizeof(T)))
    {
    }
    inline auto GetTypeSize() const noexcept { return sizeof(T); }
    inline bool empty() const noexcept { return GetSize() == 0; }
    inline TypedBufferIterator<T> begin()
    {
        const auto& bufferView = GetBufferView();
        const auto& buffer     = bufferView->GetBuffer();
        const auto byteOffset  = bufferView->GetByteOffset() + GetByteOffset();
        const auto byteStride  = bufferView->GetByteStride() > 0 ? bufferView->GetByteStride() : sizeof(T);
        return { &buffer->at(byteOffset), byteStride };
    }
    inline const TypedBufferIterator<T> begin() const
    {
        const auto& bufferView = GetBufferView();
        const auto& buffer     = bufferView->GetBuffer();
        const auto byteOffset  = bufferView->GetByteOffset() + GetByteOffset();
        const auto byteStride  = bufferView->GetByteStride() > 0 ? bufferView->GetByteStride() : sizeof(T);
        return { &buffer->at(byteOffset), byteStride };
    }
    inline auto end() { return begin() + GetSize(); }
    inline const auto end() const { return begin() + GetSize(); }
    inline auto& at(size_t index)
    {
        assert(index < GetSize());
        return *(begin() + index);
    }
    inline auto& at(size_t index) const
    {
        assert(index < GetSize());
        return *(begin() + index);
    }
    inline operator std::vector<T>() const
    {
        return { begin(), end() };
    }
};
}
