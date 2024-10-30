#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Component/BoundingVolume.hpp>
#include <SG/POSymetricMatrix.hpp>

#include <glm/vec3.hpp>

#include <array>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class POTriangle;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace std {
template <>
struct hash<TabGraph::SG::POTriangle> {
    size_t operator()(TabGraph::SG::POTriangle const& bar) const;
};
}
namespace TabGraph::SG {
class POTriangle {
public:
    bool operator==(const POTriangle& a_Lhs) const;
    std::array<uint64_t, 3> vertice        = { uint64_t(-1), uint64_t(-1), uint64_t(-1) };
    mutable POSymetricMatrix quadricMatrix = {};
    mutable Component::Plane plane         = {};
    mutable glm::vec3 originalNormal       = {};
    mutable bool collapsed                 = false;
};
}
