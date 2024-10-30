#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/POSymetricMatrix.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class POVertex;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace std {
template <>
struct hash<TabGraph::SG::POVertex> {
    size_t operator()(TabGraph::SG::POVertex const& bar) const;
};
}

namespace TabGraph::SG {
using posType = glm::vec3;
using norType = glm::vec3;
using tanType = glm::vec4;
using texType = glm::vec2;
using colType = glm::vec3;
using joiType = glm::vec4;
using weiType = glm::vec4;
class POVertex {
public:
    bool operator!=(const POVertex& a_Rhs) const;
    bool operator==(const POVertex& a_Rhs) const;
    posType position                       = posType(std::numeric_limits<posType::value_type>::infinity());
    mutable norType normal                 = norType(std::numeric_limits<norType::value_type>::infinity());
    mutable tanType tangent                = tanType(std::numeric_limits<tanType::value_type>::infinity());
    mutable texType texCoord0              = texType(std::numeric_limits<texType::value_type>::infinity());
    mutable texType texCoord1              = texType(std::numeric_limits<texType::value_type>::infinity());
    mutable texType texCoord2              = texType(std::numeric_limits<texType::value_type>::infinity());
    mutable texType texCoord3              = texType(std::numeric_limits<texType::value_type>::infinity());
    mutable colType color                  = colType(std::numeric_limits<colType::value_type>::infinity());
    mutable joiType joints                 = joiType(std::numeric_limits<joiType::value_type>::infinity());
    mutable weiType weights                = weiType(std::numeric_limits<weiType::value_type>::infinity());
    mutable POSymetricMatrix quadricMatrix = {};
};
}
