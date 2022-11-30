/*
* @Author: gpinchon
* @Date:   2020-11-25 23:58:13
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:46:23
*/

#include <cstdint>

namespace TabGraph::SG::Light::Spot {
uint32_t& GetNbr() {
	static auto s_Nbr = 0u;
	return s_Nbr;
}
}
