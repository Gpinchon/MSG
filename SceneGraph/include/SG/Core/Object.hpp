#pragma once

#include <SG/Visitor/Visitor.hpp>

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

    static std::ostream& SerializeData(std::ostream& a_Ostream, const std::string& a_Name, const std::byte* a_Data, const size_t& a_Size);
    template<typename T>
    static auto& SerializeProperty(std::ostream& a_Ostream, const std::string& a_Name, const T& a_Value) {
        return SerializeProperty(a_Ostream, a_Name, std::to_string(a_Value));
    }
    template<>
    static auto& SerializeProperty(std::ostream& a_Ostream, const std::string& a_Name, const std::string& a_Value) {
        return SerializeData(a_Ostream, a_Name, (const std::byte*)a_Value.data(), a_Value.length());
    }
    inline virtual std::ostream& Serialize(std::ostream& a_Ostream) const {
        SerializeProperty(a_Ostream, "Type", std::string(GetTypeInfo().name()));
        SerializeProperty(a_Ostream, "Id", GetId());
        SerializeProperty(a_Ostream, "Name", GetName());
        return a_Ostream;
    }
    friend std::ostream& operator<<(std::ostream& a_Ostream, const Object& a_Object) {
        return a_Object.Serialize(a_Ostream);
    }

private:
    uint32_t _id { 0 };
    std::string _name { "" };
};
};
