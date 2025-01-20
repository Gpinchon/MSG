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
namespace MSG::SG {
class POVertex;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace std {
template<typename T>
struct hash;
template <>
struct hash<MSG::SG::POVertex> {
    size_t operator()(MSG::SG::POVertex const& bar) const;
};
}

namespace MSG::SG {
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
