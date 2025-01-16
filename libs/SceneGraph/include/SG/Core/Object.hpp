#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Component/Name.hpp>

#include <array>
#include <cstdint>
#include <string>
#include <typeindex>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Object {
public:
    Object();
    Object(const std::string& a_Name)
        : Object()
    {
        _name = a_Name;
    }
    Object(const Object& a_Other)
        : Object()
    {
        _name = a_Other._name;
    }
    virtual ~Object() = default;

    auto& GetName() const
    {
        return _name;
    }
    void SetName(const std::string& a_Name)
    {
        _name = a_Name;
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

private:
    Component::Name _name;
    uint32_t _id = 0;
};
};
