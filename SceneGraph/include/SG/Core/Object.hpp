#pragma once

#include <SG/Core/Name.hpp>

#include <SG/Visitor/Visitor.hpp>

#include <string>
#include <typeindex>
#include <iosfwd>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
#define OBJECT_COMPONENTS SG::Name

/** @return the total nbr of Objects created since start-up */
uint32_t &GetObjectNbr();

template<typename RegistryType>
inline auto CreateObject(const RegistryType& a_Registry) {
    auto entity = a_Registry->CreateEntity();
    entity.AddComponent<SG::Name>("Object_" + std::to_string(++GetObjectNbr()));
    return entity;
}

class Object {
public:
    Object();
    Object(const std::string& name)
        : Object()
    {
        _name = name;
    }
    Object(const Object& other)
        : Object(other._name)
    {
    }
    inline auto GetId() const
    {
        return _id;
    }
    inline auto &GetName() const
    {
        return _name;
    }

    inline void SetId(uint32_t id)
    {
        _id = id;
    }
    inline void SetName(const std::string& name)
    {
        _name = name;
    }

    /** @return sizeof(Object) */
    inline virtual size_t GetByteSize() const
    {
        return sizeof(Object);
    }
    /** @return typeid(Object) */
    inline virtual const std::type_info& GetTypeInfo() const
    {
        return typeid(Object);
    }
    /** @return typeid(Object).name() */
    inline virtual std::string GetClassName() const
    {
        return typeid(Object).name();
    }
    /**  @return typeIndex == typeid(Object) */
    inline virtual bool IsOfType(const std::type_index& typeIndex) const
    {
        return typeIndex == typeid(Object);
    }
    /**  @return typeIndex == typeid(Object) */
    inline virtual bool IsCompatible(const std::type_index& typeIndex) const
    {
        return typeIndex == typeid(Object);
    }
    /**
     * @brief calls operator()
     * @param visitor : a Visitor to visit Object
    */
    inline virtual void Accept(Visitor& visitor)
    {
        return visitor(*this);
    }

private:
    uint32_t _id { 0 };
    std::string _name { "" };
};
};
