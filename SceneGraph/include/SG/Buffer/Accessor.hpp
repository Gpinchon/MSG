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
#include <SG/Buffer/Iterator.hpp>
#include <SG/Buffer/View.hpp>
#include <SG/Core/Inherit.hpp>
#include <SG/Core/Object.hpp>
#include <SG/Core/Property.hpp>

#include <memory>
#include <vector>
#include <cassert>

////////////////////////////////////////////////////////////////////////////////
// Forward declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
template<typename>
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
        Int8, Uint8,
        Int16, Uint16, Float16,
        Int32, Uint32, Float32,
        MaxValue
    };
    PROPERTY(int, ByteOffset, 0);
    PROPERTY(size_t, Size, 0);
    PROPERTY(uint8_t, ComponentTypeSize, 0);
    PROPERTY(uint8_t, ComponentNbr, 0);
    PROPERTY(ComponentType, ComponentType, ComponentType::Unknown);
    PROPERTY(std::shared_ptr<BufferView>, BufferView, nullptr);
    PROPERTY(bool, Normalized, false);

public:
    BufferAccessor() : Inherit() {
        SetName("Buffer::Accessor_" + std::to_string(++s_bufferAccessorNbr));
    }
    BufferAccessor(const std::shared_ptr<BufferView>& bufferView, const int& byteOffset, const size_t& size, const ComponentType& componentType, const uint8_t& componentsNbr)
        : BufferAccessor()
    {
        SetBufferView(bufferView);
        SetByteOffset(byteOffset);
        SetSize(size);
        SetComponentType(componentType);
        SetComponentNbr(componentsNbr);
        SetComponentTypeSize(GetComponentTypeSize(GetComponentType()));
        if (GetBufferView() == nullptr)
            SetBufferView(std::make_shared<BufferView>(0, GetByteOffset() + GetDataByteSize()));
    }
    inline static uint8_t GetComponentTypeSize(const ComponentType& componentType) {
        switch (componentType)
        {
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
    inline size_t GetDataByteSize() const {
        return size_t(GetComponentTypeSize()) * GetComponentNbr();
    }
    template<typename T>
    inline auto begin()
    {
        assert(sizeof(T) == GetDataByteSize());
        const auto& bufferView{ GetBufferView() };
        return BufferIterator<T>(&bufferView->at(GetByteOffset()), bufferView->GetByteStride());
    }
    template<typename T>
    inline const auto begin() const
    {
        assert(sizeof(T) == GetDataByteSize());
        const auto& bufferView{ GetBufferView() };
        return BufferIterator<T>(&bufferView->at(GetByteOffset()), bufferView->GetByteStride());
    }

    template<typename T>
    inline const BufferIterator<T> end() const
    {
        return begin<T>() + GetSize();
    }
    template<typename T>
    inline BufferIterator<T> end()
    {
        return begin<T>() + GetSize();
    }
    template<typename T>
    inline const T& at(const size_t& index) const {
        assert(index < GetSize());
        return *(begin<T>() + index);
    }
    template<typename T>
    inline T& at(const size_t& index) {
        assert(index < GetSize());
        return *(begin<T>() + index);
    }
    bool empty() const {
        return GetSize() == 0;
    }

    template<typename T>
    inline operator TypedBufferAccessor<T>() const {
        assert(GetDataByteSize() == sizeof(T));
        return TypedBufferAccessor<T>(GetBufferView(), GetByteOffset(), GetSize());
    }

    inline virtual std::ostream& Serialize(std::ostream& a_Ostream) const override {
        Inherit::Serialize(a_Ostream);
        Object::SerializeProperty(a_Ostream, "ByteOffset", GetByteOffset());
        Object::SerializeProperty(a_Ostream, "Size", GetSize());
        Object::SerializeProperty(a_Ostream, "ComponentTypeSize", GetComponentTypeSize());
        Object::SerializeProperty(a_Ostream, "ComponentNbr", GetComponentNbr());
        Object::SerializeProperty(a_Ostream, "ComponentType", (int)GetComponentType());
        Object::SerializeProperty(a_Ostream, "BufferView", GetBufferView() ? GetBufferView()->GetId() : -1);
        Object::SerializeProperty(a_Ostream, "Normalized", GetNormalized());
        return a_Ostream;
    }

private:
    template<typename> friend class TypedBufferAccessor;
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
    PROPERTY(std::shared_ptr<BufferView>, BufferView, nullptr);
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
        : Inherit()
    {
        SetName("Buffer::TypedAccessor_" + std::to_string(++BufferAccessor::s_typedBufferAccessorNbr));
    }
    TypedBufferAccessor(std::shared_ptr<BufferView> bufferView)
        : TypedBufferAccessor()
    {
        SetSize(bufferView->GetByteSize() / sizeof(T));
        SetBufferView(bufferView);
    }
    TypedBufferAccessor(const std::shared_ptr<BufferView>& bufferView, const size_t& byteOffset, const size_t& size)
        : TypedBufferAccessor(bufferView)
    {
        SetSize(size);
        SetByteOffset(byteOffset);
        SetBufferView(bufferView);
    }
    /**
     * @brief Use this constructor to allocate a new BufferView
     * @param count : the number of data chunks
    */
    TypedBufferAccessor(const size_t size)
        : TypedBufferAccessor(std::make_shared<BufferView>(size * sizeof(T)))
    {}
    inline auto GetTypeSize() const noexcept { return sizeof(T); }
    inline bool empty() const noexcept { return GetSize() == 0; }
    inline auto begin()
    {
        const auto& bufferView { GetBufferView() };
        return BufferIterator<T>(&bufferView->at(GetByteOffset()), bufferView->GetByteStride());
    }
    inline const auto begin() const
    {
        const auto& bufferView { GetBufferView() };
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
    inline operator std::vector<T>() const {
        return { begin(), end() };
    }
};
}
