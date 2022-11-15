/*
* @Author: gpinchon
* @Date:   2021-06-19 15:04:37
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-19 15:04:37
*/

#include <SG/Node/Scene.hpp>

namespace TabGraph::SG {
Scene::Scene()
	: Inherit()
{
	static auto s_sceneNbr{ 0u };
	SetName("Scene_" + std::to_string(++s_sceneNbr));
}
};