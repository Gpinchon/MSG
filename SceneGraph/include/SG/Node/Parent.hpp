#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <ECS/Registry.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward Declaration
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declaration
////////////////////////////////////////////////////////////////////////////////

namespace TabGraph::SG {
class Parent {
public:
    typedef ECS::DefaultRegistry::EntityIDType IDType;
    static constexpr auto DefaultValue = std::numeric_limits<ECS::DefaultRegistry::EntityIDType>::max();
    Parent() = default;
    Parent(const ECS::DefaultRegistry::EntityIDType& a_Parent) { reset(a_Parent); }
    Parent(const ECS::DefaultRegistry::EntityRefType& a_Parent) { reset(a_Parent); }
    void reset(const ECS::DefaultRegistry::EntityIDType& a_ParentID = DefaultValue) { _value = a_ParentID; }
    void reset(const ECS::DefaultRegistry::EntityRefType& a_Parent) { _value = ECS::DefaultRegistry::EntityIDType(a_Parent); }
    operator IDType() const { return _value; }
    operator bool() const { return _value != DefaultValue; }

private:
    IDType _value{ DefaultValue };
};
}
