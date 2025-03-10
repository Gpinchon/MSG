#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <string>
#include <typeindex>

////////////////////////////////////////////////////////////////////////////////
// Class definition
////////////////////////////////////////////////////////////////////////////////
namespace MSG::Core {
/**
 * @brief Use Curiously Recurring Template to generate proper type casting and Accept. Inspired by VulkanSceneGraph.
 * @tparam ParentClass : the class to inherit from
 * @tparam SubClass : the class that will inherit from ParentClass
 */
template <class ParentClass, class SubClass>
class Inherit : public ParentClass {
public:
    // enable access to parent's constructors
    using ParentClass::ParentClass;

    // Overriden functions
    /** @return sizeof(SubClass) */
    inline size_t GetByteSize() const override
    {
        return sizeof(SubClass);
    }
    /** @return the type_info of SubClass */
    inline const std::type_info& GetTypeInfo() const override
    {
        return typeid(SubClass);
    }
    /** @return the name of the type_info of SubClass */
    inline std::string GetClassName() const override
    {
        return typeid(SubClass).name();
    }
    /**
     * @param typeIndex : the type to test
     * @return true if typeid(SubClass) == typeIndex
     */
    inline bool IsOfType(const std::type_index& typeIndex) const override
    {
        return typeIndex == typeid(SubClass);
    }
    /**
     * @brief Goes up the inheritances to find out if one of the parent class is of this type
     * @param typeIndex : the type to test
     * @return true if SubClass IsOfType(typeIndex), else checks if ParentClass is compatible
     */
    inline bool IsCompatible(const std::type_index& typeIndex) const override
    {
        return typeIndex == typeid(SubClass) ? true : ParentClass::IsCompatible(typeIndex);
    }
};
};
