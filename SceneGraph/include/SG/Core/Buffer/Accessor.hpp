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
#include <SG/Core/Buffer/Iterator.hpp>
#include <SG/Core/Buffer/View.hpp>
#include <SG/Core/Inherit.hpp>
#include <SG/Core/Object.hpp>
#include <SG/Core/Property.hpp>

#include <cassert>
#include <memory>
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
    enum class ComponentType {
        Unknown = -1,
        Int8,
        Uint8,
        Int16,
        Uint16,
        Float16,
        Int32,
        Uint32,
        Float32,
        MaxValue
    };
    PROPERTY(int, ByteOffset, 0);
    PROPERTY(size_t, Size, 0);
    READONLYPROPERTY(uint8_t, ComponentTypeSize, 0);
    PROPERTY(uint8_t, ComponentNbr, 0);
    READONLYPROPERTY(ComponentType, ComponentType, ComponentType::Unknown);
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
        const ComponentType& a_ComponentType,
        const uint8_t& a_ComponentsNbr)
        : BufferAccessor()
    {
        SetBufferView(a_BufferView);
        SetByteOffset(a_ByteOffset);
        SetSize(a_Size);
        SetComponentType(a_ComponentType);
        SetComponentNbr(a_ComponentsNbr);
    }
    BufferAccessor(const int& byteOffset, const size_t& size, const ComponentType& componentType, const uint8_t& componentsNbr)
        : BufferAccessor()
    {
        SetByteOffset(byteOffset);
        SetSize(size);
        SetComponentType(componentType);
        SetComponentNbr(componentsNbr);
        SetBufferView(std::make_shared<BufferView>(0, GetByteOffset() + GetDataByteSize()));
    }
    inline static uint8_t GetComponentTypeSize(const ComponentType& componentType)
    {
        switch (componentType) {
        case ComponentType::Int8:
        case ComponentType::Uint8:
            return 1;
            break;
        case ComponentType::Int16:
        case ComponentType::Uint16:
        case ComponentType::Float16:
            return 2;
            break;
        case ComponentType::Int32:
        case ComponentType::Uint32:
        case ComponentType::Float32:
            return 4;
            break;
        }
        return 0;
    }
    void SetComponentType(const ComponentType& a_Type)
    {
        _SetComponentType(a_Type);
        _SetComponentTypeSize(GetComponentTypeSize(GetComponentType()));
    }
    inline size_t GetDataByteSize() const
    {
        return size_t(GetComponentTypeSize()) * GetComponentNbr();
    }

    template <typename T>
    inline auto& GetComponent(size_t a_Index, size_t a_ComponentIndex)
    {
#ifdef _DEBUG
        assert(sizeof(T) == GetComponentTypeSize());
        assert(a_ComponentIndex < GetComponentNbr());
#endif
        const auto& bufferView = GetBufferView();
        auto ptr               = ((T*)&bufferView->at(GetByteOffset())) + (a_Index * GetComponentNbr());
        return *(ptr + a_ComponentIndex);
    }

    template <typename T>
    inline auto& GetComponent(size_t a_Index, size_t a_ComponentIndex) const
    {
#ifdef _DEBUG
        assert(sizeof(T) == GetComponentTypeSize());
        assert(a_ComponentIndex < GetComponentNbr());
#endif
        const auto& bufferView = GetBufferView();
        auto ptr               = ((const T*)&bufferView->at(GetByteOffset())) + (a_Index * GetComponentNbr());
        return *(ptr + a_ComponentIndex);
    }

    template <typename T>
    inline const auto begin() const
    {
#ifdef _DEBUG
        assert(sizeof(T) == GetDataByteSize());
#endif
        const auto& bufferView = GetBufferView();
        return BufferIterator<T>(&bufferView->at(GetByteOffset()), bufferView->GetByteStride());
    }

    template <typename T>
    inline auto begin()
    {
#ifdef _DEBUG
        assert(sizeof(T) == GetDataByteSize());
#endif
        const auto& bufferView = GetBufferView();
        return BufferIterator<T>(&bufferView->at(GetByteOffset()), bufferView->GetByteStride());
    }

    template <typename T>
    inline const BufferIterator<T> end() const
    {
        return begin<T>() + GetSize();
    }
    template <typename T>
    inline BufferIterator<T> end()
    {
        return begin<T>() + GetSize();
    }

    template <typename T>
    inline const T& at(const size_t& index) const
    {
#ifdef _DEBUG
        assert(index < GetSize());
#endif
        return *(begin<T>() + index);
    }
    template <typename T>
    inline T& at(const size_t& index)
    {
#ifdef _DEBUG
        assert(index < GetSize());
#endif
        return *(begin<T>() + index);
    }

    bool empty() const
    {
        return GetSize() == 0;
    }

    template <typename T>
    inline operator TypedBufferAccessor<T>() const
    {
#ifdef _DEBUG
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
    PROPERTY(T, Min, 0);
    PROPERTY(T, Max, 0);

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
    TypedBufferAccessor(const std::shared_ptr<BufferView>& a_BufferView, const size_t& byteOffset, const size_t& size)
        : TypedBufferAccessor(a_BufferView)
    {
        SetSize(size);
        SetByteOffset(byteOffset);
    }
    /**
     * @brief Use this constructor to allocate a new BufferView
     * @param count : the number of data chunks
     */
    TypedBufferAccessor(const size_t size)
        : TypedBufferAccessor(std::make_shared<BufferView>(0, size * sizeof(T)))
    {
    }
    inline auto GetTypeSize() const noexcept { return sizeof(T); }
    inline bool empty() const noexcept { return GetSize() == 0; }
    inline auto begin()
    {
        const auto& bufferView = GetBufferView();
        return BufferIterator<T>(&bufferView->at(GetByteOffset()), bufferView->GetByteStride());
    }
    inline const auto begin() const
    {
        const auto& bufferView = GetBufferView();
        return BufferIterator<T>(&bufferView->at(GetByteOffset()), bufferView->GetByteStride());
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
