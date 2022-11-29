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
    Parent(const ECS::DefaultRegistry::EntityIDType& a_Parent = -1) { _parent = a_Parent; }

private:
    ECS::DefaultRegistry::EntityIDType _parent;
};
}
