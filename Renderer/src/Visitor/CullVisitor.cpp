/*
* @Author: gpinchon
* @Date:   2021-06-20 13:29:18
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-20 13:29:18
*/

#include <Visitors/CullVisitor.hpp>
#include <Nodes/Renderable.hpp>

namespace TabGraph::Visitors {
void CullVisitor::_Visit(Nodes::Node& node)
{
}
void CullVisitor::_Visit(Nodes::Group& node)
{
}
void CullVisitor::_Visit(Nodes::Renderable& node)
{
	for (const auto& surface : node.GetShapes()) {
		Renderer::ShapeState state;
		state.surface = surface;
		state.transform = node.GetWorldTransformMatrix(); //TODO : Store Node's parent matrix during traversal
		_result.push_back(state);
	}
}
}

