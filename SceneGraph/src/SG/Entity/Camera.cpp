/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-21 21:59:34
*/

#include <SG/Entity/Camera.hpp>

namespace TabGraph::SG::Camera {
uint32_t& GetNbr()
{
    static auto s_Nbr = 0u;
    return s_Nbr;
}
}
