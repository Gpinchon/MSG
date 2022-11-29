#include <SG/Node/NodeGroup.hpp>

namespace TabGraph::SG {
uint32_t& GetNodeGroupNbr() {
    static auto s_Nbr = 0u;
    return s_Nbr;
}
NodeGroup::NodeGroup() : Inherit() {
    static auto s_Nbr{ 0u };
    SetName("NodeGroup_" + std::to_string(++s_Nbr));
}
}