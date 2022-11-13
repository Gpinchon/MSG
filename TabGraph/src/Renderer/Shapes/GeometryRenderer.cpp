/*
* @Author: gpinchon
* @Date:   2021-03-22 20:52:18
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-22 23:18:00
*/

#include <Renderer/Shapes/ShapeRenderer.hpp>
#include <Shapes/Geometry.hpp>

#if RENDERINGAPI == OpenGL
#include <Driver/OpenGL/Renderer/Shapes/GeometryRenderer.hpp>
#endif



namespace TabGraph::Renderer {
void Render(std::shared_ptr<Shapes::Geometry> geometry, bool doubleSided)
{
    static_cast<GeometryRenderer&>(geometry->GetRenderer()).Render(doubleSided);
}
};