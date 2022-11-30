/*
* @Author: gpinchon
* @Date:   2021-06-19 15:01:38
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-26 23:39:40
*/

#include <cstdint>

namespace TabGraph::SG::Node {
uint32_t & GetNbr() {
    static auto s_Nbr = 0u;
    return s_Nbr;
}
}
