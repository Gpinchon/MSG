/*
* @Author: gpinchon
* @Date:   2021-05-12 15:59:40
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-12 18:09:11
*/

#include <Renderer/Shapes/ShapeRenderer.hpp>
#include <Shapes/Shape.hpp>

#include <glm/mat4x4.hpp>

namespace TabGraph::Renderer {
void OnFrameBegin(std::shared_ptr<Shapes::Shape> surface, const Renderer::Options& options)
{
    surface->GetRenderer().OnFrameBegin(options);
}

void Render(std::shared_ptr<Shapes::Shape> surface, const Options& options)
{
    Render(surface, options, glm::mat4(1));
}

void Render(std::shared_ptr<Shapes::Shape> surface, const Options& options, const glm::mat4& parentTransform)
{
    Render(surface, options, parentTransform, glm::mat4(1));
}

void Render(std::shared_ptr<Shapes::Shape> surface, const Options& options, const glm::mat4& parentTransform, const glm::mat4& parentLastTransform)
{
    surface->GetRenderer().Render(options, parentTransform, parentLastTransform);
}

void OnFrameEnd(std::shared_ptr<Shapes::Shape> surface, const Renderer::Options& options)
{
    surface->GetRenderer().OnFrameEnd(options);
}
};