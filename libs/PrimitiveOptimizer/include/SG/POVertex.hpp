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
template<typename T>
struct hash;
template <>
struct hash<TabGraph::SG::POVertex> {
    size_t operator()(TabGraph::SG::POVertex const& bar) const;
};
}

namespace TabGraph::SG {
using posType = glm::vec3;
class POVertex {
public:
    POVertex() = default;
    POVertex(const posType& a_Pos)
        : position(a_Pos)
    {
    }
    bool operator!=(const POVertex& a_Rhs) const;
    bool operator==(const POVertex& a_Rhs) const;
    posType position                       = posType(std::numeric_limits<posType::value_type>::infinity());
    mutable POSymetricMatrix quadricMatrix = {};
};
}
