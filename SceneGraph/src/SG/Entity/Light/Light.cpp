/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:46:24
*/

#include <cstdint>

namespace TabGraph::SG::Light {
uint32_t& GetNbr() {
	static auto s_Nbr = 0u;
	return s_Nbr;
}
}