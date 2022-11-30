/*
* @Author: gpinchon
* @Date:   2019-07-16 08:55:52
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-19 10:01:35
*/

#include <SG/Camera/Orbit.hpp>
#include <SG/Node/Node.hpp>

namespace TabGraph::SG {
uint32_t& GetNbr() {
    static auto s_Nbr = 0u;
    return s_Nbr;
}
}