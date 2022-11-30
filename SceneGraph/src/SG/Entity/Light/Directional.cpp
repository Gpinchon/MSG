/*
* @Author: gpinchon
* @Date:   2020-11-24 21:47:21
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-19 00:13:17
*/

#include <cstdint>

namespace TabGraph::SG::Light::Directional {
uint32_t& GetNbr() {
    static auto s_Nbr = 0u;
    return s_Nbr;
}
}
