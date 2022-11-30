/*
* @Author: gpinchon
* @Date:   2019-07-15 10:36:36
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:46:31
*/

#include <SG/Camera/FPS.hpp>

namespace TabGraph::SG::FPSCamera {
uint32_t& GetNbr()
{
    static auto s_Nbr = 0u;
    return s_Nbr;
}
}
