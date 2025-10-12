#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/PO/SymetricMatrix.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace Msg::PO {
class Vertex;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace std {
template <typename T>
struct hash;
template <>
struct hash<Msg::PO::Vertex> {
    size_t operator()(Msg::PO::Vertex const& bar) const;
};
}

namespace Msg::PO {
using posType = glm::vec3;
class Vertex {
public:
    Vertex() = default;
    Vertex(const posType& a_Pos)
        : position(a_Pos)
    {
    }
    bool operator!=(const Vertex& a_Rhs) const;
    bool operator==(const Vertex& a_Rhs) const;
    posType position                     = posType(std::numeric_limits<posType::value_type>::infinity());
    mutable SymetricMatrix quadricMatrix = {};
};
}
