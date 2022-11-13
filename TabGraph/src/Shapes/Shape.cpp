/*
* @Author: gpinchon
* @Date:   2021-06-29 22:43:55
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-29 22:43:55
*/

#include <Shapes\Shape.hpp>

using namespace TabGraph;

Shapes::Shape::~Shape()
{
}

void TabGraph::Renderer::ShapeRendererDeleter::operator()(ShapeRenderer* p) const
{
	delete p;
}
