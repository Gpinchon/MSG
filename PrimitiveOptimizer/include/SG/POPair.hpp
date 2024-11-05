#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/POVertex.hpp>

#include <array>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class POPair;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace std {
template <>
struct hash<TabGraph::SG::POPair> {
    size_t operator()(TabGraph::SG::POPair const& bar) const;
};
}

namespace TabGraph::SG {
class POPair {
public:
    POPair() = default;
    POPair(const uint64_t& a_V0, const uint64_t& a_V1);
    bool operator==(const POPair& a_Lhs) const;
    std::array<uint64_t, 2> vertice = { uint64_t(-1), uint64_t(-1) };
    mutable bool edge               = false;
    mutable double contractionCost  = 0;
    mutable glm::vec3 target        = {};
};
}
