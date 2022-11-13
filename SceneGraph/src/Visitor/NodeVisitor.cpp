/*
* @Author: gpinchon
* @Date:   2021-06-20 13:28:58
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-20 13:28:58
*/

#include <Visitor/NodeVisitor.hpp>
#include <Node/Node.hpp>
#include <Node/NodeGroup.hpp>
#include <Node/Renderable.hpp>

namespace TabGraph::SG {
void NodeVisitor::_Traverse(Node& node)
{
	switch (mode)
	{
	case Mode::VisitOnce:
		return;
	case Mode::VisitParents:
		node.GetParent()->Accept(*this);
		break;
	default:
		break;
	}
}

void NodeVisitor::_Traverse(NodeGroup& group)
{
	if (mode == Mode::VisitChildren)
		for (auto& child : group.GetChildren()) child->Accept(*this);
	else _Traverse(static_cast<Node&>(group));
}
void NodeVisitor::_Traverse(Renderable& renderable)
{
	_Traverse(static_cast<Node&>(renderable));
}
}
