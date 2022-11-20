/*
* @Author: gpinchon
* @Date:   2021-06-20 13:28:58
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-20 13:28:58
*/

#include <SG/Visitor/NodeVisitor.hpp>
#include <SG/Node/Node.hpp>
#include <SG/Node/NodeGroup.hpp>

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
}
