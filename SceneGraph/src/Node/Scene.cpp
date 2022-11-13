/*
* @Author: gpinchon
* @Date:   2021-06-19 15:04:37
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-19 15:04:37
*/

#include <Nodes/Scene.hpp>

#if RENDERINGAPI == OpenGL
#include <Driver/OpenGL/Renderer/SceneRenderer.hpp>
#endif

namespace TabGraph::Nodes {
Scene::Scene()
	: Inherit()
{
	static auto s_sceneNbr{ 0u };
	SetName("Scene_" + std::to_string(++s_sceneNbr));
}
};