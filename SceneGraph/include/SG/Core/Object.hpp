#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <string>
#include <typeindex>
#include <iosfwd>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Object {
public:
    Object();
    Object(const std::string& name) : Object()
    {
        _name = name;
    }
    Object(const Object& other) : Object(other._name)
    {}

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

private:
    uint32_t _id { 0 };
    std::string _name { "" };
};
};
