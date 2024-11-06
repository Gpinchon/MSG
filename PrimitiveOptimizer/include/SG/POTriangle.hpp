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
using norType = glm::vec3;
using tanType = glm::vec4;
using texType = glm::vec2;
using colType = glm::vec3;
using joiType = glm::vec4;
using weiType = glm::vec4;
struct POVertexAttribs {
    mutable norType normal    = norType(std::numeric_limits<norType::value_type>::infinity());
    mutable tanType tangent   = tanType(std::numeric_limits<tanType::value_type>::infinity());
    mutable texType texCoord0 = texType(std::numeric_limits<texType::value_type>::infinity());
    mutable texType texCoord1 = texType(std::numeric_limits<texType::value_type>::infinity());
    mutable texType texCoord2 = texType(std::numeric_limits<texType::value_type>::infinity());
    mutable texType texCoord3 = texType(std::numeric_limits<texType::value_type>::infinity());
    mutable colType color     = colType(std::numeric_limits<colType::value_type>::infinity());
    mutable joiType joints    = joiType(std::numeric_limits<joiType::value_type>::infinity());
    mutable weiType weights   = weiType(std::numeric_limits<weiType::value_type>::infinity());
};
class POTriangle {
public:
    // WEIGHTS AND JOINTS AREN'T INTERPOLATED HERE !!!
    POVertexAttribs InterpolateAttribs(const glm::vec3& a_BarycentricCoord) const;
    bool operator==(const POTriangle& a_Lhs) const;
    std::array<uint64_t, 3> vertice        = { uint64_t(-1), uint64_t(-1), uint64_t(-1) };
    std::array<POVertexAttribs, 3> attribs = {};
    mutable POSymetricMatrix quadricMatrix = {};
    mutable Component::Plane plane         = {};
    mutable glm::vec3 originalNormal       = {};
    mutable bool collapsed                 = false;
};
}
